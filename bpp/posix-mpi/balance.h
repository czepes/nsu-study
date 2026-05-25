#ifndef BALANCE_HEADER
#define BALANCE_HEADER

#include <math.h>
#include <pthread.h>
#include <stdlib.h>

// Balance parameters
#define TASKS_PER_PROC 200
#define ITER_MAX 5
#define WORKER_THREADS 2
#define REQT_THRESHOLD 10
#define RESP_THRESHOLD 10
#define RESP_MULT 0.4
#define WEIGHT_MUL 10000

typedef struct {
  int size;
  int rank;
  int tid;
} thread_arg_t;

typedef enum { PENDING, COMPLETED, SENT } task_status_t;

typedef struct {
  int weight;
  task_status_t status;
} task_t;

typedef struct {
  double result;
  double max_time;
  double mean_time;
  double max_diff;
  double imbalance;
  int *tasks_stat;
  int *tasks_stat_total;
  double *times;
  double *time_diffs;
} statistics_t;

struct balancer {
  pthread_t requester;
  pthread_t responser;
  thread_arg_t rq_arg;
  thread_arg_t rp_arg;
  pthread_t workers[WORKER_THREADS];
  thread_arg_t w_args[WORKER_THREADS];
};

int compute_weight(int i, int iter, int rank, int size);
void complete_task(int weight);
int count_pending_tasks();

// Logging
#define DUMMY_FORMAT "[############:##]"
#define THREAD_FORMAT "[%-9s %02d:##]"
#define PROC_FORMAT "[############:%02d]"
#define FULL_FORMAT "[%-9s %02d:%02d]"
#define STR_FORMAT "[%-9s %2s:%2s]"
#define FORMAT_SIZE 32
#define WORKER_NAME "worker"
#define RESPONSER_NAME "responser"
#define REQUESTER_NAME "requester"
#define MASTER_NAME "master"

#endif
