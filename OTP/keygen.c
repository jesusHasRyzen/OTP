//
//  keygen.c
//  OTP
//
//  Created by Jesus Ponce on 8/8/22.
//

#include <stdio.h>
#include <stdlib.h> //atoi
#include <unistd.h> // write


static char const alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

int main(int argc, const char * argv[]) {
 
    if(argc < 2)
    {
        printf("need keylength!!!\n");
        return 1;
    }
    int keylength = atoi(argv[1]);
//    int key_position;
    char key;
    for(int i = 0; i < keylength; i++)
    {
        key= alphabet[rand() & 26];
        printf("%c", key);
    }
    
    return 0;
}
