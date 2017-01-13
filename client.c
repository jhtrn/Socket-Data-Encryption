/*
Programmer: Justine Tran 4950, Edgard Luigi Sanchez 0191
Program: client.c 
Date: Nov 11, 2016 : 01:53
Description: This program sends a request to the server,
             waits for the server to send client a unique key,
             receives an encrypted record from the server,
             decrypts the record using the unique key and displays the record 
             the terminal.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_FILE_LENGTH 256
#define STUDENT_ANSWER_SOCKET_PORT 9000
void decrypt(char record[],int key)
{
    unsigned int i;
    for(i=0;i<strlen(record);++i)
    {
        //simple but effective decryption
        record[i] = record[i] ^ key;
        record[i] = record[i] + key;
    }
}

int main()
 {
   char record[MAX_FILE_LENGTH];
   char keyStr[MAX_FILE_LENGTH], idStr[MAX_FILE_LENGTH];
   int key = 0, clientKey = 0;
   
   int connectfd = 0;
   struct sockaddr_in addrofserver;
    
   //create new socket (TCP not UDP)
   connectfd = socket(AF_INET, SOCK_STREAM, 0);
   if(connectfd == -1)
	 printf("Error creating socket!\n");

   //set address and port number
   addrofserver.sin_family = AF_INET;
   addrofserver.sin_addr.s_addr = inet_addr("127.0.0.1");
   addrofserver.sin_port = htons(STUDENT_ANSWER_SOCKET_PORT);
    
   //connect client socket to server addr
   if (connect(connectfd,(struct sockaddr *) &addrofserver, sizeof(addrofserver)) == -1) 
        printf("Error with connection\n");
   
   //Prompt user for ID
   printf("\nPlease enter your ID: ");
   scanf("%s", idStr);

   //Send server ID to retrieve data from server
   if (send(connectfd, idStr, MAX_FILE_LENGTH,0) == -1)
	 printf("ID was not sent to server.\n");
   
   printf("Sending...\nDone sending ID to server.\n\n");

   //Receive key string from server
   if (recv(connectfd, keyStr, MAX_FILE_LENGTH,0) == -1)
   printf("Error: Key is not received from server!\n");

   //Convert key string into int 
   sscanf(keyStr,"%d",&key);

   //if key is -1, ID was not found. terminate program
   if(key == -1)
   {
      printf("ID not found, terminating client session...\n\n" );
      exit(0);
   }
   //Continues program if ID was found
   else
   {
      //Receive encrypted data string from server
      if (recv(connectfd, record, MAX_FILE_LENGTH,0) == -1)
    	   printf("Error: Encrypted data is not received from server!\n");

      //Outputs key to client 
      printf("Key obtained from server: %s",keyStr);
      printf("\nEncrypted record: %s", record);

      //Prompts client for key to decrypt data
      printf("\nPlease enter a key to get decrypted record: ");
      scanf("%d", &clientKey);
  
      if(clientKey != key)
      {
         printf("Key is incorrect, terminating client session...\n\n" );
         exit(0);
      }
      else
      {
         decrypt(record,clientKey);
         printf("Decrypted student record: %s\n", record);
         printf("\n");
      }
   }
  
   //close socket
   close(connectfd); 

   return 0;
}
