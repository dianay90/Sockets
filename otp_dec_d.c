
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


int MAX= 100000,status; 
int filenameFlag=0;
int messageFlag=0;

//CITE OSU CS344 Lectures and Notes and CS344 Piazza Posts 

//perror prints to stderr

void error(const char*msg) {perror(msg); exit(1);}

int main(int argc, char *argv[])
{

//create buffer to hold file, key, and decryption text

char key[MAX], fileName[MAX];
char completeMessage[MAX], readBuffer[MAX];
char completeMessagev2[MAX], readBufferv2[MAX];
char keyv3[MAX], filev3[MAX];
char decrypt[MAX];
//make variables to create socket connection 
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, r,spawnpid;
	socklen_t sizeOfClientInfo;
	char buffer[256];
//char buffer[MAX];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

//perror prints to stderr!!!!



		// Accept a connection, blocking if one is not available until one connects

while(1)
{
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept

		if (establishedConnectionFD < 0) error("ERROR on accept");
//fork process 
		spawnpid = fork();

		switch (spawnpid)
		{

		case -1: 
			perror("Failed to create process");
			exit(1);
			break; 

		case 0: //child

			//Cite OSU CS344 Lectures 

			memset(completeMessage, '\0', sizeof(completeMessage)); // Clear the buffer

			int index=0;
			while (strstr(completeMessage, "%") == NULL) // As long as we haven't found the terminal...

		{

				memset(readBuffer, '\0', sizeof(readBuffer)); // Clear the buffer

				r = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk
			
strcat(completeMessage, readBuffer); // Add that chunk to what we have so far
//				printf("PARENT: Message received from child: \"%s\", total: \"%s\"\n", readBuffer, completeMessage);
				if (r == -1) { printf("r == -1\n"); break; } // Check for errors
				if (r == 0) { printf("r == 0\n"); break; }
	index++;
			}


//check if the file is coming from enc. enc will always send a ~ first
if(completeMessage[0]== '~')
{

//if file is coming from enc send message to enc client to print error message and exit 

				send(establishedConnectionFD, "!", 1, 0);

				
}


int j=0;
 index=0;//start at 0 here since dec message does not begin with ~ while enc does 
while(completeMessage[j]!='@') //traverse until you hit the key delimiter
{

keyv3[j]= completeMessage[j];
//printf("%c %c", keyv3[index], completeMessage[index]);

index++;j++;
}


//printf("key  is %s\n",keyv3);

//move it forward one 
index++; 
int k=0;//traverse until you hit the message delimiter 
while (completeMessage[index]!= '%')

{

filev3[k]=completeMessage[index];

//printf("%c %c", filev3[k], completeMessage[index]);

index++; k++;
}
//printf("file3 lengths %d, %s\n",strlen(filev3),filev3);

//printf("filename  is %s\n",filev3);


filenameFlag=1;
messageFlag=1;
 
//Cite: Decrypt idea from post on piazza https://piazza.com/class/jf1c8stzym5f8?cid=213
//Cite: https://piazza.com/class/jf1c8stzym5f8?cid=226

//char* usableChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

				memset(decrypt, '\0', sizeof(decrypt)); // Clear the buffer


if (filenameFlag && messageFlag)
{
			int i, temp; 
			//start i at one to ignore the sentinel
	for (i = 0;i < strlen(filev3)-1; i++)

{

temp=0;
//remap space to left bracket
if(filev3[i] == 32)
{
filev3[i]= 91;
}

//remap space to left bracket
if (keyv3[i] == 32)
{
keyv3[i]=91;

}

//do reverse of encrypt:  frame shift letter s to have an ascii value of 0-26 and subtract final value 
temp= (filev3[i]-65) -(keyv3[i]-65);

//if negative then add 27. add 27 since letters have been frame shifted to  be in range 0-26
if (temp<0)
{
temp=temp+27;
}

//shift letters back by adding 65. 
decrypt[i]=65+temp;


//remap  bracket to space 
if (decrypt[i] == 91)
{
decrypt[i]= 32;
}


}

decrypt[i]= '\0';



	


//printf("SERER: dec text %s\n",decrypt);
//send decrypted text to client 

charsRead= send(establishedConnectionFD, decrypt, strlen(decrypt), 0);




			if (charsRead< 0) error("error sending to socket");
//close child socket
			close(establishedConnectionFD);
			exit(0);
}


			break; 


		default: 
			waitpid(spawnpid, &status, 0);//clean up child process
		}


	}

	close(listenSocketFD); // Close the listening socket

	return 0; 
}
