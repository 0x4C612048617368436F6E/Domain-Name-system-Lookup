/*
================================
Background Research

Should Know what a DNS is and how it works

1.) Get the Domain-Name

2.) Ensure that the Domain is in the right format - Can use regex
For example if provided input is: 'https://www.whois.com/' remove the 'https://www.' if its:'https://www.whois.com/whois/whois.com' remove the 'https://www.' and '/whois/whois.com'

2.) Domain name will be from command line argument

3.) Check domain is in right format using regex

4.) We will send a DNS query to user Internet Service Provider’s (ISP) recursive DNS server

*/
#include<stdio.h>
#include <stdlib.h>
#include<regex.h>

#define NUMBEROFINPUTS 2
//Error values
#define INCORRECTNUMBEROFINPUTS 1
#define BUFFERSIZEERROR 2
#define FILEERROR 3
#define MALLOCERROR 4
#define REALLOCERROR 5

#define BUFFERSIZE 1024
typedef void (*Callback)(int);
typedef void (*_Callback)();

typedef struct ERRORMSG{
    char message[BUFFERSIZE];
    int errorCode;
} CUSTOMERROR;

char* fileLocation = "/etc/resolv.conf";

void bufferSizeTooSmall(int wrongBufferSize);

void incorrectNumberOfInput(int NumOfInputs);

void fileError();

void UnableToAllocateMemoryUsingMalloc();

void UnableToAllocateMoreMemoryUsingRealloc();

void treatInput(int numOfInputs, char* input,Callback errorCallback);

int isDomainNameFormatValidRegex(char* input);

void sendDNSQueryToUserISPRecursiveDNSServer(void);

void readFile(char* fileName,_Callback errorCallback,_Callback errorCallbackMalloc, _Callback errorCallbackRealloc);

void fileError(){
    CUSTOMERROR customError;
    size_t n = snprintf(customError.message,sizeof(customError.message),"Unable to open File");

    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = BUFFERSIZEERROR;
        printf("Error Type: File Error\n");
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not fo throug, still exit
    exit(EXIT_FAILURE);
}

void UnableToAllocateMoreMemoryUsingRealloc(){
    CUSTOMERROR customError;
    size_t n = snprintf(customError.message,sizeof(customError.message),"Unable to Reallocate Buffer");

    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = BUFFERSIZEERROR;
        printf("Error Type: Malloc Error\n");
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not fo throug, still exit
    exit(EXIT_FAILURE);
}

void UnableToAllocateMemoryUsingMalloc(){
    CUSTOMERROR customError;
    size_t n = snprintf(customError.message,sizeof(customError.message),"Unable to Allocate Buffer");

    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = BUFFERSIZEERROR;
        printf("Error Type: Malloc Error\n");
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not fo throug, still exit
    exit(EXIT_FAILURE);
}

void readFile(char* fileName,_Callback errorCallback,_Callback errorCallbackMalloc, _Callback errorCallbackRealloc){
    //since we are using linux, we will focus on making this just for linux

    /*
    We have to read a specific file called '/etc/resolv/conf' -> should store a nameserver whihc is usually the user ISP recursive server
    */
    FILE* file;
    size_t storageSize = BUFFERSIZE;
    char *storageBuffer = (char*)malloc(sizeof(char)*storageSize);
    char tempStore;
    size_t pointer = 0;

    if(!storageBuffer){
        //callback error
        errorCallbackMalloc();
    }

    file = fopen(fileName,"r");
    if(!file){
        //callback
        fclose(file);
        errorCallback();
    }
    //now what we do is read the file, and then find a way to get the value from 'nameserver'

    /*
    The approach we will use here is to get each character from the File, have a storage to store that (malloc that storage) if the number of size goes above that storage we increase the storage using realloc
    */
    while((tempStore = fgetc(file))!=EOF){
        if(pointer >= storageSize){
            //need to realloc
            storageSize+=BUFFERSIZE;
            if(!(realloc(storageBuffer,storageSize))){
                //callback realloc error
                errorCallbackMalloc();
            }
            *(storageBuffer+pointer) = tempStore;
            continue;
        }
        *(storageBuffer+pointer) = tempStore;
        pointer++;
    }

    //terminate the string
    *(storageBuffer+pointer) = '\0';
    printf("%s",storageBuffer);

    //Find a way to search the string and get postion of where:
    /*
    nameserver : 10.45.5.6
    get the starting position and ending position
    */
    int pos1 = 0;
    int pos2 = 0;
    int idx = 0;
    char NS[10] = "namespace"; 
    for(size_t i = 0; i<pointer; i++){
        //check if substring 'namespace' exist
        if((*(storageBuffer+i)) == (*(NS+idx))){
            //Find solutions for this...
        }
    }
    fclose(file);
}

void sendDNSQueryToUserISPRecursiveDNSServer(void){
    //get ISP recursive DNS server and send DNS query to it
}

int isDomainNameFormatValidRegex(char* input){
    regex_t regex;
    int compilationStatus;
    //compiler code
    //"^\\(https\\?:\\/\\/\\)\\?\\(www\\.\\)"
    compilationStatus = regcomp(&regex,"^\\(https\\?:\\/\\/\\)\\?\\(www\\.\\)\\?",0);
    //try and find fix for above regex
    if(compilationStatus == 0){
        printf("Regex compiled successfully");
    }else{
        printf("compilation failed");
    }

    //execution code
    compilationStatus = regexec(&regex,input,0,NULL,0);
    if(compilationStatus == 0){
        printf("\nMatch Found\n");
        return 0;
    }else if(compilationStatus == REG_NOMATCH){
        printf("\n%d\n",compilationStatus);
        printf("\nRegex not be match\n");
    }else{
        printf("An error occured\n");
    }

    //free regex
    regfree(&regex);
    return 1;
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
    printf("\n%s\n",input);
    int success = isDomainNameFormatValidRegex(input);
    if(success == 0){
        //throw an error
        printf("\nMatched");
    }else{
        printf("An Error occured\n");
    }
}

int main(int argc, char** args){
    //printf("%d",__STDCVERSION__);
    //treatInput(argc,args[1],incorrectNumberOfInput);
    //printf("%s",FILELOCATION);
    readFile(fileLocation,fileError,UnableToAllocateMemoryUsingMalloc,UnableToAllocateMoreMemoryUsingRealloc);
    return 0;
}
