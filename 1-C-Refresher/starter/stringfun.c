#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here

int setup_buff(char *buff, char *user_str, int len) {
    //TODO: #4:  Implement the setup buff as per the directions
    if (!buff || !user_str) 
        return -2;
    int i = 0, j = 0;
    int space_flag = 0;
    while (user_str[i] != '\0') {
        while (user_str[i] == ' ' || user_str[i] == '\t') 
            i++;
        while (user_str[i] != '\0' && user_str[i] != ' ' && user_str[i] != '\t') {
            if (j >= len) 
                return -1;
            buff[j++] = user_str[i++];
            space_flag = 1;
        }
        if (user_str[i] != '\0' && space_flag) {
            if (j >= len) 
                return -1;
            buff[j++] = ' ';
            space_flag = 0;
        }
    }
    if (j > 0 && buff[j - 1] == ' ') 
        j--;
    while (j < len) 
        buff[j++] = '.';
    return j;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    printf("]");
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int len, int str_len) {
    //YOU MUST IMPLEMENT
    if (!buff || str_len <= 0) 
        return 0;
    int count = 0, in_word = 0;
    for (int i = 0; i < str_len; i++) {
        if (buff[i] != ' ' && buff[i] != '.') {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }
    return count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
void reverse_string(char *buff, int str_len) {
    if (!buff || str_len <= 0) 
        return;
    int actual_len = 0;
    for (int i = 0; i < str_len; i++) {
        if (buff[i] == '.') 
            break;
        actual_len++;
    }
    char *start = buff;
    char *end = buff + actual_len - 1;
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void print_words(char *buff, int str_len) {
    if (!buff || str_len <= 0) 
        return;
    int word_index = 1, char_count = 0, in_word = 0;
    printf("Word Print\n");
    printf("----------\n");
    for (int i = 0; i < str_len; i++) {
        if (buff[i] != ' ' && buff[i] != '.') {
            if (!in_word) {
                printf("%d. ", word_index++);
                in_word = 1;
            }
            putchar(buff[i]);
            char_count++;
        } else if (in_word) {
            printf("(%d)\n", char_count);
            char_count = 0;
            in_word = 0;
        }
    }
    if (in_word) 
        printf("(%d)\n", char_count);
    printf("\nNumber of words returned: %d\n", word_index - 1);
}

int replace_word(char *buff, int str_len, char *target, char *replacement) {
    if (!buff || !target || !replacement || str_len <= 0) {
        return -1;
    }
    char temp_buff[BUFFER_SZ];
    char *src = buff, *dest = temp_buff;
    char *found = NULL;
    int copied = 0;
    int target_len = 0;
    int replacement_len = 0;
    for (char *p = target; *p != '\0'; p++) {
        target_len++;
    }
    for (char *p = replacement; *p != '\0'; p++) {
        replacement_len++;
    }
    while (*src != '.' && copied < str_len) {
        char *t = target;
        char *s = src;
        while (*t != '\0' && *t == *s) {
            t++;
            s++;
        }
        if (*t == '\0') {
            found = src;
            break;
        }
        if (copied < BUFFER_SZ) {
            *dest++ = *src;
            copied++;
        }
        src++;
    }
    if (!found) {
        return -2;
    }
    if (copied + replacement_len >= BUFFER_SZ) {
        return -3;
    }
    for (int i = 0; i < replacement_len && copied < BUFFER_SZ; i++) {
        *dest++ = replacement[i];
        copied++;
    }
    src += target_len;
    while (*src != '.' && copied < BUFFER_SZ && (src - buff) < str_len) {
        *dest++ = *src++;
        copied++;
    }
    while (copied < BUFFER_SZ) {
        *dest++ = '.';
        copied++;
    }
    for (int i = 0; i < BUFFER_SZ; i++) {
        buff[i] = temp_buff[i];
    }
    return 0;
}


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // Ensures argv[1] exists and starts with '-', preventing out of bounds access or invalid input
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // Ensures a string argument is provided to prevent errors and display usage if missing
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff, user_str_len);
            break;
        case 'w':
            print_words(buff, user_str_len);
            break;
        case 'x':
            if (argc != 5) {
                printf("Error: Missing arguments for -x option\n");
                exit(1);
            }
            rc = replace_word(buff, BUFFER_SZ, argv[3], argv[4]);
            if (rc == -1) {
                printf("Error: Invalid input\n");
                exit(1);
            }
            if (rc == -2) {
                printf("Error: Word not found\n");
                exit(1);
            }
            if (rc == -3) {
                printf("Error: Replacement too long\n");
                exit(1);
            }
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
// Passing both pointer and length ensures functions remain flexible, prevent overflows, and allow reuse with different buffer sizes
