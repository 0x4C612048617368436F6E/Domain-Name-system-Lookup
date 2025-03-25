/*
================================
Background Research

Should Know what a DNS is and how it works

1.) Get the Domain-Name

2.) Ensure that the Domain is in the right format - Can use regex
For example if provided input is: 'https://www.whois.com/' remove the 'https://www.' if its:'https://www.whois.com/whois/whois.com' remove the 'https://www.' and '/whois/whois.com'

2.) Domain name will be from command line input, 

*/
#include<stdio.h>
#include <stdlib.h>
#include<regex.h>

#define NUMBEROFINPUTS 2
//Error values
#define INCORRECTNUMBEROFINPUTS 1
#define BUFFERSIZEERROR 2
#define BUFFERSIZE 1024
typedef void (*Callback)(int);

typedef struct ERRORMSG{
    char message[BUFFERSIZE];
    int errorCode;
} CUSTOMERROR;

void bufferSizeTooSmall(int wrongBufferSize);

void incorrectNumberOfInput(int NumOfInputs);

void treatInput(int numOfInputs, char* input,Callback errorCallback);

int isDomainNameFormatValidRegex(char* input){
    regex_t regex;
    static int compilationStatus;
    //compiler code
    compilationStatus = regcomp(&regex,"[www{3}|WWW{3}].[a-zA-Z].com",0);
    if(compilationStatus == 0){
        printf("Regex compiled successfully");
    }else{
        printf("compilation failed");
    }

    //execution code
    compilationStatus = regexec(&regex,input,0,NULL,0);
    if(compilationStatus == 0){
        printf("\nMatch Found\n");
    }else if(compilationStatus == REG_NOMATCH){
        printf("%d\n",compilationStatus);
        printf("\nRegex not be match\n");
    }else{
        printf("An error occured\n");
    }

    //free regex
    regfree(&regex);
    return compilationStatus;
}


void bufferSizeTooSmall(int wrongBufferSize){
    CUSTOMERROR customError;
    size_t n = snprintf(customError.message,sizeof(customError.message),"Buffer Size Error. Expected 0 or 1024. Got: %d",wrongBufferSize);
    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = BUFFERSIZEERROR;
        printf("Error Type: Buffer Size too small\n");
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not fo throug, still exit
    exit(EXIT_FAILURE);
}

void incorrectNumberOfInput(int NumOfInputs){
    CUSTOMERROR customError;
    size_t n = snprintf(customError.message,sizeof(customError.message),"Number of Inputs must be 2, but you entered: %d",NumOfInputs);
    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = INCORRECTNUMBEROFINPUTS;
        printf("Error Type: Incorrect Number of Input\n");
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }else{
        //another custom Error
        //errorCallback(n,errorMessage,bufferSizeTooSmall);
        bufferSizeTooSmall(n);
    }
}

void treatInput(int numOfInputs, char* input,Callback errorCallback){
    if(numOfInputs < NUMBEROFINPUTS || numOfInputs > NUMBEROFINPUTS){
        errorCallback(numOfInputs);
    }
    //Make sure input is of correct format before sending to regex function

    int success = isDomainNameFormatValidRegex(input);
    if(success == 0){
        //throw an error
        printf("\nMatched");
    }else{
        printf("An Error occured");
    }
}

int main(int argc, char** args){
    //printf("%d",__STDCVERSION__);
    treatInput(argc,args[1],incorrectNumberOfInput);
    return 0;
}