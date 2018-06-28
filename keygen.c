
//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int main(int argc, char** argv)
{

	srand(time(NULL));
	//Cite:https://stackoverflow.com/questions/24809443/how-to-generate-random-number-from-a-given-set-of-values
	if (argc < 2)
	{

		printf("Not enough arguments!");
	}


	//make the array consist of ASCII code for capital letters A-Z and 32, space
int length= atoi(argv[1]);	


	int i;
	for (i = 0; i < length; i++)

	{
		int keySelection[27] = { 65,66,67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	77,	78,	79,	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,32 };
		int randomIndex = rand() % 27; //generate random index from values -26 which are indices of the array 

		//generate random from an array


		int randomValue = keySelection[randomIndex];
		fprintf(stdout, "%c", randomValue);// the ascii value gets converted to char 
	}
	fprintf(stdout, "\n");


    return 0;
}

