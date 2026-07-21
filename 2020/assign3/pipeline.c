/**
 * File: pipeline.c
 * ----------------
 * Presents the implementation of the pipeline routine.
 */

#include "pipeline.h"
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

void pipeline(char *argv1[], char *argv2[], pid_t pids[]) {
  int fds[2];
  // fds[0] is read end, fds[1] is write end
  if (pipe(fds) == -1) {
    perror("pipe");
    return;
  }

  pids[0] = fork();
  if (pids[0] == -1) {
    perror("fork");
    close(fds[0]);
    close(fds[1]);
    return;
  }
  if (pids[0] == 0) {
    // child processor
    if (dup2(fds[1], STDOUT_FILENO) == -1) {
      perror("dup2");
      _exit(1);
    }
    close(fds[0]);
    close(fds[1]);
    execvp(argv1[0], argv1); // auto search PATH
    perror(argv1[0]);
    _exit(127);
  }
  // 如果管道的写端没有被完全关闭，会导致第二个进程一直等 EOF（文件结束符）
  close(fds[1]);

  pids[1] = fork();
  if (pids[1] == -1) {
    perror("fork");
    kill(pids[0], SIGTERM);
    close(fds[0]);
    close(fds[1]);
    waitpid(pids[0], NULL, 0);
    return;
  }
  if (pids[1] == 0) {
    // child processor
    if (dup2(fds[0], STDIN_FILENO) == -1) {
      perror("dup2");
      _exit(1);
    }
    close(fds[0]);
    close(fds[1]);
    execvp(argv2[0], argv2); // auto search PATH
    perror(argv2[0]);
    _exit(127);
  }
  close(fds[0]);
}
