#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <linux/kcmp.h>

#ifndef SYS_pidfd_getfd
#define SYS_pidfd_getfd 438
#endif

void testPtrace(int pid)
{
  printf("Ptrace start\n");

  printf("PTRACE_SEIZE\n");
  if (ptrace(PTRACE_SEIZE, pid, 0, 0) < 0)
  {
    perror("ptrace PTRACE_SEIZE");
    return;
  }

  printf("PTRACE_INTERRUPT\n");
  if (ptrace(PTRACE_INTERRUPT, pid, 0, 0) < 0)
  {
    perror("ptrace PTRACE_INTERRUPT");
    return;
  }

  printf("waitpid\n");
  if (waitpid(pid, NULL, 0) < 0)
  {
    perror("waitpid");
    return;
  }

  printf("PTRACE_DETACH\n");
  if (ptrace(PTRACE_DETACH, pid, 0, 0) < 0)
  {
    perror("ptrace PTRACE_DETACH");
    return;
  }

  printf("Ptrace success\n");
}

void testPidfdGetfd(int pid)
{
  printf("pidfd_getfd start\n");

  printf("pidfd_open\n");
  int pidfd = syscall(SYS_pidfd_open, pid, 0);
  if (pidfd < 0)
  {
    perror("pidfd_open");
    return;
  }

  printf("pidfd_getfd\n");
  int fd = syscall(SYS_pidfd_getfd, pidfd, 2, 0);
  if (fd < 0)
  {
    perror("pidfd_getfd");
    close(pidfd);
    return;
  }

  printf("kcmp\n");
  // Child inherited stdout so it should be the same.
  int cmp = syscall(SYS_kcmp, getpid(), getpid(), KCMP_FILE, 2, fd);
  if (cmp < 0)
  {
    perror("kcmp");
  }
  else if (cmp != 0)
  {
    printf("error: file descriptors should be equal");
  }

  close(fd);
  close(pidfd);

  printf("pidfd_getfd success\n");
}

void testKcmp(int pid)
{
  printf("kcmp start\n");

  printf("kcmp\n");
  // Child inherited stdout so it should be the same.
  int cmp = syscall(SYS_kcmp, getpid(), pid, KCMP_FILE, 2, 2);
  if (cmp < 0)
  {
    perror("kcmp");
  }
  else if (cmp != 0)
  {
    printf("error: file descriptors should be equal");
  }

  printf("kcmp success\n");
}

void main()
{
  printf("Start\n");
  printf("Fork\n");
  int pid = fork();
  if (pid == 0)
  {
    printf("Pause in the child\n");
    pause();
    exit(0);
  }
  else if (pid < 0)
  {
    perror("fork");
    exit(1);
  }
  printf("Child's PID is %d\n", pid);
  printf("Sleep\n");
  sleep(1);
  testPtrace(pid);
  testPidfdGetfd(pid);
  testKcmp(pid);
  printf("Kill child\n");
  if (kill(pid, 9) < 0)
  {
    perror("kill");
  }
  printf("Wait for child\n");
  if (waitpid(pid, NULL, 0) < 0)
  {
    perror("waitpid");
  }
  printf("End\n");
}