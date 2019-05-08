#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

struct Server {
  char ip[255];
  int port;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }

  return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}


char** ReadFileStrings(char* filename){
  FILE* file = fopen(filename, "r");

  if (file == NULL){
    printf("\nError occured while reading\n");
    return NULL;
  }


  fseek(file, 0, SEEK_END);
  int FileSize = ftell(file);    //узнаем размер файла
  fseek(file, 0, SEEK_SET);

  char* txt = (char*)malloc(sizeof(char)*FileSize + 1);
  FileSize = fread(txt, sizeof(char), FileSize, file);    //Присваиваем FileSize количество реально прочитанных байт
  txt[FileSize] = '\0';

  void* search_ptr = txt;
  char** strings = (char**)calloc(1000, sizeof(char*));          //выделим побольше памяти, потом ее урежем realloc'ом
  int cnt = 0;
  do{                                    //считаем количество строк
    int length = 0;
    if (memchr(search_ptr, '\n', FileSize*sizeof(char)) != NULL)    //длина строки
      length = (intptr_t)memchr(search_ptr, '\n', FileSize*sizeof(char)) - (intptr_t)search_ptr + 1;
    else
      length = strlen((char*)search_ptr) + 1;

    strings[cnt] = (char*)calloc(length, sizeof(char));
    memcpy(strings[cnt], search_ptr, length - 1);            //копируем -1 символ, чтобы сохранить \0 в конце строки
    cnt++;
  } while ((search_ptr = (void*)((intptr_t)memchr(search_ptr, '\n', FileSize*sizeof(char)) + (intptr_t)sizeof(char))) != (void*)1);

  strings = (char**)realloc(strings, (cnt + 1)*sizeof(char*));
  fclose(file);
  
  return strings;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255
  char** serverPortList;
  int serverCount = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        if (k <= 0) {
            printf("k should be positive\n");
            
            return 1;
        }
        
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        if (mod <= 0) {
            printf("mod should be positive\n");
            
            return 2;
        }
        
        break;
      case 2:
        memcpy(servers, optarg, strlen(optarg));
        
        serverPortList = ReadFromFile(servers);
        char** serverPortListTmp = serverPortList;
        for(int i = 1; *serverPortListTmp != NULL; ++i) {
            printf("Server #%d: %s\n", i, *serverPortListTmp);
            serverPortListTmp++;
        }
        serverCount = serverPortListTmp - serverPortList;
        
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  // TODO: for one server here, rewrite with servers from file
  
  // Legacy
  //unsigned int servers_num = 1;
  //struct Server *to = malloc(sizeof(struct Server) * servers_num);
  // End of Legacy
  
  struct Server *to = malloc(sizeof(struct Server) * serversCount);
  // TODO: delete this and parallel work between servers
  // Legacy
  //to[0].port = 20001;
  //memcpy(to[0].ip, "127.0.0.1", sizeof("127.0.0.1"));
  // End of Legacy
  
  for(int i = 0; i < servers_num; i++){
      char* adressParts = strtok(server_port_list[i],":");
      strcpy(to[i].ip, adressParts);
      adressParts = strtok(NULL,":");
      to[i].port = atoi(addr_part);
  }

  // TODO: work continiously, rewrite to make parallel
  
  uint64_t numsPerServer;

  numsPerServer = k/serversCount;
  if(k % serversCount != 0)
    numsPerServer++;
          
  pthread_t threads[servers_num];
  Mail mail2server[servers_num];
  uint64_t partial_res[servers_num];
  
  // Legacy
  /*
  for (int i = 0; i < servers_num; i++) {
    struct hostent *hostname = gethostbyname(to[i].ip);
    if (hostname == NULL) {
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(to[i].port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }

    // TODO: for one server
    // parallel between servers
    uint64_t begin = 1;
    uint64_t end = k;

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      exit(1);
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
      fprintf(stderr, "Recieve failed\n");
      exit(1);
    }

    // TODO: from one server
    // unite results
    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));
    printf("answer: %llu\n", answer);

    close(sck);
  }
  free(to);
  */
  // End of Legacy
 
  bool isKReached = false;
  uint64_t usedServersCount = serverCount;
  for (int i = 0; i < serversCount && !isKReached; i++) {
    struct hostent *hostname = gethostbyname(to[i].ip);
    if (hostname == NULL) {
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);  
    }

    mail2server[i].server.sin_family = AF_INET;
    mail2server[i].server.sin_port = htons(to[i].port);
    mail2server[i].server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);
    
    /*
    if(servers_num != 1 && j == servers_num && servers_num*part <= k){
        mail2server[i].begin = i*part + 1;
        mail2server[i].end = k;
    }
    else
        if(i == 0){
            
            mail2server[i].end = part;
        }
        else{
            mail2server[i].begin = i*part + 1;
            mail2server[i].end = j*part;
        }
    */
    int begin = i*numsPerServer + 1;
    int end = begin + numsPerServer;
    if (begin <= k) {
        mail2server[i].begin = begin;
        if (end < k) {
            mail2server[i].end = end;
        } else {
            mail2server[i].end = k;
            isKReached = true;
            usedServersCount = i;
        }
    }
      
        
   
    mail2server[i].mod = mod;
    printf("Server #%d: begin - %llu, end - %llu, mod - %llu\n",\
                                    i,\
                                    mail2server[i].begin,\
                                    mail2server[i].end,\
                                    mail2server[i].mod);
    
    
    pthread_create(&threads[i], NULL, ConnectAndSR, (void *)&mail2server[i]);
  }
  
  for (int i = 0; i < usedServerCount; ++i) {
    pthread_join(threads[i], (void **)&partial_res[i]);
    printf("Server#%d result: %llu\n", i, partial_res[i]);
  }
  
  free(to);
  
  uint64_t result = 1;
  for(int i = 0; i < usedServerCount; i++) {
    result *= partial_res[i]; 
    result %= mod;      
  }

  printf("\nFINAL RESULT: %llu\n", result);  

  return 0;
}
