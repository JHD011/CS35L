#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//convenience functions
bool has_duplicate_char(char* str, int str_len);
char swap_char(char* from, char* to, int str_len, char ch);

int main(int argc, char** argv) {
    //check that there are 3 args (name of executable is one)
    if (argc != 3) {
	printf("tr2b takes exactly 2 arguments");
	exit(1);
    }
    //set to and from
    char* from = argv[1];
    char* to = argv[2];
    //set lengths to avoid recalculating it
    int from_len = strlen(from);
    int to_len = strlen(to);
    //check that their lengths match and that both are non-zero
    if (from_len == 0 || from_len != to_len) {
	printf("Both arguments must have the same non-zero length");
	exit(1);
    }
    //check for duplicate chars
    if (has_duplicate_char(from, from_len)) {
	printf("The first argument must not have any duplicate chars");
	exit(1);
    }
    //now actually get and replace the bytes
    char curr;
    for (curr = getchar();curr != EOF;curr = getchar()) {
	if (feof(stdin)) break;
	putchar(swap_char(from, to, from_len, curr));
    }
	
}

//checks for duplicate chars in a string
bool has_duplicate_char(char* str, int str_len) {
    bool res = false;
    int rec_len = 0;
    char* rec = (char*)malloc(sizeof(char) * str_len);
    int i;
    for (i = 0;i<str_len;i++) {
	if (strchr(rec, str[i]) == NULL) {
	    rec[rec_len] = str[i];
	    rec_len++;
	}
	else {
	    res = true;
	    break;
	}
    }
    free(rec);
    return res;
}

//checks from and to and swaps char as appropriate
char swap_char(char* from, char* to, int str_len, char ch) {
    int i;
    for (i = 0;i<str_len;i++) {
	if (from[i] == ch) return to[i];
    }
    return ch;
}
