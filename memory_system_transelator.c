#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>
#include "config.h"



struct memory_token {
  char number[MAXSIZE];
  char image[MAXSIZE];
};

struct memory_token* csv_to_memory_array(char* memory_token_deck_path,int* p_amount_of_tokens) {
    int size_of_memory_token_array = 50;
    struct memory_token* memory_token_array = malloc(size_of_memory_token_array*sizeof(struct memory_token));
    char buffer[MAXSIZE];
    FILE *deck = NULL;
    int flag = 0;
    int lines = 0;
    deck = fopen(memory_token_deck_path,"r");
    while (fgets(buffer, MAXSIZE, deck)) {
        char *token = strtok(buffer, ",");
	/* Make a struct memory_token to add to memory_token_array */
	struct memory_token tmp_memory_token = {};
        while (token) {
	    /* this will be 4 each time. The promt, answer, date and delay */
	    int parse_helper = flag % 2;
	    switch(parse_helper) {
	    	case 0:
			/* parse prompt */
			strcpy(tmp_memory_token.number,token);
			break;
	    	case 1:
			/* parse answer */
			strcpy(tmp_memory_token.image,token);
			break;
		default:
			continue;
			;;
	    }
	    ++flag;
            token = strtok(NULL, ",");
        }
	/* append to memory_token array  */
	if (lines>=size_of_memory_token_array) {
		size_of_memory_token_array*=2;
		memory_token_array = realloc(memory_token_array,size_of_memory_token_array*sizeof(struct memory_token));
	}
	memory_token_array[lines] = tmp_memory_token;
	++lines;
    }
    	*p_amount_of_tokens = lines;
	return memory_token_array;
}


char* number_to_image(char* number_string, struct memory_token* memory_token_array,int size_of_array) {
	for (int i=0;i<size_of_array;i++) {
		struct memory_token cur_token = memory_token_array[i];
		if (strcmp(number_string,cur_token.number) == 0) {
			char* image_string = calloc(1,sizeof(cur_token.image));
			strcpy(image_string,cur_token.image);
			return image_string;
		}
	}
	char* image_string = calloc(20,sizeof(char));
	char* fourohfour = "404";
	strcpy(image_string,fourohfour);
	return image_string;

}

void slice(char* str, char *result, size_t start, size_t end)
{
    strncpy(result, str + start, end - start);
}

void main(int argc, char **argv) {
	/* Define input character array with big capacity and a variable to keep track of input leng*/
	char input_string[MAXSIZE];
	int input_length;

	/* Check how the user wants to read in the digits to be transelated */
	if (argc<2) {
		printf("Please use format ./sltranselator -i \"numbers\" or \n ./sltranselator -f file_name for files with numbers to be transelated \n or -std for command line input stdin\n");
		exit(0);
	}
	if (strncmp("-i",argv[1],2) == 0) {
		/* Read in from stdin, the length works fine here */
		strcpy(input_string,argv[2]);
		input_length = strlen(input_string);
	}
	else if (strncmp("-f",argv[1],2) == 0) {
		/* Read in from file, the length over counts by one for the null charater */
		char* file_with_numbers = argv[2];
		FILE* fp = fopen(file_with_numbers,"r");
		char* buffer = NULL;
		size_t len;
		ssize_t bytes_read = getdelim( &buffer, &len, '\0', fp);
		if ( bytes_read != -1) {
			strcpy(input_string,buffer);
			input_length = strlen(input_string) - 1;
		}
		else {
			printf("\ncould not read file\n");
			exit(0);
		}

	}
	else if (strncmp("-std",argv[1],2) == 0) {
		/* Read in from stdin, the length over counts by one for the null charater */
		get_user_input:
	 	memset(input_string, '\0', strlen(input_string));

	 	printf("Enter numbers to be transelated: \n");
    	 	fgets(input_string,MAXSIZE, stdin);
		input_length = strlen(input_string) - 1;
	}
	else {
		printf("Please use format ./sltranselator -i \"numbers\" \n or ./sltranselator -f file_name for files with numbers to be transelated \n or -std for command line input stdin\n");
		exit(0);
	}


	/* Read in the memory system from csv file into an array of structs (TO DO, make a macro for this) */
	int amount_of_tokens;
	struct memory_token* memory_token_array =  csv_to_memory_array(PATH_TO_SYSTEM,&amount_of_tokens);


	while (input_length % DIGITS_PER_IMAGE != 0) {
	/* Adjust input lengh by adding 0s until it can be transelated cleanly */
		input_string[input_length] = '0';
		input_string[input_length + 1] = '\0';
		input_length++;
	}

	for (int i=0;i<input_length;i+=DIGITS_PER_IMAGE) {
		/*
		   Jump forward by digits per image length and transelate each

		   We slice the digits we need into digits
		*/

		/* example 0 1 2 3 4 5 -> 0 1 2 and 3 4 5 */
		char digits[DIGITS_PER_IMAGE];
		slice(input_string,digits,i,i+DIGITS_PER_IMAGE);
		char* image = number_to_image(digits,memory_token_array,amount_of_tokens);
		printf("%s\n",digits);
		if (strncmp(image,"404",3) != 0) {
			printf("%s\n",image);
		}
		else {
			printf("\nImage no found\n");
		}
		free(image);

	}
	if (strncmp("-std",argv[1],2) == 0) {
		/* If user used stdin, we will ask them for more digits */
		goto get_user_input;
	}

};

