#include <stdio.h>
#include <signal.h>
#include <unistdio.h>
#include <stdlib.h>

int counter = 300;

void sigint_signterm_handler(int counter) {
    counter -= 100;
    printf("-100s \n");
}

int main() {
    if (signal(SIGINT, sigint_signterm_handler) == SIG_ERR) {
        exit(1);
    }



    // signal(SIGTERM, sigint_signterm_handler);
    while (counter > 0) {
        counter -= 1;
        printf("%d\n", counter);
    }
    printf("BOOM!!!\n");
    return 0;
}