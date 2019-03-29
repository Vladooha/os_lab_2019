#include <stdio.h>
#include <sys/sysinfo.h>
#include<signal.h>

#define THREADS 5
#define BIGARRSIZE 999999999

void showMem();

int main(void)
{
    int pids[THREADS];
    
    for (int i = 0; i < THREADS; ++i) {
        pids[i] = fork();
        
        switch(pids[i]) {
            case -1:
                // Error
                perror("fork");
                
                return -1;
            case 0:
                // Child
                showMem("Child before");
                for (int i = 0; i < 999999999; ++i) {
                    int* bigarr = (int*) malloc(BIGARRSIZE * sizeof(int));
                }
                showMem("Child after");
                
                return 42;
            default: {
                // Parent
                //printf("Parent #%d\n", i);
                break;
            }
        }
    }
    
    sleep(1);
    
    int* memRefresher;
    showMem("Parent before kill()");
    
    //sleep(1);
    
    memRefresher = (int*) malloc(sizeof(int));
    for (int i = 0; i < THREADS; ++i) {
        kill(pids[i], SIGKILL);
    }
    
    printf("After kill()\n");
    showMem("Parent");
    
    //sleep(1);
    
    memRefresher = (int*) malloc(sizeof(int));
    for (int i = 0; i < THREADS; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
    }
    
    printf("After wait()\n");
    showMem("Parent");

    return 0;
}

void showMem(char* owner) {
    unsigned long int FreeMem = 0;
    struct sysinfo info;
    sysinfo(&info);
    FreeMem = info.freeram/(1024*1024);
    printf("[%s] Free memory: %d Mb\n", owner, FreeMem);
}