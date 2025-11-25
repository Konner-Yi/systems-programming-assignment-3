// Server-side 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT "8080"
#define BUFFER_SIZE 1024
void* multi_client_handling(void *arg){
  int client_fd = *(int *)arg;
  free(arg);
  char buffer[BUFFER_SIZE];
  int bytes_read;
  // file read
  FILE *fp = fopen("Benefits_Canada.txt", "r");
  if(fp==NULL){
    perror("Error opening file");
    close(client_fd);
    exit(1);
    }
  // send (initial  welcome message)
  const char *msg = "Welcome to Benefits Canada Server!\n";
  send(client_fd, msg, strlen(msg), 0);
  
  
  // send prompt
  char name_prompt[BUFFER_SIZE] = "Please enter your name:";
  send(client_fd, name_prompt, strlen(name_prompt), 0);
  
  // recieves the name of person
  memset(buffer, 0, BUFFER_SIZE);
  bytes_read=recv(client_fd, buffer, BUFFER_SIZE-1, 0);
  if(bytes_read<=0){
    close(client_fd);
    pthread_exit(NULL);
  }
  buffer[bytes_read] = '\0';
  buffer[strcspn(buffer, "\r\n")] =0; // deals with fgets on client side.
  // send greeting to person and prompts about benefits:
  char greeting[BUFFER_SIZE];
  char line[BUFFER_SIZE];
  char info[BUFFER_SIZE];
  snprintf(greeting, BUFFER_SIZE, "hello %s\n To get information about benefits, please select from the following:", buffer);
  
  send(client_fd, greeting, strlen(greeting), 0);
  // grabs serial and name info and sends
  while(fgets(line, sizeof(line), fp)){
    char* serial = strtok(line, ";");
    strtok(NULL, ";");
    char *name = strtok(NULL, ";");
    if(serial && name){ // Ignores null ptr
      snprintf(info, BUFFER_SIZE, "%s: %s\n", serial, name);
      send(client_fd, info, strlen(info), 0);
    }
  }
  
  
  while(1){
    // recieve
    bytes_read = recv(client_fd, buffer, BUFFER_SIZE-1, 0);
    if(bytes_read<=0) break;
      buffer[bytes_read] = '\0'; // null-termate buffer
     
      if(strcmp(buffer, "hello\n")==0){
        const char *response = "Hi";
        send(client_fd, response, strlen(response), 0);
      }
      
  }
  
  pthread_exit(NULL);
  
  
  
}
int main()
{
  struct addrinfo hints, *res;
  int server_fd;
  char buffer[BUFFER_SIZE];
  int bytes_read;
  
  // getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags= AI_PASSIVE;
    if(getaddrinfo(NULL, PORT, &hints, &res) !=0){
      perror("getaddrinfo");
      return 1;
    
    }
  //Socket 
  server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if(server_fd < 0)
  {
   perror("socket");
   return 1; //exit(EXIT_FALIURE);
  }
  //reusing address
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  
  // binding
  if(bind(server_fd, res->ai_addr, res->ai_addrlen)<0){
    perror("bind");
    close(server_fd);
    return 1;
  }
  
  freeaddrinfo(res);
  
  // listen
  if(listen(server_fd, 10) <0) {
    perror("listen");
    close(server_fd);
    return 1;
  }
  printf("Listening to incoming connection on port %s...", PORT);
  
  // accept req
  while(1){
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int)); // for mutliple clients
        if(!client_fd) continue;
        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
        if(*client_fd<0){
          perror("accept");
          free(client_fd); 
          continue;
        }
  
  printf("Client connected.\n");
  // Thread create
  pthread_t thrid;
  if(pthread_create(&thrid, NULL, multi_client_handling, client_fd)!=0) {
  perror("pthread create");;
  close(*client_fd);
  free(client_fd);
  } else {
    pthread_detach(thrid);
  }
  }
  
  
  //close 
  
  close(server_fd);
  
  return 0;
  
}
