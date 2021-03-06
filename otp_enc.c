
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//Cite CS344 Lectures and notes 
int MAX = 1000000, MAX2= 1000020; 
int match = 0; 
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
//Cite https://stackoverflow.com/questions/4823177/reading-a-file-character-by-character-in-c

char *readAndInspect(char *fileName) {
//keySelection consists of allowable characters and space 
	int keySelection[27] = { 65,66,67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	77,	78,	79,	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,32 };
	int i, tempCode;
	FILE *file = fopen(fileName, "r");
	char *code;
	size_t n = 0;
	int c;

	if (file == NULL) return NULL; //could not open file

//use fseek to determine end of file 
	fseek(file, 0, SEEK_END);
	long f_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	code = malloc(f_size);
//traverse until file end
	while ((c = fgetc(file)) != EOF) {
		code[n] = (char)c;
			tempCode = code[n];

		for (i = 0; i < 27; i++)
		{
//for loop compares temp code against every other valid letter to see if there are bad characters			

//			printf("%c", tempCode);
			if (tempCode == keySelection[i]|| tempCode =='\n')
			{
				match = 1;
			}
		}
//if not match there were bad characters not part of keySelection array 		
			if (!match)
			{
				fprintf(stderr, "ERROR:Invalid characters in file %s\n", fileName);
				exit(1);

			}

		n++; 
	}

	code[n] = '\0';

	return code;
}




int main(int argc, char *argv[])
{
//create variables to start socket
	int socketFD, portNumber, charsWritten, charsRead, charsWrittenv2;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

//create buffers to store key, message, and encrypted message 

	char buffer[MAX];
char stop[3];
char final[MAX2]; 
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

																						 
//inspect for bad characters																					 
	char* fileName = readAndInspect(argv[1]);
	char* key = readAndInspect(argv[2]);

	//if the files make it to this point there are no bad characters

	//check if the key is too short 
	if (strlen(fileName) >  strlen(key))
	{
		fprintf(stderr, "ERROR:Key is too short\n");
		exit(1);

	}

	char fileNamev2[MAX];
	char keyv2[MAX];

//files coming from enc will have a ~ before them

	keyv2[0] = '~';

//concatanate buffers 
	strcat(fileNamev2, fileName);
	strcat(keyv2, key);
//find end of strings to add delimiters
int delimiter= strlen(fileNamev2);
int delimiterv2= strlen(keyv2);

//add terminating message for key and message
fileNamev2[delimiter]='%';
keyv2[delimiterv2]='@';


	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");
	


	 //Send message to server

charsWrittenv2=0; 
	while (charsWrittenv2<strlen(keyv2))
	{

//send key to server
		charsWrittenv2 = send(socketFD, keyv2, strlen(keyv2),0); // Write to the server

//error message sent in case error writing to socket or data is not done being read 
		if (charsWrittenv2 < 0) error("CLIENT: ERROR writing to socket");
		if (charsWrittenv2 < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	}

charsWritten=0;
	while (charsWritten< strlen(fileNamev2))
	{

//send file to the server

		charsWritten = send(socketFD, fileNamev2, strlen(fileNamev2),0); // Write to the server
//error message sent in case  error writing to socket or data is not being read
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	
	}

//receive message from server in case enc tries to connect to the incorrect server
memset(stop, '\0',sizeof(stop));
charsRead=recv(socketFD,stop, sizeof(stop)-1, 0);
if (charsRead<0) error("CLIENT: ERROR readings from socket");
if(stop[0]=='!')
{
//if server sends back ! that means enc tried to connect with wrong server
 fprintf(stderr,"ERROR:otp_enc cannot use otp_dec_d.Port number:%d", portNumber);

exit(2); }


	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

//clear buffer
memset(final, '\0', sizeof(final)); 

//concatenate stop buffer and buffer for final message
strcat(final,stop);
strcat(final,buffer); 
//print the final message 
printf("%s\n",final);


	close(socketFD); // Close the socket
	return 0;
}
