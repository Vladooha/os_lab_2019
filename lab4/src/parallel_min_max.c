#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _NC "[0m"

static void catchAlarm(int signo);
static void killParent(int signo);
pid_t lastPid;
pid_t pidsToKill[100];
unsigned int timeout = 0;

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }
  
  printf("Timeout is %d sec\n", timeout);

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n --f --timeout \"timeout\"",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  
  /*
  printf("Array:\n");
  for (int i = 0; i < array_size; ++i) {
      printf("%d\n", array[i]);
  }
  */

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int pipefd[2];
  pipe(pipefd);
  
  //int pids[999];

  int array_piece = array_size / pnum > 0 ? array_size / pnum : 1;
  if (timeout != 0) {
      printf("Timeout set on %d sec\n", timeout);
      signal(SIGALRM, catchAlarm);
      alarm(timeout);
}
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    pidsToKill[i] = child_pid;
    lastPid = child_pid;
    if (child_pid >= 0) {
      // successful fork
      
      active_child_processes += 1;
      if (child_pid == 0) {
          sleep(1);
        // child process
        
        //printf("PID: %d\n", pids[i]);
        
        // parallel somehow
        
        int begin = i * array_piece < array_size ? i * array_piece : array_size;
        int end = (i + 1) * array_piece < array_size ? (i + 1) * array_piece : array_size;
          
        struct MinMax buff;
        
        if (begin == array_size) {
            buff = GetMinMax(array, 0, 1);
        } else {
            buff = GetMinMax(array, begin, end);
        }
        /*
 printf(COLOR _BOLD _RED "\n\tPARENT: %d, this->PID: %d, CHILD: %d || min: %i, max: %i\n" COLOR _NC,\
                                                getppid(),\
                                                getpid(),\
                                                pids[i],\
                                                buff.min,\
                                                buff.max);
                                                */
        if (with_files) {
          // use files here
          
          //printf("B4 char*\n");
          
          char p[4];
          //itoa(i, p, 3);
          snprintf(p, sizeof(p), "%d", i);
          char p1[8] = "fork";
          strcat(p1, p);

          //printf("B4 file\n");          
          FILE *fp = fopen(p1, "wb");
          fwrite(&buff, sizeof(struct MinMax), 1, fp);
          int fcls = fclose(fp);
          //printf("Fclose: %d\n", fcls);
          
          printf("%s - child min: %d, child max: %d\n", p1, buff.min, buff.max);
        } else {
          // use pipe here
          
          write(pipefd[1], &buff, sizeof(struct MinMax));
        }
        
        //printf("max: %d, min: %d\n", buff.max, buff.min);
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  int counter = 0;
  
  if (timeout == 0) {
      printf("Waiting for childs\n");
      
      while (active_child_processes > 0) {
        // your code here
        if(lastPid){
        wait(NULL);
        signal(SIGALRM, catchAlarm);
          alarm(timeout);
          sleep(timeout+3);
        //kill(pids[counter], SIGKILL);
        
        active_child_processes -= 1;
        }
      }
  } else {
      waitpid(-1, NULL, WNOHANG);  
  }
  
  /*
  if (with_files) {
      char p[4];
      snprintf(p, sizeof(p), "%d", 0);
      char p1[8] = "fork";
      strcat(p1, p);
      
      while (!access(p1, F_OK)) {
          printf("%s - reading unavailable!\n", p1);
      }
      
      printf("Reading available!\n");
  }
  */

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  int min = INT_MAX;
  int max = INT_MIN;
  
  
  
  for (int i = 0; i < pnum; i++) {
    struct MinMax buff;
    

      
    if (with_files) {
      // read from files
      
      //printf("B4 str\n");
      
      char p[4];
      //itoa(i, p, 3);
      snprintf(p, sizeof(p), "%d", i);
      char p1[8] = "fork";
      strcat(p1, p);

      //printf("B4 file\n");
      
      
      
      FILE *fp = fopen(p1, "rb");
      if (fp == NULL) {
          printf("%s can't be open\n", p1);
      } else {
          printf("%s opened\n", p1);
      }
      fseek(fp, 0, SEEK_SET);
      size_t frd = fread(&buff, sizeof(struct MinMax), 1, fp);
      //printf("Fread: %lu\n", frd);
      fclose(fp);
      
      //printf("After file\n");
      printf(COLOR _BOLD _YELLOW  "Local min: %d, local max: %d" COLOR _NC "\n", buff.min, buff.max);
    } else {
      // read from pipes
      printf("Trying to read...\n");
      
      read(pipefd[0], &buff, sizeof(buff));
    }

    printf("Trying to parse...\n");

    min = buff.min;
    max = buff.max;

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}

static void catchAlarm(int signo) {
    for (int i = 0; i < 100; ++i) {
        if (pidsToKill[i] != 0) {
            printf("%d terminated\n", pidsToKill[i]);
        }
    }
    
    if (timeout != 0) {
        signal(SIGALRM, killParent);
        alarm(timeout);
    }
}

static void killParent(int signo) {
    printf("Can't read data of terminated children\n");
    kill(0, SIGKILL);
}