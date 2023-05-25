#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/*

* This example shows a "signal action function"

* Send the child various signals and observe operation.

*

*/

void ChildHandler (int sig, siginfo_t *sip, void *notused)

{

 int status;


   printf ("The process generating the signal is PID: %d\n",

            sip->si_pid);

   fflush (stdout);


   status = 0;

   /* The WNOHANG flag means that if there's no news, we don't wait*/

   if (sip->si_pid == waitpid (sip->si_pid, &status, WNOHANG))

   {

       /* A SIGCHLD doesn't necessarily mean death - a quick check */

       if (WIFEXITED(status)|| WTERMSIG(status))

         printf ("The child is gone\n"); /* dead */

       else

         printf ("Uninteresting\n"); /* alive */

   }

   else

   {

     /* If there's no news, we're probably not interested, either */

     printf ("Uninteresting\n");

   }

}


int main()

{


 struct sigaction action;


 action.sa_sigaction = ChildHandler; /* Note use of sigaction, not    

                                        handler */

 sigfillset (&action.sa_mask);

 action.sa_flags = SA_SIGINFO; /* Note flag,otherwise NULL in function*/

 sigaction (SIGCHLD, &action, NULL);


 fork();


 while (1)

 {

   printf ("PID: %d\n", getpid());

   sleep(1);

 }

}
