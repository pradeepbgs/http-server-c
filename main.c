#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3000
#define BUFFER_SIZE 1024

int create_server_socket();
void handle_connection(int client_address);

int main(){
  int server_fd = create_server_socket();
  if (server_fd == -1) {
    perror("Failed to  create server\n");
    return -1;
  }
  printf("Server started listening on port: %d\n",PORT);
  
  // step 5 start listening 
  struct sockaddr_in client_address;
  socklen_t addrlen = sizeof(client_address);
  
  while (1) {
    
    int client_socket = accept(server_fd, (struct sockaddr*)&client_address,&addrlen);
    if (client_socket < 0) {
      perror("Accept failed");
      close(server_fd);
      return -1;
    }
    
   handle_connection(client_socket);
  }
  // close socket after req done
  close(server_fd);
  return 0;
}

int create_server_socket(){
  int server_fd;
  struct sockaddr_in address;
  int opt = 1;
  
  // create a socket (ipv4,TCP)
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  
  // step2: setting socket option (optional but usefull)
  if (setsockopt(server_fd,     SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    perror("setsocket failed\n");
    close(server_fd);
    return -1;
  }
  
  // step 3 : binding the socket to PORT and IP address(INADDR_ANY means any local IP)
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; //// Bind to any address
  address.sin_port = htons(PORT); // convert port to network type byteorder
  
  if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
    perror("bind failed\n");
    exit(EXIT_FAILURE);
  }
  
  // step 4 start listening for incoming req
    if (listen(server_fd, 10) < 0) {
            perror("Listen failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
    
 return  server_fd; 
}

void handle_connection(int client_socket){
  char buffer[BUFFER_SIZE] = {0};
  int valread = read(client_socket,buffer,BUFFER_SIZE); 
  if (valread <0) {
    perror("Read failed");
     close(client_socket);
  }
  // printf("Received message from client: %s\n", buffer);
     
  // create a handle_connection handler ()
  char *jsonRes = "{\"message\": \"Hello from socket!\"}";
  
  // char response [BUFFER_SIZE];
  // snprintf(response, sizeof(response),
  //         "HTTP/1.1 200 OK\r\n"
  //         "Content-Type: application/json\r\n"
  //         "Content-Length: %zu\r\n"
  //         "Connection: close\r\n\r\n"
  //         "%s",
  //         strlen(jsonRes),
  //         jsonRes
  //     );
  // or simple
  
  const char *response = 
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/plain\r\n"
  "\r\n"
  "Hello from server";
  write(client_socket, response,strlen(response));     
  close(client_socket);
  
}
