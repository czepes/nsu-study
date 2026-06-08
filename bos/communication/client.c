#include "sock.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int client_running = 0;

void server_sighandler(int signum) {
  switch (signum) {
  case SIGPIPE: {
    const char *msg = "[client] Server disconnected\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    client_running = 0;
    break;
  }
  }
}

int init_signals() {
  struct sigaction act = {
      .sa_handler = server_sighandler,
      .sa_flags = 0,
  };
  sigemptyset(&act.sa_mask);

  // signal(SIGPIPE, SIG_IGN);
  if (sigaction(SIGPIPE, &act, NULL) == -1) {
    return -1;
  }

  return 0;
}

int init_client() {
  if (init_signals() == -1) {
    perror("[client] Failed to init signals");
    return -1;
  }

  int connfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (connfd == -1) {
    perror("[client] Failed to create socket");
    return 1;
  }

  struct sockaddr_un host_addr = {.sun_family = AF_UNIX, .sun_path = SOCK_ADDR};
  socklen_t host_addr_len = sizeof(host_addr);

  if (connect(connfd, (struct sockaddr *)&host_addr, host_addr_len) == -1) {
    perror("[client] Failed to connect to server");
    return 1;
  }
  printf("[client] Connected to server\n");

  return connfd;
}

int run_writer(int connfd) {
  const size_t size = 256;
  char buffer[size];
  char response[size];

  client_running = 1;
  printf("[writer] Starting session\n");
  while (client_running) {
    printf("> ");
    fflush(stdout);

    if (fgets(buffer, size, stdin) == NULL) {
      printf("[writer] Reached EOF\n");
      break;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    if (strcmp(buffer, "exit") == 0) {
      break;
    }

    if (write(connfd, buffer, len) != len) {
      perror("[writer] Failed to send message");
      return 1;
    }

    printf("[writer] Sent: \"%s\"\n", buffer);

    ssize_t n = read(connfd, response, size);
    if (n <= 0) {
      if (n == 0) {
        printf("[writer] Server disconnected\n");
      } else {
        perror("[writer] Failed to receive echo\n");
        return 1;
      }
      break;
    }

    response[n] = '\0';
    printf("[writer] Echo: \"%s\"\n", response);
  }
  client_running = 0;

  printf("[client] Shutting down...\n");
  return 0;
}

void finalize_client(int connfd) { close(connfd); }

int main(void) {
  int connfd = init_client();
  if (connfd == -1) {
    return 1;
  }
  int r = run_writer(connfd);
  finalize_client(connfd);
  return r;
}
