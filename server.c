/*
Programmer: Justine Tran 4950, Edgard Luigi Sanchez 0191
Program: server.c
Date: Nov 11, 2016: 01:53
Description: This program receives a request(ID) from a client,
             generates a key specifically for that client, 
             Server looks up the id from the table,
             retrieves and encrypts record using the key and sends the encrypted record to
             the client where it is decrypted using the unique key. 

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
#include <time.h>

#define MAX_FILE_LENGTH 256
#define STUDENT_ANSWER_SOCKET_PORT 9000
void encrypt(char record[],int key)
{
    unsigned int i;
    for(i=0;i<strlen(record);++i)
    {
        //simple but effective encryption
        record[i] = record[i] - key;
        record[i] = record[i] ^ key;
    }
}

int main() {
   char record[MAX_FILE_LENGTH];
   int id = 0;
   char keyStr[MAX_FILE_LENGTH], idStr[MAX_FILE_LENGTH];

   /*parallel array to identify student id - not the best programming
   method but for the sake of simplicity to practice sockets*/
  //create an array of 10 student.
  int idArr[] ={674, 003, 747, 503, 232, 839, 432, 988, 493, 372};
  char *student[] = {"Karl Lam, Electric Engineer, 674",
                    "John Smith, Computer Science, 003",
                    "Jane Berry, Mechanical Engineer, 747",
                    "Lary Dane, Psychology, 503",
                    "Matt Moore, Physics, 232",
                    "Noreen Lee, Computer Science, 839",
                    "Sandy Mayer, Biology, 432",
                    "Dan Dee, Astronomy, 988",
                    "Lynn Mao, Mathematics, 493",
                    "Andy Lee, Chemistry, 372"};

   
   //file descriptors to store values of socket/accept calls
   int listenfd = 0, acceptConnfd = 0; 
   struct sockaddr_in addrofserver; //from netinet/in.h

   //creates new socket (TCP not UDP)
   listenfd = socket(AF_INET, SOCK_STREAM, 0); 
   if(listenfd == -1)
	 printf("Error in creating socket!\n");
    
   //set address and port number
   addrofserver.sin_family = AF_INET;
   addrofserver.sin_addr.s_addr = inet_addr("127.0.0.1"); //Connection at loopback address
   addrofserver.sin_port = htons(STUDENT_ANSWER_SOCKET_PORT); //converts port number to network byte order

   //bind socket to an address on host
   if(bind(listenfd, (struct sockaddr *) &addrofserver, sizeof(addrofserver)) == -1)
   	printf("Error binding!\n");

   //listen socket for connection, set at max num connection 5
   listen(listenfd, 5); 

   while(1)
   {
          //block system until client connects, doesn't return client address
         acceptConnfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
         
         if(acceptConnfd == -1)
         	printf("Error accepting connection!\n");

         //Receives ID from client
         if (recv(acceptConnfd, idStr, MAX_FILE_LENGTH,0) == -1)
         	printf("Error: ID is not received from client!\n");
         printf("\nSuccessfully received request.\n");
         //Convert ID string into int
         sscanf(idStr,"%d",&id);

        int isFound = 0;
        //Find where ID resides in the record
        for(int i = 0; i < 10; i++)
        {
            if(id == idArr[i])
            {
              memset(record,0,sizeof(record)); //reseting buffer 
              // copy data to record
              strcpy(record,student[i]);
              isFound = 1;
              break; // break out when found
            }
        }
       	//Sets idStr as -1 and send back to client if ID is not found
        if(isFound == 0)
        {
          snprintf(keyStr, 10, "%d", -1);
          printf("Attempt to access invalid ID.\n\n");
          if (send(acceptConnfd, keyStr, MAX_FILE_LENGTH,0) == -1)
          	printf("Error: ID was not sent to client.\n");
        }
        else
        {
             //Generate random key  
             srand(time(NULL));
             // 64 = char A, reduce the probability to get alpha char is in the encryption display.
             int randKey = abs((rand()%100)-64);
             //convert it into string
             snprintf(keyStr, 10, "%d", randKey);

             //Encrypt data with random key in preparation to send back to client
             encrypt(record, randKey);
             
             //Send key to client
             printf("Sending key to client...\n");
             if (send(acceptConnfd, keyStr, MAX_FILE_LENGTH,0) == -1)
             	printf("Error: Key was not sent to client.\n");
            
             printf("Done sending key to client.\n\n");

             //Send encrypted data to client
             printf("Sending encrypted data to client.\n");
             if (send(acceptConnfd, record, MAX_FILE_LENGTH,0) == -1)
             	printf("Error: Encrypted data was not sent to client.\n");
             
             printf("Done sending encrypted data to client.\n\n");
          
        }
          printf("Finished servicing a client.\n");
          printf("...........................................\n");
         //close sockets
         close(acceptConnfd);
         sleep(1);
   }

   return 0;
}
