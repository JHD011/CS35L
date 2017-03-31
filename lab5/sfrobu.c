#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>

const char SPACE = ' ';
const char NEWLINE = '\n';

int frobcmp(char const* a, char const* b, bool ignore_case);
int frobcmp_wrapper(const void* a, const void* b);
int frobucmp_wrapper(const void* a, const void* b);

int main(int argc, char** argv) {
    int i;
    char* input;
    int i_line_size = 30;
    int input_size = 0;
    int buf_size = 0;
    int num_words = 0;
    bool ignore_case = false;
    //first off, let's check if the -f option was called
    if (argc > 2) {
	fprintf(stderr, "sfrobu only accepts a single argument");
	exit(1);
    }
    ignore_case = (argc == 2 && strcmp(argv[1], "-f") == 0);
    if (!ignore_case && argc == 2) {
	fprintf(stderr, argv[1]);
	fprintf(stderr, " is not an option");
	exit(1);
    }
    struct stat i_stat;
    //attempt to get file info with fstat
    if (fstat(0, &i_stat) != 0) {
	fprintf(stderr, "Error getting input data");
	exit(1);
    }
    //check for regular file using the S_ISREG macro
    if (S_ISREG(i_stat.st_mode)) {
	//if the file is regular, allocate enough memory from the start
	input = (char*)malloc(sizeof(char) * (i_stat.st_size + 20));
	//use read to get as much data as expected
	if (read(0, input, i_stat.st_size) > 0) {
	    //now do some uniform space-formatting
	    char* temp = (char*)malloc(sizeof(char) * (i_stat.st_size + 1));
	    buf_size = i_stat.st_size;
	    int i;
	    for(i = 0;i<i_stat.st_size;i++) {
		//only add non-space chars and the first space char in a sequence
		if (input[i] != SPACE || (i > 0 && input[i] == SPACE && input[i-1] != SPACE)) {
		    //increment num_words when a space char follows a non-space char
		    if (input[i] == SPACE) num_words++;
		    //add the chars as normal
		     temp[input_size] = input[i];
		    input_size++;
		}
	    }
	    //free the original input and set input to the cleaned string
	    char* del = input;
	    free(del);	   
	    input = temp;
	}
    }
    else {	
	input = (char*)malloc(sizeof(char) * i_line_size); 
	buf_size = i_line_size;
    }
    //now let's get the rest of the chars one by one
    //should work seamlessly with a growing file
    char curr[1];
    for (i = input_size;;i++) {
	//get the latest char
	if (read(0, curr, 1) <= 0 || *curr == EOF) break;
	 //if necessary, double the input buffer size
	if (i == buf_size) {
	    buf_size *= 2;
	    input = (char*)realloc(input, sizeof(char) * buf_size);
	    //check for realloc failure
	    if (input == NULL) {
		fprintf(stderr, "Could not reallocate memory.");
		exit(1);
	    }
	}
	//check for spaces
	if (*curr == SPACE) {
	    //check if the current char in the input buffer is a space
	    if (input_size > 0 && input[input_size-1] == SPACE) continue;
	    //if it's not, append the space normally and increment the word count
	    else if (input_size != 0) {
		input[input_size++] = SPACE;
		num_words++;
	    }
	}
	//otherwise append the char normally
	else {
	    input[input_size++] = *curr;
	}
    }
    //ensure that the last char is a space
    if (input[input_size] != SPACE) {
	num_words++;
	input[input_size++] = SPACE;
    }
    /* now that we have a uniformly space-separated input buffer, we
       can split the words into an array */
    //allocate a c-string arr with num_words pointers
    char** word_arr = (char**)malloc(sizeof(char*) * (num_words + 1));
    int end, beg, word_count;
    //loop through input buffer
    for (end = 0, beg = 0, word_count = 0;end<input_size;end++) {
	//if you've found a space, you've found a new word
	if (input[end] == SPACE) {
	    //allocate enough space for the word
	    word_arr[word_count] = (char*)malloc(sizeof(char) * ((end - beg)+1));
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
    //it's sortin' time - remember to check ignore_case
    if (ignore_case) qsort(word_arr, word_count, sizeof(char*), frobucmp_wrapper);
    else qsort(word_arr, word_count, sizeof(char*), frobcmp_wrapper);
    //now let's output the results
    int j;
    for (j = 0;j<word_count;j++) {
	//temp stores a word
	char* temp = word_arr[j];
	//loop through word chars
	while(true) {
	    //use write to write to stdout
	    if (write(1, temp, 1) <= 0) {
		fprintf(stderr, "Error while writing to stdout");
		exit(1);
	    }
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
int frobcmp(char const* a, char const* b, bool ignore_case) {
    char atemp, btemp, a_dec, b_dec;
    int count = 0;
    //memfrob does an XOR on each byte w/ the number 42
    while (true) {
	atemp = a[count];
	btemp = b[count];
	a_dec = atemp^42;
	b_dec = btemp^42;
	if (ignore_case) {
	    if (islower(a_dec)) a_dec = toupper(a_dec);
	    if (islower(b_dec)) b_dec = toupper(b_dec);
	}				  
	//check if both are space bytes
	if (atemp == SPACE && btemp == SPACE) return 0;
	//check if a < b or a is a space byte and b isn't
	if ((a_dec) < (b_dec) || (atemp == SPACE && btemp != SPACE)) return -1;
	//check if a > b or b is a space byte and a isn't
	if ((a_dec) > (b_dec) || (btemp == SPACE && atemp != SPACE)) return 1;
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
    return frobcmp(amod, bmod, false);
}

/*
This is another wrapper function for frobcmp, but this one
marks the ignore_case flag as true for the -f feature
*/
int frobucmp_wrapper(const void* a, const void* b) {
    char const* amod = *(char const**)a;
    char const* bmod = *(char const**)b;
    return frobcmp(amod, bmod, true);
}
	    
