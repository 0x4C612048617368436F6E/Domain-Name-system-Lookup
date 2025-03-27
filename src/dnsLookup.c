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

5.) Will implment the DNS query from scratch - First create the DNS request structure

*/
#include<stdio.h>
#include<stdlib.h>
#include<regex.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>

#define NUMBEROFINPUTS 2
//Error values
#define INCORRECTNUMBEROFINPUTS 1
#define BUFFERSIZEERROR 2
#define FILEERROR 3
#define MALLOCERROR 4
#define REALLOCERROR 5
#define REGEXCOMPILATIONERROR 6

#define BUFFERSIZE 1024
#define MAXLENGTHOFNAMESERVERADDRESS 27
//above can be:
/*
nameserver 255.255.255.255

maximum length -> 27 (including null terminating character)
*/
typedef void (*Callback)(int);
typedef void (*_Callback)();

typedef struct ERRORMSG{
    char message[BUFFERSIZE];
    int errorCode;
} CUSTOMERROR;

/*
DNS format
 -----------------
|    Header       |
 -----------------
|    Question     |
 -----------------
|    Answer       |
 -----------------
|    Authority    |
 -----------------
|    Additional   |
 -----------------

*/

//create DNS header format
/*
ID - 16 bits -> 2 byte
QR - 1 bit -> use bit field
OPCODE - 4 bit
*/

typedef struct {
    unsigned char QR: 1,
    unsigned short OPCODE: 4,
    unsigned char AA: 1,
    unsigned char TC,
    unsigned char RD: 1,
    unsigned char RA,
    unsigned short Z,
    unsigned short RCODE:4,
}DNS_HEADER_FLAG;

typedef struct{
    unsigned short int ID,
    unsigned short int QDCOUNT,
    unsigned short int ANCOUNT,
    DNS_HEADER_FLAG Flag;
    unsigned short int NSCOUNT,
    unsigned short int ARCOUNT

}DNS_HEADER;

//configure query
typedef struct{
    char Domain[1024];
    unsigned short int QTYPE;
    unsigned short int CLASS; 

}DNS_QUERY_QUESTION;

//configure resource record

char* fileLocation = "/etc/resolv.conf";

void bufferSizeTooSmall(int wrongBufferSize);

void incorrectNumberOfInput(int NumOfInputs);

void fileError();

void UnableToAllocateMemoryUsingMalloc();

void UnableToAllocateMoreMemoryUsingRealloc();

void regexCompilationError();

void treatInput(int numOfInputs, char* input,Callback errorCallback);

int isDomainNameFormatValidRegex(char* input,_Callback callback);

void sendDNSQueryToUserISPRecursiveDNSServer(char* domain);

char* readFileAndReturnRecursiveAddress(char* fileName,_Callback errorCallback,_Callback errorCallbackMalloc, _Callback errorCallbackRealloc);

char* subStringExtractorAndTrim(char* actualString, size_t initialPos,_Callback errorCallbackMalloc, _Callback errorCallbackRealloc);

void fileError(){
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),"Unable to open File");

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
    int n = snprintf(customError.message,sizeof(customError.message),"Unable to Reallocate Buffer");

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
    int n = snprintf(customError.message,sizeof(customError.message),"Unable to Allocate Buffer");

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

char* readFileAndReturnRecursiveAddress(char* fileName,_Callback errorCallback,_Callback errorCallbackMalloc, _Callback errorCallbackRealloc){
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
                errorCallbackRealloc();
            }
            *(storageBuffer+pointer) = tempStore;
        }else{
            *(storageBuffer+pointer) = tempStore;
        }
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
    char *NS = "nameserver";

    int isSatisfied = 0;
    int isFound = 0;

    for(size_t i = 0; i<pointer; i++){
        //check if substring 'namespace' exist
        if((*(storageBuffer+i)) == (*(NS+idx))){

            if(isFound){
                printf("\n================\n");
            }

            isFound = 0;
            printf("%c - %ld\n",*(storageBuffer+i),i);
            if((pos2-pos1) == 9){
                printf("\nGot the line\n");
                isSatisfied = 1;
            }
            //Find solutions for this...
            idx++;
            pos2++;
        }else{
            isFound = 1;
            if(pos2 > pos1){
                pos1 = pos2;
            }
            idx = 0;
            pos1++;
            pos2++;
        }

        //extract the remaining string, then return it 

        if(isSatisfied){
            size_t storageBuf = MAXLENGTHOFNAMESERVERADDRESS;

            char *res = (char*)malloc(sizeof(char)*storageBuf);

            if(!res){
                //callback error
                errorCallbackMalloc();
            }

            size_t position = 0;
            for(size_t i=pos1;*(storageBuffer+i)!='\0';i++){
                if(position >= storageBuf){
                    //reallocate space
                    storageBuf+=MAXLENGTHOFNAMESERVERADDRESS;
                    if(!(realloc(res,storageBuf))){
                        //callback realloc error
                        errorCallbackRealloc();
                    }
                    *(res+position) = *(storageBuffer+i);
                }else{
                    *(res+position) = *(storageBuffer+i);
                }
                    position++;
            }
            *(res+position) = '\0';
            fclose(file);
            return res;
        }
    }
    fclose(file);
    return "";
}

char* subStringExtractorAndTrim(char* actualString, size_t initialPos, _Callback errorCallbackMalloc, _Callback errorCallbackRealloc){
    size_t actualInitialPos = initialPos;
    while(*(actualString+actualInitialPos) == ' '){
        actualInitialPos++;
    }

    size_t storageBuf = MAXLENGTHOFNAMESERVERADDRESS;
    size_t pointer = 0;
    char *returnedSubString = (char*)malloc(sizeof(char)*storageBuf);

    if(!returnedSubString){
        //callback error
        errorCallbackMalloc();
    }

    int length = 0;
    for(int i=0;*(actualString+i)!='\0';i++){
        length++;
    }

    for(int i=actualInitialPos;i<length;i++){
        if(pointer>=storageBuf){
            storageBuf+=MAXLENGTHOFNAMESERVERADDRESS;
            if(!(realloc(returnedSubString,storageBuf))){
                //callback realloc error
                errorCallbackRealloc();
            }

            *(returnedSubString+pointer) = *(actualString+i);
        }else{
            *(returnedSubString+pointer) = *(actualString+i);
        }
        pointer++;
    }
    *(returnedSubString+pointer) = '\0';
    printf("\n%s\n",returnedSubString);
}

void sendDNSQueryToUserISPRecursiveDNSServer(char* domain){
    //get ISP recursive DNS server and send DNS query to it
    char* ISPRecursiveAddress = subStringExtractorAndTrim(readFileAndReturnRecursiveAddress(fileLocation,fileError,UnableToAllocateMemoryUsingMalloc,UnableToAllocateMoreMemoryUsingRealloc),10,UnableToAllocateMemoryUsingMalloc,UnableToAllocateMoreMemoryUsingRealloc);

    struct addrinfo hints, *res;
    int status;
    char ipstr[INET_ADDRSTRLEN];
    //HERE

}

//adapt this for regexError
void regexCompilationError(){
    //callback function for regex error
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),"Regex Compilcation Error");

    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = REGEXCOMPILATIONERROR;
        printf("Error Type: Regex Error\n");
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not fo throug, still exit
    exit(EXIT_FAILURE);
}

int isDomainNameFormatValidRegex(char* input,_Callback callback){
    regex_t regex;
    int compilationStatus;
    //compiler code
    //"^\\(https\\?:\\/\\/\\)\\?\\(www\\.\\)"
    compilationStatus = regcomp(&regex,"^\\(https\\?:\\/\\/\\)\\?\\(www\\.\\)\\?",0);
    //try and find fix for above regex
    if(compilationStatus == 0){
        printf("Regex compiled successfully");
    }else{
        //printf("compilation failed");
        callback();
    }

    //execution code
    compilationStatus = regexec(&regex,input,0,NULL,0);
    if(compilationStatus == 0){
        printf("\nMatch Found\n");
        return 0;
    }else if(compilationStatus == REG_NOMATCH){
        //printf("\n%d\n",compilationStatus);
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
    int n = snprintf(customError.message,sizeof(customError.message),"Buffer Size Error. Expected 0 or 1024. Got: %d",wrongBufferSize);
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
    int n = snprintf(customError.message,sizeof(customError.message),"Number of Inputs must be 2, but you entered: %d",NumOfInputs);
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
    int success = isDomainNameFormatValidRegex(input,regexCompilationError);
    if(success == 0){
        //throw an error
        printf("\nMatched");
    }else{
        printf("An Error occured\n");
    }

    //call the DNS query mechanism here
    sendDNSQueryToUserISPRecursiveDNSServer(input);
}

int main(int argc, char** args){
    treatInput(argc,args[1],incorrectNumberOfInput);
    return 0;
}
