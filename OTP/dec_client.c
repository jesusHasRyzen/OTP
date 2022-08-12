//
//  dec_client.c
//  OTP
//
//  Created by Jesus Ponce on 8/8/22.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(0);
}

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber,
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address));

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname);
  if (hostInfo == NULL) {
    fprintf(stderr, "CLIENT: ERROR, no such host\n");
    exit(0);
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr,
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

void send_all(int socket, char *buffer, int lenght)
{
//    printf("contents of buffer : %s\n", buffer);

    char *ptr = buffer;
    while (lenght > 0)
    {
        int charsWritten = send(socket, ptr, lenght, 0);
        if (charsWritten < 0){
          error("CLIENT: ERROR writing to socket in key");
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


int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  char buffer[256];
  // Check usage & args
//  if (argc < 3) {
    if (argc < 4) {
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
    exit(0);
  }

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");
//  setupAddressStruct(&serverAddress, atoi(argv[2]), argv[1]);
    
  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }
  // Get input message from user
    char *header = "dec_client";
    char *key_header = "\n";
    charsWritten = send(socketFD, header, strlen(header), 0);
//+++++++++++++++++++++++++++++++++++++++++
    char *fileName = argv[1];
    FILE *fileDescriptor = fopen(fileName, "r");
//    char *keyfileName = argv[2];
//    FILE *keyDescriptor = fopen(keyfileName, "r");
    if(fileDescriptor == NULL)
    {
        error("error plaintext file\n");
        exit(1);
    }
    int fileContentSize = 0;
    int keyContentSize = 0;
//    char key_Contents[256];
    while(fgets(buffer, sizeof(buffer) - 1, fileDescriptor) != NULL)
    {
//          buffer[strcspn(buffer, "\n")] = '\0';
        fileContentSize = strlen(buffer);
        
        // check for valid inputs
        for (int i = 0; i < fileContentSize-1; i++) {
            if((buffer[i] == 32 || (buffer[i] > 64 && buffer[i] < 91)))
               {
                continue;
            }
               else{
                error("CLIENT : not valid character read in plain text file\n");
                exit(1);
            }
        }
        //send fileContents to server
        // Send message to server
        // Write to the server
//          charsWritten = send(socketFD, header, strlen(header), 0);
        buffer[strcspn(buffer, "\n")] = '\0';//+++++++++++++++++++++++++++++++++++++
//        printf("contents being passed to send %s\n", buffer);
        if(buffer[0] == '\0')
        {
//            printf("CLIENT : made into break loop\n");
            break;
        }
//        printf("CLIENT : sending: %s\n", buffer);
        charsWritten = send(socketFD, buffer, strlen(buffer), 0);
        if (charsWritten < 0){
          error("CLIENT: ERROR writing to socket in plaintext");
        }
        if (charsWritten < strlen(buffer)){
          printf("CLIENT: WARNING: Not all data written to socket!\n");
        }
//        printf("client  looping first while loop\n");

    }
//    printf("client pass the first while loop\n");
    fclose(fileDescriptor);
    char *keyfileName = argv[2];
    FILE *keyDescriptor = fopen(keyfileName, "r");
    if(keyDescriptor == NULL)
    {
        error("error key file\n");
        exit(1);
    }

    charsWritten = send(socketFD, key_header, strlen(key_header), 0);
    char key_Contents[256];
    int max_size = 256;
    int c;
    char *key_sent = malloc(max_size);
    if(key_sent == NULL)
    {
        printf("out of memory!!\n");
        exit(1);
    }
//    printf("client starting the key while loop\n");

    while (((c = fgetc(keyDescriptor)) != EOF))
    {
        if (keyContentSize == max_size-1) {
            max_size = max_size + 256;
            key_sent = realloc(key_sent, max_size);
        }
        key_sent[keyContentSize] = c;
        keyContentSize = keyContentSize +1;
    }
//        keyContentSize = strlen(key_Contents);
        // check for valid inputs
        for (int i = 0; i < keyContentSize-1; i++) {
            if((key_sent[i] == 32 || (key_sent[i] > 64 && key_sent[i] < 91)))
               {
                continue;
            }
               else{
                   printf("invalid char is :%c in count is %d\n", key_Contents[i], i);
                error("CLIENT : not valid character read in key file\n");
                exit(1);
            }
        }
//        key_sent[strcspn(buffer, "\n")] = '\0';
        if(key_sent[0] == '\0')
        {
//            printf("CLIENT : made into break loop\n");
//            break;
        }

//        printf("CLIENT : sending: %s\n", key_sent);
//    printf("contents of key_sent : %s\n", key_sent);
    send_all(socketFD, key_sent, strlen(key_sent));

//    printf("enc_client : after sendall\n");

//
  // Get return message from server
  // Clear out the buffer again for reuse
    char op_encryption[keyContentSize];
    int countRecv = 0;
    do{
//        printf("enc_client : inside do while for recv\n");
  memset(buffer, '\0', sizeof(buffer));
        memset(op_encryption, '\0', sizeof(op_encryption));

  // Read data from the socket, leaving \0 at end

  charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        strcat(op_encryption, buffer);
//        printf("CLIENT: I received this from the server in buffer : \"%s\"\n", buffer);

//        printf("length of buffer %zu\n", strlen(buffer));
        countRecv = countRecv + strlen(buffer);
//        printf("chars read at the end of enc_client %d\n", charsRead);
  if (charsRead < 0){
//      break;
    error("CLIENT: ERROR reading from socket line 215");
    }
//        strcat(op_encryption, buffer);
//        printf("client: the count recv is at %d\n", countRecv);
//        printf("CLIENT: I received this op_encryption[last]: \"%c\"\n", op_encryption[countRecv]);

//  printf("CLIENT: I received this from the server in op_encryption: \"%s\"\n", op_encryption);
        printf("%s", op_encryption);
    }while(charsRead > 0 && op_encryption[countRecv-1] != '\n');
//   Close the socket
  close(socketFD);
  return 0;
}
