//
//  enc_server.c
//  OTP
//
//  Created by Jesus Ponce on 8/8/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int num_key[1000];
//int plaintext[1000];

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
}
//void settext(char* buf)
//{
//    int buf_length = strlen(buf);
//    for(int i = 0; i < buf_length; i ++)
//    {
//        plaintext[i] = buf[i];
//    }
//}
void set_key(char* buf)
{
    int buf_length = strlen(buf);
    for(int i = 0; i < buf_length; i ++)
    {
        num_key[i] = buf[i];
    }
}



// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address));

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
    int tCount = 0;
  int connectionSocket, charsRead;
  char buffer[256];
  char buffer_key[1000];
  char plaintext[1000];
    int total_plaintext_read = 0;
    int total_keytext_read = 0;

  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) {
    fprintf(stderr,"USAGE: %s port\n", argv[0]);
    exit(1);
  }
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("SERVER: ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket,
          (struct sockaddr *)&serverAddress,
          sizeof(serverAddress)) < 0){
    error("SERVER: ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5);
    
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
    if (connectionSocket < 0){
      error("SERVER : ERROR on accept");
    }
    printf("SERVER: Connected to client running at host %d port %d\n",
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));
    // Get the message from the client and display it
    memset(buffer, '\0', 256);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, 255, 0);
    if(strcmp(buffer, "enc_client") != 0)
    {
        error("SERVER: Error, wrong client attempting to connect to enc_server\n");
    }
      char enc_text[1000];

      while(charsRead > 0 )
      {
          memset(buffer, '\0', 256);
          // Read the client's message from the socket
          charsRead = recv(connectionSocket, buffer, 255, 0);
          total_plaintext_read = total_plaintext_read + charsRead;
//          settext(buffer);
          strcat(plaintext, buffer);
          if(strchr(buffer, '\n') != NULL)
          {
              printf("entered the break in plaintext\n");
              break;
          }
          printf("SERVER plaintext : I received this from the client: \"%s\"\n", buffer);
    if (charsRead < 0){
      error("SERVER : ERROR reading from socket");
    }
      }
      do
      {
          memset(buffer, '\0', 256);
          charsRead = recv(connectionSocket, buffer, 256, 0);
//          set_key(buffer);
          total_keytext_read = total_keytext_read + charsRead;
          strcat(buffer_key, buffer);
          printf("SERVER key: I received this from the client: \"%s\"\n", buffer);
          printf("SERVER key: I got this saved int he buffer key \"%s\"\n", buffer_key);
    if (charsRead < 0){
      error("SERVER : ERROR reading from socket");
    }
      }while(charsRead > 0);
      
//    printf("SERVER: I received this from the client: \"%s\"\n", buffer);
    // Send a Success message back to the client
    charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
      printf("contents of plaintext\n");

      for (int i = 0; i < total_plaintext_read; i++) {
          printf("%c", plaintext[i]);
      }
      printf("\ncontents of numkey\n");

      for (int i = 0; i < total_keytext_read; i++) {
          printf("%c", buffer_key[i]);
      }
    if (charsRead < 0){
      error("ERROR writing to socket");
    }
    // Close the connection socket for this client
    close(connectionSocket);
  }
  
  // Close the listening socket
  close(listenSocket);
  return 0;
}
