// Client-side TCP
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#define SEVER_ADDR "127.0.0.1"
#define PORT "8080"
#define BUFFER_SIZE 1024

int main()
{

  struct addrinfo hints, *res; // res: results of getting address info 
  int sockfd;
  char buffer[BUFFER_SIZE];
  int bytes;
  // get address info 
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    if(getaddrinfo(SEVER_ADDR, PORT, &hints, &res) !=0){
      perror("getaddrinfo");
      exit(1);
    
    }
  //Socket 
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if(sockfd < 0)
  {
   perror("socket");
   exit(1); 
  }
  //connect
  if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
    perror("connect");
    exit(1);
  }
  freeaddrinfo(res);
  // send
  
  
  
  // recieve (while loop included for server to send until the prompt "Please enter your name:" is sent
  while(1){
  bytes = recv(sockfd, buffer, BUFFER_SIZE-1, 0);
  if(bytes<0){
    perror("recv");
    exit(1);
  }
  buffer[bytes] = '\0';
  printf("%s\n", buffer);
  if(strstr(buffer, "Please enter your name:")){
    break;
  }
  memset(buffer, 0, BUFFER_SIZE);
  }
  
  
  // send input name
  fgets(buffer, BUFFER_SIZE, stdin); // for input for client-server
  buffer[strcspn(buffer, "\n")] = 0;
  send(sockfd, buffer, strlen(buffer), 0);
  
  // recieve response from server using while loop (the response is extracted from txt
  
  while(1){
  bytes= recv(sockfd, buffer, BUFFER_SIZE-1, 0);

  if(bytes<0){
    perror("recv");
    exit(1);
  }
  buffer[bytes] = '\0';
  
  printf("%s\n", buffer);
  
  if(strstr(buffer, "")){
    break;
  }
  memset(buffer, 0, BUFFER_SIZE);
  }
  //close
  
  close(sockfd);
  
  return 0;
  
}
