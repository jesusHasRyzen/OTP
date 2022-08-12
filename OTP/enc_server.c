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

static char const alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

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
char otp_encryption(char buffer, char key)
{
    int buffValue, keyValue, returnValue;
    for(int j =0; j < 27; j ++)
    {
        if(buffer == alphabet[j])
        {
            buffValue = j;
//            printf("enc server : plaintext char value: %d\n", buffValue);
        }
        if(key == alphabet[j])
        {
            keyValue = j;
//            printf("enc server : key char value: %d\n", keyValue);
        }
    }
    

    returnValue = buffValue + keyValue;
    
//    printf("return char value before mod: %d\n", returnValue);
    if (returnValue > 26) {
        returnValue = returnValue - 27;
    }
    returnValue =  returnValue % 27;
//    printf("return char value after mod: %d\n", returnValue);
//    printf("the char value encrypted is %c\n", alphabet[returnValue]);
    return alphabet[returnValue];
}
void send_all(int socket, char *buffer, int lenght)
{
//    printf("contents of buffer : %s\n", buffer);

    char *ptr = buffer;
    while (lenght > 0)
    {
        int charsWritten = send(socket, ptr, lenght, 0);
        if (charsWritten < 0){
          error("CLIENT: ERROR writing to socket");
        }
        if (charsWritten < strlen(ptr)){
          error("CLIENT: WARNING: Not all data written to socket!\n");
        }
//        printf("contents of ptr : %c\n", *ptr);

        ptr = ptr + charsWritten;
//        printf("contents of ptr : %s\n", *ptr);
        lenght = lenght - charsWritten;
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
    memset(buffer_key, '\0', sizeof(buffer_key));
    memset(plaintext, '\0', sizeof(plaintext));


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
  listen(listenSocket, 10);
    
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
//              printf("SERVER: entered the break in plaintext\n");
              break;
          }
//          printf("SERVER plaintext : I received this from the client: \"%s\"\n", buffer);
    if (charsRead < 0){
      error("SERVER : ERROR reading from socket");
    }
      }
      do
      {
//          printf("SERVER: test before key while loop\n");
          memset(buffer, '\0', 256);
          charsRead = recv(connectionSocket, buffer, 255, 0);
//          set_key(buffer);
          total_keytext_read = total_keytext_read + charsRead;
//          printf("SERVER:  count of key chars read %d\n", total_keytext_read);
          strcat(buffer_key, buffer);
          if(strchr(buffer, '\n') != NULL)
          {
//              printf("SERVER: entered the break in key\n");
              break;
          }
//          printf("SERVER key: I received this from the client: \"%s\"\n", buffer);
//          printf("SERVER key: I got this saved int he buffer key \"%s\"\n", buffer_key);
    if (charsRead < 0 ){
      error("SERVER : ERROR reading from socket");
    }
      }while(charsRead > 0 && total_keytext_read < total_plaintext_read);
//      printf("SERVER: enc_server : after key while loop\n");

//      printf("SERVER: testing errors before encryption\n");
      char encryptedText[total_plaintext_read];
      for (int i = 0; i < total_plaintext_read-1; i++) {
      encryptedText[i] = otp_encryption(plaintext[i], buffer_key[i]);
      }
      
//      printf("SERVER: testing errors after encryption\n");

//      printf("SERVER:  %s\n", encryptedText);
      encryptedText[strlen(encryptedText)] = '\n';
      send_all(connectionSocket, encryptedText, total_keytext_read);

    if (charsRead < 0){
      error("SERVER: ERROR writing to socket");
    }
    // Close the connection socket for this client
    close(connectionSocket);
  }
  
  // Close the listening socket
  close(listenSocket);
  return 0;
}
