//
// Created by maxim on 1/5/20.
//

#ifdef __GNUC__
#pragma implementation "sig_handlers.h"
#endif

#include "sig_handlers.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>
#include <unistd.h>
#include <wait.h>
#include <sys/prctl.h>

void print_backtrace()
{
   char pid_buf[30];
   sprintf(pid_buf, "%d", getpid());
   char name_buf[512];
   name_buf[readlink("/proc/self/exe", name_buf, 511)] = 0;
   prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
   int child_pid = fork();
   if (!child_pid)
   {
      dup2(2,1);
      fprintf(stdout,"stack trace for %s pid=%s\n", name_buf, pid_buf);
      execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
      abort();
   }
   else
   {
      waitpid(child_pid,NULL,0);
   }
}

void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
   fflush(stderr);
   fflush(stdout);

   const char* code_string = si->si_code == SEGV_MAPERR ? "SEGV_MAPERR" : "SEGV_ACCERR";

   fprintf(stderr, "\n========= SIGFAULT =========\nSignal code: %i (%s)\nSignal errno: %i\nSignal address: %p\n" \
         "Lower bound: %p\nUpper bound: %p\n\n",
           si->si_code, code_string, si->si_errno, si->si_addr, si->si_lower, si->si_upper);

   print_backtrace();

   fprintf(stderr, "=============================\n");
   fflush(stderr);

   exit(1);
}

void abort_sigaction(int signal, siginfo_t *si, void *arg)
{
   fflush(stderr);
   fflush(stdout);

   fprintf(stderr, "\n========= ABORT =========\nSignal errno: %i\nSignal address: %p\n\n",
           si->si_errno, si->si_addr);

   print_backtrace();

   fprintf(stderr, "==========================\n");
   fflush(stderr);

   exit(1);
}


void register_sig_handlers(void)
{
   struct sigaction seg;
   struct sigaction abr;

   memset(&seg, 0, sizeof(struct sigaction));
   sigemptyset(&seg.sa_mask);
   seg.sa_sigaction = segfault_sigaction;
   seg.sa_flags = SA_SIGINFO;
   sigaction(SIGSEGV, &seg, NULL);

   memset(&abr, 0, sizeof(struct sigaction));
   sigemptyset(&abr.sa_mask);
   abr.sa_sigaction = abort_sigaction;
   abr.sa_flags = SA_SIGINFO;
   sigaction(SIGABRT, &abr, NULL);
}


