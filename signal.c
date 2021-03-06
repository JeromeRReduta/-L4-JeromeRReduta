/**
 * signal.c
 *
 * Demonstrates signal handling. During normal operation this program counts up
 * each second, starting from 1. Upon receiving SIGINT, the final count will be
 * printed and the program will exit. Many programs use this type of signal
 * handling to clean up and gracefully exit when the user presses Ctrl+C.
 *
 * Compile: gcc -g -Wall signal.c -o signal
 * Run: ./signal
 */

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned long count = 0;
int timer = 15;

/* Function prototypes: */
void init_signals();
void sigint_handler(int signo);
void finish_countdown();
void reset_count();
void increase_count_by(int num);

// Initializes the signals
void init_signals()
{
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, sigint_handler);
    signal(SIGQUIT, sigint_handler);
    // Got these 2 lines of code from https://linuxhint.com/sigalarm_alarm_c_language/
    signal(SIGUSR1, sigint_handler);
}

// Handles inputted signals
void sigint_handler(int signo)
{
    printf("\n");

    switch(signo) {
        // End program
        case SIGINT:
            finish_countdown();
            break;

        // Reset count
        case SIGALRM:
            reset_count();
            break;

        // Increment count by 5
        case SIGQUIT:
            increase_count_by(5);
            break;

        // Increment count by 10
        case SIGUSR1:
            increase_count_by(10);
            break;
    }
}

// Completes countdown and exits program
void finish_countdown()
{
    printf("Final count is: %ld\n", count);
    printf("Goodbye!\n");
    exit(0);
}

// Resets count
void reset_count()
{
    printf("Resetting count!\n");
    count = 0;
    alarm(timer);
}

// Increases count by num
void increase_count_by(int num) {
    count += num;
    printf("Count increased by %d: %ld\n", num, count);
}

int main(void) {

    printf("Starting up. My PID = %d\n", getpid());

    init_signals();
    alarm(timer);

    /* NOTE: You should NOT modify below this line!!!! ----------------------*/
    /* Instead, use signal handlers to accomplish what you have to do.       */
    while (true) {
        count++;
        printf("Count: %ld\n", count);
        sleep(1);
    }
    /* ----------------------------------------------------------------------*/

    return 0;

}
