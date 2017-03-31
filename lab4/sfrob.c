#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define true 1
#define false 0

const char SPACE = ' ';
const char NEWLINE = '\n';

int frobcmp(char const* a, char const* b);
int frobcmp_wrapper(const void* a, const void* b);

int main(void) {
    int i_line_size = 30;
    //allocate memory - must explicitly cast to char*
    char* input = (char*)malloc(sizeof(char) * i_line_size);
    //read byte by byte
    char curr;
    int i, char_count, num_words;
    for (i = 0, char_count = 0, num_words = 0, curr = getchar();curr != EOF;i++, curr = getchar()) {
	//check error reading stdin
	if (ferror(stdin)) {
	    fprintf(stderr, "Error reading input");
	    exit(1);
	}
	//if necessary, double the input buffer size
	if (i == i_line_size) {
	    i_line_size *= 2;
	    input = (char*)realloc(input, sizeof(char) * i_line_size);
	    //check for realloc failure
	    if (input == NULL) {
		fprintf(stderr, "Could not reallocate memory.");
		exit(1);
	    }
	}
	//once EOF is reached
	if (curr == EOF) {
	    printf("this is eof\n");
	    //if EOF is the first char, the file is empty
	    if (i == 0) {
		free(input);
		return 0;
	    }
	    //append a space at the end of the file if there isn't one
	    if (char_count > 0 && input[char_count-1] != SPACE) {
		input[char_count] = SPACE;
		char_count++;
	    }
	}
	//otherwise, append the chars to the input buffer
	else {
	    //check for newlines or spaces
	    if (curr == SPACE) {
		//check if the current char in the input buffer is a space
		if (char_count > 0 && input[char_count-1] == SPACE) continue;
		//if it's not, append the space normally and increment the word count
		else if (char_count != 0) {
		    input[char_count] = SPACE;
		    char_count++;
		    num_words++;
		}
	    }
	    //otherwise append the char normally
	    else {
		input[char_count] = curr;
		char_count++;
	    }
	}
    }
    char_count--;
    //check for ending space one more time
    if (input[char_count - 1] != SPACE) input[++char_count] = SPACE;
    /* now that we have a uniformly space-separated input buffer, we
       can split the words into an array */
    //allocate a c-string arr with num_words pointers
    char** word_arr = (char**)malloc(sizeof(char*) * (num_words + 1));
    int end, beg, word_count;
    //loop through input buffer
    for (end = 0, beg = 0, word_count = 0;end<=char_count;end++) {
	//if you've found a space, you've found a new word
	if (input[end] == SPACE) {
	    //allocate enough space for the word
	    word_arr[word_count] = (char*)malloc(sizeof(char) * (end - beg));
	    //and add the chars in
	    int j;
	    for (j = beg;j<=end;j++) {
		word_arr[word_count][j - beg] = input[j];
	    }
	    //increment word counter
	    word_count++;
	    //update beg
	    beg = end + 1;
	}
    }
    //it's sortin' time
    qsort(word_arr, word_count, sizeof(char*), frobcmp_wrapper);
    //now let's output the results
    int j;
    for (j = 0;j<word_count;j++) {
	//temp stores a word
	char* temp = word_arr[j];
	//loop through word chars
	while(true) {
	    //use putchar to write to stdout
	    putchar(*temp);
	    //if you find space, break and move on to next char
	    if (*temp++ == SPACE) break;
	}
	//free each word as you get to it to avoid looping again
	free(word_arr[j]);
    }
    //now free the input buffer and the word_arr
    free(input);
    free(word_arr);
}


/*
Takes a and b as input, and returns an int result that is negative,
zero, or positive depending on whether a is less than, equal to, or
greater than b.
*/
int frobcmp(char const* a, char const* b) {
    char atemp, btemp;
    int count = 0;
    //memfrob does an XOR on each byte w/ the number 42
    while (true) {
	atemp = a[count];
	btemp = b[count];
	//check if both are space bytes
	if (atemp == SPACE && btemp == SPACE) return 0;
	//check if a < b or a is a space byte and b isn't
	if ((atemp^42) < (btemp^42) || (atemp == SPACE && btemp != SPACE)) return -1;
	//check if a > b or b is a space byte and a isn't
	if ((atemp^42) > (btemp^42) || (btemp == SPACE && atemp != SPACE)) return 1;
	//increment counter
	count++;
    }
}

/*
Because qsort requires a comparator function of a different type
than frobcmp, we must wrap frobcmp
*/
int frobcmp_wrapper(const void* a, const void* b) {
    char const* amod = *(char const**)a;
    char const* bmod = *(char const**)b;
    return frobcmp(amod, bmod);
}


