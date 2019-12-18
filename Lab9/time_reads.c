/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long amounts, seconds;


/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */
void handler(int sig) {
    printf(MESSAGE, amounts, seconds);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);
    struct itimerval t;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_value.tv_sec = (long int) seconds;
    setitimer (ITIMER_PROF, &t, NULL);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    struct sigaction newact;
    newact.sa_handler = handler;
    newact.sa_flags = 0; //default flag
    sigemptyset(&newact.sa_mask); //No signals are blocked during handler.
    sigaction(SIGPROF, &newact, NULL);
    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    int r;
    int number;
    for (;;) {
        r = random() % 100;
        fseek(fp, r * sizeof(int), SEEK_SET);
        fread(&number, sizeof(int), 1 , fp);
        printf("%d\n", number);
        amounts++;
    }
    return 1; // something is wrong if we ever get here!
}
