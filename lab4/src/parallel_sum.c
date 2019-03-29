#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include <pthread.h>

#include "utils.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

long Sum(const struct SumArgs *args) {
  long sum = 0;
  for (int i = args->begin; i < args->end; ++i) {
      sum += args->array[i];
  }

  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */
  
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            printf("Seed: %d\n", seed);
            // your code here
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            printf("Size: %d\n", array_size);
            // your code here
            // error handling
            break;
          case 2:
            threads_num = atoi(optarg);
            printf("Threads: %d\n", threads_num);
            // your code here
            // error handling
            break;
          

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  //if (optind < argc) {
    //printf("Has at least one no option argument\n");
    //return 1;
  //}

  if (seed == 0 || array_size == 0 || threads_num == 0) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n",
           argv[0]);
    return 1;
  }

  pthread_t threads[threads_num];
  
  /*
   * TODO:
   * your code here
   * Generate array here
   */
  
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  
  if (array_size < 9) {
      printf("Array:\n");
      for (int i = 0; i < array_size; ++i) {
          printf("%d\n", array[i]);
      }
  }
  
  struct SumArgs args[threads_num];
  for (int i = 0, j = 0; i < array_size && j < threads_num; i += array_size/threads_num, ++j) {
    args[j].array = array;
    args[j].begin = i;
    args[j].end = (i + array_size/threads_num) > array_size ? array_size : (i + array_size/threads_num);
    if (j == threads_num - 1) {
        args[j].end = array_size;
    }
    
    printf("Struct #%d: begin - %d, end - %d\n", j, args[j].begin, args[j].end);
  }

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&(args[i]))) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  long total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    long sum = 0;
    pthread_join(threads[i], (void **)&sum);
    printf("Sum #%d: %ld\n", i, sum);
    total_sum += sum;
    printf("Total #%d: %ld\n", i, total_sum);
  }
  
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Total: %ld\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
  return 0;
}
