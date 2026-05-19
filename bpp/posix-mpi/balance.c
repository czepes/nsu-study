#include "balance.h"
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int finished = 0;
task_t *tasks = NULL;
int tasks_count = 0;
double global_result = 0.0;

int iter_active = 0;
int workers_active = 0;
int requesters_active = 0;
int responsers_active = 0;

pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t iter_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t tasks_change_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t few_tasks_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t workers_done_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t iter_start_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t iter_end_cond = PTHREAD_COND_INITIALIZER;

#define REQUEST_TAG 123
#define ACKNOWL_TAG 222
#define RESPNSE_TAG 321

#define GET_REQUEST 0
#define END_REQUEST 1

int modulo(int n, int mod) { return (n + mod) % mod; }

int compute_weight(int i, int iter, int rank, int size) {
  int w1 = abs(50 - i);
  int w2 = abs(rank - (iter % size));
  return w1 * w2 * WEIGHT_MUL;
}

void complete_task(int weight) {
  double s = 0.0;
  for (int j = 0; j < weight; j++) {
    s += sin(j);
  }

  pthread_mutex_lock(&task_mutex);
  global_result += s;
  pthread_mutex_unlock(&task_mutex);
}

int count_pending_tasks() {
  int count = 0;
  for (int i = 0; i < tasks_count; i++) {
    if (tasks[i].status == PENDING) {
      count++;
    }
  }
  return count;
}

// Worker-thread: Completes tasks
void *start_worker(void *arg) {
  thread_arg_t data = *((thread_arg_t *)arg);
  int size = data.size;
  int rank = data.rank;
  int tid = data.tid;

#if LOG_WORKER
  char self[FORMAT_SIZE];
  snprintf(self, sizeof(self), FULL_FORMAT, WORKER_NAME, rank, tid);
#endif

  pthread_mutex_lock(&iter_mutex);
  workers_active++;
  pthread_mutex_unlock(&iter_mutex);

  while (!finished) {
    int task_idx = -1;
    int task_weight = 0;

    // Take a task if there are any
    pthread_mutex_lock(&task_mutex);
    for (int i = 0; i < tasks_count; ++i) {
      if (tasks[i].status == PENDING) {
        task_idx = i;
        task_weight = tasks[i].weight;
        tasks[i].status = COMPLETED;
        break;
      }
    }

    int tasks_left = tasks_count - task_idx - 1;
    tasks_left = tasks_left < 0 ? 0 : tasks_left;

    if (tasks_left < REQT_THRESHOLD) {
#if LOG_WORKER
      printf("%s Few tasks left (%d)\n", self, tasks_left);
      fflush(stdout);
#endif
      pthread_cond_signal(&few_tasks_cond);
    }
    pthread_mutex_unlock(&task_mutex);

    if (task_idx == -1) {
#if LOG_WORKER
      printf("%s Waiting for new tasks\n", self);
      fflush(stdout);
#endif
      pthread_mutex_lock(&task_mutex);
      workers_active--;
      if (workers_active == 0) {
        pthread_cond_signal(&workers_done_cond);
      }
      pthread_cond_wait(&tasks_change_cond, &task_mutex);
      workers_active++;
      pthread_mutex_unlock(&task_mutex);
      continue;
    }

#if LOG_WORKER
    printf("%s [⇶] task[%d] = %d\n", self, task_idx, task_weight);
    fflush(stdout);
#endif
    complete_task(task_weight);
#if LOG_WORKER
    printf("%s [✔] task[%d] = %d\n", self, task_idx, task_weight);
    fflush(stdout);
#endif
  }

  pthread_mutex_lock(&iter_mutex);
  workers_active--;
  pthread_mutex_unlock(&iter_mutex);

#if LOG_WORKER
  printf("%s Exiting\n", self);
  fflush(stdout);
#endif
  return NULL;
}

#if LOG_RESPONSER
int send_tasks(char self[], int target) {
#else
int send_tasks(int target) {
#endif
  pthread_mutex_lock(&task_mutex);
  int remaining = count_pending_tasks();
  int to_send = remaining > RESP_THRESHOLD ? remaining * RESP_MULT : 0;

#if LOG_RESPONSER
  printf("%s Acking request from %02d ([↕] %d)\n", self, target, to_send);
  fflush(stdout);
#endif
  MPI_Send(&to_send, 1, MPI_INT, target, ACKNOWL_TAG, MPI_COMM_WORLD);

  if (to_send == 0) {
    pthread_mutex_unlock(&task_mutex);
    return 0;
  }

  int *send_weights = (int *)calloc(to_send, sizeof(int));
  int idx = 0;
  for (int i = 0; i < tasks_count && idx < to_send; i++) {
    if (tasks[i].status == PENDING) {
      send_weights[idx++] = tasks[i].weight;
      tasks[i].status = SENT;
    }
  }

#if LOG_RESPONSER
  printf("%s Response to %02d ([↑] %d)\n", self, target, to_send);
  fflush(stdout);
#endif
  pthread_mutex_unlock(&task_mutex);

  MPI_Send(send_weights, to_send, MPI_INT, target, RESPNSE_TAG, MPI_COMM_WORLD);
  free(send_weights);

  return to_send;
}

// responser-thread: Responses to other processes requests
void *start_responser(void *arg) {
  thread_arg_t data = *((thread_arg_t *)arg);
  int size = data.size;
  int rank = data.rank;
  int tid = data.tid;

  MPI_Status status;
  int request;

#if LOG_RESPONSER
  char self[FORMAT_SIZE];
  snprintf(self, sizeof(self), FULL_FORMAT, RESPONSER_NAME, rank, tid);
#endif

  pthread_mutex_lock(&iter_mutex);
  responsers_active++;
  pthread_mutex_unlock(&iter_mutex);

  while (!finished) {
#if LOG_RESPONSER
    printf("%s Waiting requests\n", self);
    fflush(stdout);
#endif

    MPI_Recv(&request, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST_TAG, MPI_COMM_WORLD,
             &status);

    if (request == END_REQUEST) {
      break;
    }

    if (request == GET_REQUEST) {
#if LOG_RESPONSER
      printf("%s Got request from %02d\n", self, status.MPI_SOURCE);
      fflush(stdout);
#endif

#if LOG_RESPONSER
      send_tasks(self, status.MPI_SOURCE);
#else
      send_tasks(status.MPI_SOURCE);
#endif
    }
  }

  pthread_mutex_lock(&iter_mutex);
  responsers_active--;
  pthread_mutex_unlock(&iter_mutex);

#if LOG_RESPONSER
  printf("%s Exiting\n", self);
  fflush(stdout);
#endif
  return NULL;
}

#if LOG_REQUESTER
int request_tasks_from(char self[], int target) {
#else
int request_tasks_from(int target) {
#endif
#if LOG_REQUESTER
  printf("%s Request to %02d\n", self, target);
  fflush(stdout);
#endif
  int request = GET_REQUEST;
  MPI_Send(&request, 1, MPI_INT, target, REQUEST_TAG, MPI_COMM_WORLD);

  int to_recv;
  MPI_Recv(&to_recv, 1, MPI_INT, target, ACKNOWL_TAG, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
#if LOG_REQUESTER
  printf("%s Acked by %02d ([↕] %d)\n", self, target, to_recv);
  fflush(stdout);
#endif

  if (to_recv == 0) {
    return 0;
  }

  int *recv_weights = (int *)malloc(to_recv * sizeof(int));
  MPI_Recv(recv_weights, to_recv, MPI_INT, target, RESPNSE_TAG, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
#if LOG_REQUESTER
  printf("%s Response from %02d ([↓] %d)\n", self, target, to_recv);
  fflush(stdout);
#endif

  pthread_mutex_lock(&task_mutex);
  task_t *more_tasks =
      (task_t *)realloc(tasks, (tasks_count + to_recv) * sizeof(task_t));
  for (int i = 0; i < to_recv; ++i) {
    more_tasks[tasks_count + i].weight = recv_weights[i];
    more_tasks[tasks_count + i].status = PENDING;
  }
  tasks = more_tasks;
  tasks_count += to_recv;
  pthread_mutex_unlock(&task_mutex);

  // Wake up workers
  pthread_cond_broadcast(&tasks_change_cond);

  free(recv_weights);
  return to_recv;
}

#if LOG_REQUESTER
int request_tasks_rr(char self[], int rank, int size, int *failures,
                     int *last_resp) {
#else
int request_tasks_rr(int rank, int size, int *failures, int *last_resp) {
#endif
  if (*last_resp < 0) {
    *last_resp = rank;
  }
  int start = modulo(*last_resp + 1, size);
  for (int i = 0; i < size; i++) {
    int target = modulo(start + i, size);
    if (target == rank) {
      continue;
    }
#if LOG_REQUESTER
    int received = request_tasks_from(self, target);
#else
    int received = request_tasks_from(target);
#endif
    if (received > 0) {
      *failures = 0;
      *last_resp = target;
      return received;
    } else {
      (*failures)++;
    }
  }

#if LOG_REQUESTER
  printf("%s No tasks from other processes\n", self);
  fflush(stdout);
#endif
  return 0;
}

#if LOG_REQUESTER
int request_tasks_sort(char self[], int rank, int size, int loads[size]) {
#else
int request_tasks_sort(int rank, int size, int loads[size]) {
#endif
  int targets[size];
  for (int i = 0; i < size; i++) {
    targets[i] = i;
  }

  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      if (loads[targets[j]] > loads[targets[i]]) {
        int tmp = targets[i];
        targets[i] = targets[j];
        targets[j] = tmp;
      }
    }
  }

  for (int i = 0; i < size; i++) {
    int target = targets[i];
    if (loads[target] == 0 || target == rank) {
      continue;
    }
#if LOG_REQUESTER
    int received = request_tasks_from(self, target);
#else
    int received = request_tasks_from(target);
#endif
    if (received > 0) {
      return received;
    }
  }

#if LOG_REQUESTER
  printf("%s No tasks from other processes\n", self);
  fflush(stdout);
#endif
  return 0;
}

// Requester-thread: Requests tasks from other processes
void *start_requester(void *arg) {
  thread_arg_t data = *((thread_arg_t *)arg);
  int size = data.size;
  int rank = data.rank;
  int tid = data.tid;

#if LOG_REQUESTER
  char self[FORMAT_SIZE];
  snprintf(self, sizeof(self), FULL_FORMAT, REQUESTER_NAME, rank, tid);
#endif

  pthread_mutex_lock(&iter_mutex);
  requesters_active++;
  pthread_mutex_unlock(&iter_mutex);

  while (!finished) {
    pthread_mutex_lock(&iter_mutex);
#if LOG_REQUESTER
    printf("%s Waiting for iteration start\n", self);
    fflush(stdout);
#endif
    requesters_active--;
    if (!finished && !iter_active) {
      pthread_cond_wait(&iter_start_cond, &iter_mutex);
    }
    requesters_active++;
#if LOG_REQUESTER
    printf("%s Iteration started\n", self);
    fflush(stdout);
#endif
    pthread_cond_broadcast(&tasks_change_cond);
    pthread_mutex_unlock(&iter_mutex);

    if (finished) {
      break;
    }

    int remaining = 0;
    int failures = 0;
    int do_request = 0;
    int last_resp = -1;

    while (1) {
      pthread_mutex_lock(&task_mutex);
      remaining = count_pending_tasks();
      do_request = remaining < REQT_THRESHOLD;
      if (!do_request) {
#if LOG_REQUESTER
        printf("%s Waiting for few tasks to remain\n", self);
        fflush(stdout);
#endif
        pthread_cond_wait(&few_tasks_cond, &task_mutex);
        pthread_mutex_unlock(&task_mutex);
        continue;
      }
      pthread_mutex_unlock(&task_mutex);

#if LOG_REQUESTER
      printf("%s %d tasks remaining\n", self, remaining);
      fflush(stdout);
#endif

      if (do_request) {
#if LOG_REQUESTER
        int received =
            request_tasks_rr(self, rank, size, &failures, &last_resp);
#else
        int received = request_tasks_rr(rank, size, &failures, &last_resp);
#endif
      }

      if (do_request && failures > size) {
        pthread_mutex_lock(&task_mutex);
        if (workers_active > 0) {
#if LOG_REQUESTER
          printf("%s Waiting for workers to finish\n", self);
          fflush(stdout);
#endif
          requesters_active--;
          pthread_cond_wait(&workers_done_cond, &task_mutex);
          requesters_active++;
        }
        pthread_mutex_unlock(&task_mutex);
        break;
      }
    }

    pthread_mutex_lock(&iter_mutex);
    iter_active = 0;
    pthread_cond_signal(&iter_end_cond);
    pthread_mutex_unlock(&iter_mutex);

#if LOG_REQUESTER
    printf("%s Iteration ended\n", self);
    fflush(stdout);
#endif
  }

  pthread_mutex_lock(&task_mutex);
  requesters_active--;
  pthread_mutex_unlock(&task_mutex);

  pthread_cond_broadcast(&tasks_change_cond);
  int end = END_REQUEST;
  MPI_Send(&end, 1, MPI_INT, rank, REQUEST_TAG, MPI_COMM_WORLD);

#if LOG_REQUESTER
  printf("%s Exiting\n", self);
  fflush(stdout);
#endif
  return NULL;
}

void print_info(int size) {
  char head[FORMAT_SIZE];
  char prefix[FORMAT_SIZE];
  snprintf(head, sizeof(head), STR_FORMAT, "balancer", "Pn", "Tn");
  snprintf(prefix, sizeof(prefix), STR_FORMAT, "balancer", "##", "##");
  printf("%s ========= LOAD BALANCER ========\n"
         "%s [ PROCESSORS     %13d ]\n"
         "%s [ TASKS_PER_PROC %13d ]\n"
         "%s [ TASKS_TOTAL    %13d ]\n",
         head, prefix, size, prefix, TASKS_PER_PROC, prefix,
         size * TASKS_PER_PROC);
  fflush(stdout);
}

void init_iteration(int iter, int rank, int size) {
  pthread_mutex_lock(&task_mutex);
  if (tasks != NULL) {
    free(tasks);
  }

  global_result = 0.;
  tasks_count = TASKS_PER_PROC;

  tasks = (task_t *)malloc(tasks_count * sizeof(task_t));
  for (int i = 0; i < tasks_count; i++) {
    tasks[i].weight = compute_weight(i, iter, rank, size);
    tasks[i].status = PENDING;
  }
  pthread_mutex_unlock(&task_mutex);
}

void gather_statistics(int rank, int size, double work_time,
                       statistics_t *stats_p, double *total_time,
                       double *total_result) {
  statistics_t stats;
  if (rank == 0) {
    stats.times = (double *)calloc(size, sizeof(double));
    stats.time_diffs = (double *)calloc(size, sizeof(double));
    stats.tasks_stat = (int *)calloc(3 * size, sizeof(int));
    stats.tasks_stat_total = (int *)calloc(size, sizeof(int));
  }

  // Gathering each process tasks statistics
  int local_stat[3] = {0};
  for (int i = 0; i < tasks_count; i++) {
    switch (tasks[i].status) {
    case (COMPLETED):
      local_stat[0]++;
      break;
    case (SENT):
      local_stat[1]++;
      break;
    case (PENDING):
      local_stat[2]++;
      break;
    }
  }
  MPI_Gather(local_stat, 3, MPI_INT, stats.tasks_stat, 3, MPI_INT, 0,
             MPI_COMM_WORLD);

  // Gathering each process working time
  MPI_Gather(&work_time, 1, MPI_DOUBLE, stats.times, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  // Gathering result
  double result;
  MPI_Reduce(&global_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // Iteration results
  if (rank != 0) {
    return;
  }

  double max_time = 0., min_time = stats.times[0];
  double mean_time = 0.;
  for (int i = 0; i < size; i++) {
    mean_time += stats.times[i];
    if (stats.times[i] > max_time) {
      max_time = stats.times[i];
    }
    if (stats.times[i] < min_time) {
      min_time = stats.times[i];
    }
  }
  mean_time /= size;

  stats.result = result;
  stats.max_time = max_time;
  stats.mean_time = mean_time;
  stats.imbalance = max_time - min_time;

  int total_done = 0;
  int total_sent = 0;
  int total_left = 0;
  double max_diff = 0.;
  for (int i = 0; i < size; i++) {
    total_done += stats.tasks_stat[i * 3];
    total_sent += stats.tasks_stat[i * 3 + 1];
    total_left += stats.tasks_stat[i * 3 + 2];

    double diff = stats.times[i] - mean_time;
    stats.time_diffs[i] = diff;

    if (diff > max_diff) {
      max_diff = diff;
    }
  }

  stats.max_diff = max_diff;
  stats.tasks_stat_total[0] = total_done;
  stats.tasks_stat_total[1] = total_sent;
  stats.tasks_stat_total[2] = total_left;

  *stats_p = stats;
  *total_time += max_time;
  *total_result += result;
}

void print_statistics(int iter, int size, int rank, statistics_t *stats) {
  if (rank != 0) {
    return;
  }

  char head[FORMAT_SIZE];
  char prefix[FORMAT_SIZE];
  snprintf(head, sizeof(prefix), STR_FORMAT, "iteration", "No", "Pn");
  snprintf(prefix, sizeof(prefix), THREAD_FORMAT, "iteration", iter);
  printf("%s ========= ITERATION %2d =========\n"
         "%s Result    = %.6f\n"
         "%s Iter.time = %.4f sec\n"
         "%s Imbalance = %.4f sec (%.2f%%)\n"
         "%s ================================\n"
         "%s Tasks          : Work   : Diffs\n"
         "%s done sent left :   time :    (%%)\n",
         head, iter, prefix, stats->result, prefix, stats->max_time, prefix,
         stats->imbalance, stats->imbalance / stats->max_time * 100., prefix,
         prefix, prefix);
  fflush(stdout);

  for (int i = 0; i < size; i++) {
    int tasks_done = stats->tasks_stat[i * 3];
    int tasks_sent = stats->tasks_stat[i * 3 + 1];
    int tasks_left = stats->tasks_stat[i * 3 + 2];
    double diff_perc = stats->time_diffs[i] / stats->mean_time * 100.;

    char prefix[FORMAT_SIZE];
    snprintf(prefix, sizeof(prefix), FULL_FORMAT, "iteration", iter, i);
    printf("%s %4d %4d %4d : %6.3f : %6.2f\n", prefix, tasks_done, tasks_sent,
           tasks_left, stats->times[i], diff_perc);
    fflush(stdout);
  }

  snprintf(prefix, sizeof(prefix), THREAD_FORMAT, "iteration", iter);
  printf("%s ---------------:--------:-------\n"
         "%s %4d %4d %4d : %6.3f :\n",
         prefix, prefix, stats->tasks_stat_total[0], stats->tasks_stat_total[1],
         stats->tasks_stat_total[2], stats->mean_time);

  free(stats->times);
  free(stats->time_diffs);
  free(stats->tasks_stat);
  free(stats->tasks_stat_total);
}

void print_results(int rank, double total_time, double total_result) {
  if (rank != 0) {
    return;
  }

  char prefix[FORMAT_SIZE];
  snprintf(prefix, sizeof(prefix), STR_FORMAT, "resulting", "##", "##");
  printf("%s ============= TOTAL ============\n"
         "%s Total time   = %.3f sec\n"
         "%s Total result = %.3f\n",
         prefix, prefix, total_time, prefix, total_result);
  fflush(stdout);
}

void start_balancer(int size, int rank, struct balancer *balancer) {
  balancer->rp_arg = (thread_arg_t){size, rank, 0};
  balancer->rq_arg = (thread_arg_t){size, rank, 0};
  pthread_create(&balancer->requester, NULL, start_requester,
                 &balancer->rq_arg);
  pthread_create(&balancer->responser, NULL, start_responser,
                 &balancer->rp_arg);
  for (int t = 0; t < WORKER_THREADS; t++) {
    balancer->w_args[t] = (thread_arg_t){size, rank, t};
    pthread_create(&balancer->workers[t], NULL, start_worker,
                   balancer->w_args + t);
  }
}

void finish_balancer(struct balancer *balancer) {
  pthread_mutex_lock(&iter_mutex);
  finished = 1;
  pthread_cond_broadcast(&iter_start_cond);
  pthread_mutex_unlock(&iter_mutex);

  pthread_join(balancer->requester, NULL);
  pthread_join(balancer->responser, NULL);
  pthread_cond_broadcast(&tasks_change_cond);
  for (int t = 0; t < WORKER_THREADS; t++) {
    pthread_join(balancer->workers[t], NULL);
  }
  pthread_mutex_destroy(&task_mutex);
  pthread_mutex_destroy(&iter_mutex);
  pthread_cond_destroy(&iter_start_cond);
  pthread_cond_destroy(&iter_end_cond);
  pthread_cond_destroy(&tasks_change_cond);
  pthread_cond_destroy(&workers_done_cond);

  if (tasks != NULL) {
    free(tasks);
  }
}

int main(int argc, char **argv) {
  int rank, size;
  int required = MPI_THREAD_MULTIPLE;
  int provided;

  MPI_Init_thread(&argc, &argv, required, &provided);
  if (provided < required) {
    fprintf(stderr, "Failed to MPI init with required thread level support\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0) {
    if (size < 2) {
      fprintf(stderr, "Expected at least 2 cpus\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    print_info(size);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  struct balancer balancer;
  statistics_t stats;

  start_balancer(size, rank, &balancer);

  double total_time = 0.;
  double total_result = 0.;

  for (int iter = 0; iter < ITER_MAX; iter++) {
    init_iteration(iter, rank, size);

    double start_time = MPI_Wtime();
    pthread_mutex_lock(&iter_mutex);
    iter_active = 1;
    pthread_cond_broadcast(&iter_start_cond);
    pthread_cond_wait(&iter_end_cond, &iter_mutex);
    double end_time = MPI_Wtime();
    pthread_mutex_unlock(&iter_mutex);
    double work_time = end_time - start_time;

    MPI_Barrier(MPI_COMM_WORLD);
    gather_statistics(rank, size, work_time, &stats, &total_time,
                      &total_result);
    print_statistics(iter, size, rank, &stats);
  }

  finish_balancer(&balancer);
  print_results(rank, total_time, total_result);

  MPI_Finalize();
  return 0;
}
