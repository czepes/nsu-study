#include "sock.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int server_running = 0;

void server_sighandler(int signum) {
  const char *msg = "[server] Shutting down...\n";
  write(STDOUT_FILENO, msg, strlen(msg));

  switch (signum) {
  case SIGINT: // hard termination => killing readers
  {
    signal(SIGINT, SIG_IGN);
    kill(-getpgrp(), SIGINT);
    break;
  }
  case SIGUSR1: // soft termination => waiting readers
  {
    const char *msg = "[server] Waiting for readers to finish\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    break;
  }
  }

  server_running = 0;
}

void reader_sighandler(int signum) {
  switch (signum) {
  case SIGINT: {
    signal(SIGINT, SIG_IGN);
    const char *msg = "[reader] Shutting down...\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    server_running = 0;
    break;
  }
  }
}

int init_signals(int is_server) {
  struct sigaction act = {
      .sa_handler = is_server ? server_sighandler : reader_sighandler,
  };
  sigemptyset(&act.sa_mask);

  if (sigaction(SIGINT, &act, NULL) == -1) {
    return -1;
  }

  if (is_server) {
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
      return -1;
    }
  }

  return 0;
}

int init_server() {
  int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("[server] Failed to create socket");
    return -1;
  }

  unlink(SOCK_ADDR);
  unlink(TERM_ADDR);

  int termfd = open(TERM_ADDR, O_CREAT | O_TRUNC | O_WRONLY, 0600);
  if (termfd == -1) {
    perror("[server] Failed to create term-file");
    close(sockfd);
    return -1;
  }

  pid_t server_pid = getpid();
  if (write(termfd, &server_pid, sizeof(pid_t)) == -1) {
    perror("[server] Failed to write PID to term-file");
    close(sockfd);
    close(termfd);
    return -1;
  }
  close(termfd);

  struct sockaddr_un host_addr = {
      .sun_family = AF_UNIX,
      .sun_path = SOCK_ADDR,
  };

  if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) == -1) {
    perror("[server] Failed to bind socket to file");
    close(sockfd);
    return -1;
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("[server] Failed to init listening socket");
    close(sockfd);
    return -1;
  }

  if (init_signals(1) == -1) {
    perror("[server] Failed to init signals");
    return -1;
  }

  printf("[server] Listening on %s\n", SOCK_ADDR);
  return sockfd;
}

int run_reader(int cliend_id, int connfd) {
  char buffer[256];
  size_t len = 256;

  while (server_running) {
    ssize_t n = read(connfd, buffer, len);
    if (n <= 0) {
      if (n == 0) {
        printf("[reader] Client %d disconnected\n", cliend_id);
      } else if (errno != EINTR) {
        fprintf(stderr, "[reader] Failed to receive message from client %d\n",
                cliend_id);
      }
      break;
    }

    buffer[n] = '\0';
    printf("[reader] Received: \"%s\" from client %d\n", buffer, cliend_id);
    fflush(stdout);

    if (write(connfd, buffer, n) != n) {
      fprintf(stderr, "[reader] Failed to echo client %d\n", cliend_id);
      return 1;
    }
  }

  return 0;
}

pid_t stop_reader(int options) {
  int status;
  pid_t pid = waitpid(-1, &status, options);
  if (pid > 0) {
    if (WIFEXITED(status)) {
      printf("[server] Reader [%d] exited with code %d\n", pid,
             WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
      printf("[server] Reader [%d] terminated with signal %d\n", pid,
             WTERMSIG(status));
    }
  }
  return pid;
}

int run_server(int sockfd) {
  struct sockaddr_un peer_addr;
  socklen_t peer_addr_len = sizeof(peer_addr);
  int connfd;
  int status;
  pid_t pid;
  int client_id = 0;

  server_running = 1;
  while (server_running) {
    do {
      pid = stop_reader(WNOHANG);
    } while (pid > 0);

    connfd = accept(sockfd, (struct sockaddr *)&peer_addr, &peer_addr_len);
    if (connfd == -1) {
      if (errno == EINTR) {
        continue;
      }
      perror("[server] Failed to establish connection");
      break;
    }
    printf("[server] New client connected\n");

    pid = fork();
    switch (pid) {
    case -1:
      perror("[server] Failed to create reader process");
      close(connfd);
      close(sockfd);
      return -1;
    case 0: {
      close(sockfd);
      init_signals(0);
      int r = run_reader(client_id, connfd);
      close(connfd);
      exit(r);
    }
    default: {
      close(connfd);
      printf("[server] Launched reader [%d]\n", pid);
      client_id++;
      continue;
    }
    }
  }
  server_running = 0;
  return 0;
}

void finalize_server(int sockfd) {
  int status;
  pid_t pid;
  do {
    pid = stop_reader(0);
  } while (pid > 0);

  close(sockfd);
  unlink(SOCK_ADDR);
  unlink(TERM_ADDR);
}

int main(void) {
  int sockfd = init_server();
  if (sockfd == -1) {
    return 1;
  }
  int r = run_server(sockfd);
  finalize_server(sockfd);
  return r;
}
