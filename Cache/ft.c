#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
void handler(int sig) {
int stat;
wait(&stat);
if (WIFEXITED(stat))
printf("%d", WEXITSTATUS(stat));
}
int main() {
int i;
pid_t pids[5];
signal(SIGCHLD, handler);
for (i=0; i<5; i++)
if ((pids[i] = fork()) == 0) {
printf("1");
while (1) ; /* inf loop */
}
for (i=0; i<5; i++)
kill(pids[i], SIGKILL);
sleep(1);
return 2;
}