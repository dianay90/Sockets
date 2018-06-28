
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


//CITE: CS344 Lectures and Notes, CS344 Piazza posts 	

void error(const char*msg) {perror(msg); exit(1);}

int main(int argc, char *argv[])
{
//Create filenames to hold the buffers for the key and file  and encryption file 

char key[MAX], fileName[MAX];
char completeMessage[MAX], readBuffer[MAX];
char completeMessagev2[MAX], readBufferv2[MAX];
char keyv3[MAX], filev3[MAX];
char encrypt[MAX];

//initialize buffers with memset
memset(encrypt, '\0', sizeof(encrypt));
memset(key, '\0', sizeof(key));
memset(fileName, '\0', sizeof(fileName));



	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, r,spawnpid;
	socklen_t sizeOfClientInfo;
	char buffer[256];
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



		// Accept a connection, blocking if one is not available until one connects

while(1)
{
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept

		if (establishedConnectionFD < 0) error("ERROR on accept");

//Fork to create child process 
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
if(completeMessage[0]!= '~')
{
//if the message doest not begin with ~ then it is NOT from enc, so reject it 

				send(establishedConnectionFD, "!", 1, 0);
//send ! to dec which will make dec print an error message and then exit 

}

//start the  index at 1 to ignore the ~
int j=0;
 index=1;
//traverse until you hit the first delimiter that ends the key 
while(completeMessage[index]!='@')
{

keyv3[j]= completeMessage[index];
//printf("%c %c", keyv3[index], completeMessage[index]);

index++;j++;//increment both indice trackers 
}


//printf("key  is %s\n",keyv3);

//move index forward one to ignore very next character not part of message

index++; 
int k=0;

//keep going until you hit the next delimiter that ends the message 

while (completeMessage[index]!= '%')

{

filev3[k]=completeMessage[index];

//printf("%c %c", filev3[k], completeMessage[index]);

index++; k++; //increment boith indice trackers
}


//printf("filename  is %s\n",filev3);

filenameFlag=1;
messageFlag=1; 


//char* usableChars = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	memset(encrypt, '\0', sizeof(encrypt)); // Clear the buffer


//Cite:Encrypt idea from piazza post https://piazza.com/class/jf1c8stzym5f8?cid=213

if (filenameFlag && messageFlag)
{
			int i, temp; 
			//start i at one to ignore the sentinel
	for (i = 0;i < strlen(filev3)-1; i++)

	{
//remap space to left bracket value 
	if(filev3[i] ==32)
{

filev3[i] =91;

}
//remap space to left brack value 
if (keyv3[i]== 32)
{

keyv3[i]= 91;
}			


//subtract 65 from the key and file so they  have ascii value of 0-26 
temp= (filev3[i]-65) +(keyv3[i]-65);


// modulus encryption and  shift letterse back to their normal values by adding 65
encrypt[i] = 65 +(temp % 27);

//remap brackets back to spaces 
if (encrypt[i] == 91)
{
encrypt[i] = 32;

}


	}
encrypt[i]='\0';
//printf("encrypted text %s\n",encrypt);

//send encrypted text to client 
charsRead= send(establishedConnectionFD, encrypt, strlen(encrypt), 0);


			if (charsRead< 0) error("error sending to socket");
//close child socket
			close(establishedConnectionFD);
			exit(0);
}

			break; 



		default: 
			waitpid(spawnpid, &status, 0); //parent will clean up child process
			//close(newSocket);

		}

	}


	close(listenSocketFD); // Close the listening socket
	return 0; 
}
