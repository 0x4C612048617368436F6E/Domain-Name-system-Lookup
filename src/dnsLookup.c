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
#if defined(WIN_32)
//Implement WINDOWS version after

#elif defined(__linux__)


#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<regex.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
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

#define MAXOFUDPPACKET 64000
//UDP can hold a MAX of 64KB which equates to 6400 bytes

/*
1 byte = 0.001 KB
*/

typedef void (*Callback2Input)(char*,char*);
typedef void (*callback3Input)(char*,char*,int);
typedef void (*_callback3Input)(char*,char*,int);

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
    unsigned char QR: 1;
    unsigned short OPCODE: 4;
    unsigned char AA: 1;
    unsigned char TC;
    unsigned char RD: 1;
    unsigned char RA;
    unsigned short Z;
    unsigned short RCODE:4;
}DNS_HEADER_FLAG;

typedef struct{
    unsigned short int ID;
    unsigned short int QDCOUNT;
    unsigned short int ANCOUNT;
    DNS_HEADER_FLAG Flag;
    unsigned short int NSCOUNT;
    unsigned short int ARCOUNT;
}DNS_HEADER;

//configure query

typedef struct{
    unsigned char Domain[BUFFERSIZE];
}DNS_DOMAIN;

typedef struct{
    //DNS_DOMAIN DOMAIN;
    unsigned short int QTYP;
    unsigned short int CLASS; 
}DNS_QUERY;

//configure resource record
typedef struct{
    unsigned short TYPE;
    unsigned short CLASS;
    unsigned int TTL;
    unsigned short DATA_LEN; 
}DNS_RESOURCE_DATA;

typedef struct{
    DNS_DOMAIN DOMAIN;
    DNS_RESOURCE_DATA RESOURCE;
    unsigned char RESOURCE_DATA[BUFFERSIZE]; 
}DNS_RESOURCE_RECORD;

char* fileLocation = "/etc/resolv.conf";

void Callback2CharInput(char* ErrMsg,char* ErrType);

void Callback3CharInput(char* ErrMsg,char* ErrType,int status);

void Callback3CharInputVariation(char* ErrMsg,char* ErrType,int status);

void treatInput(int numOfInputs, char* input,_callback3Input errorCallback);

int isDomainNameFormatValidRegex(char* input,Callback2Input RegexCompileError);

void generateRandom16BitNumber();

void sendDNSQueryToUserISPRecursiveDNSServer(char* domain,Callback2Input callback);

char* convertDomainIntoDNSDomainFormat(char* domain, char* query);

void DNSResponse();

size_t simpleStrlen(const char* string);

char* readFileAndReturnRecursiveAddress(char* fileName,Callback2Input errorCallback,Callback2Input errorCallbackMalloc, Callback2Input errorCallbackRealloc);

char* subStringExtractorAndTrim(char* actualString, size_t initialPos, Callback2Input errorCallbackMalloc, Callback2Input errorCallbackRealloc);

void socketInitiation(Callback2Input SocketInitiationError,Callback2Input SendToError,char* query,size_t DNS_HEADER_SIZE, size_t querySize,size_t DNS_QUESTION_SIZE,struct sockaddr_in destination);

void Callback2CharInput(char* ErrMsg,char* ErrType){
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),ErrMsg);

    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = BUFFERSIZEERROR;
        printf("Error Type: %s\n",ErrType);
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not fo throug, still exit
    exit(EXIT_FAILURE);
}

void Callback3CharInput(char* ErrMsg,char* ErrType,int status){
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

void Callback3CharInputVariation(char* ErrMsg,char* ErrType,int status){
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),ErrMsg,status);
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

char* readFileAndReturnRecursiveAddress(char* fileName,Callback2Input errorCallback,Callback2Input errorCallbackMalloc, Callback2Input errorCallbackRealloc){
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
        errorCallbackMalloc("Unable to Allocate Buffer","Malloc Error");
    }

    file = fopen(fileName,"r");
    if(!file){
        //callback
        fclose(file);
        errorCallback("Unable to open File","File Error");
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
                errorCallbackRealloc("Unable to Reallocate Buffer","Reallocation Error");
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
                errorCallbackMalloc("Unable to Allocate Buffer","Malloc Error");
            }

            size_t position = 0;
            for(size_t i=pos1;*(storageBuffer+i)!='\0';i++){
                if(position >= storageBuf){
                    //reallocate space
                    storageBuf+=MAXLENGTHOFNAMESERVERADDRESS;
                    if(!(realloc(res,storageBuf))){
                        //callback realloc error
                        errorCallbackRealloc("Unable to Reallocate Buffer","Reallocation Error");
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

char* subStringExtractorAndTrim(char* actualString, size_t initialPos, Callback2Input errorCallbackMalloc, Callback2Input errorCallbackRealloc){
    size_t actualInitialPos = initialPos;
    while(*(actualString+actualInitialPos) == ' '){
        actualInitialPos++;
    }

    size_t storageBuf = MAXLENGTHOFNAMESERVERADDRESS;
    size_t pointer = 0;
    char *returnedSubString = (char*)malloc(sizeof(char)*storageBuf);

    if(!returnedSubString){
        //callback error
        errorCallbackMalloc("Unable to Allocate Buffer","Malloc Error");
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
                errorCallbackRealloc("Unable to Reallocate Buffer","Reallocation Error");
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

void DNSResponse(){
    //response from DNS
    /*
    This will usually inlcude:
DNS format
 -----------------
|    Header       |
 -----------------
|    Question     |
 -----------------
|    Answer       | RR
 -----------------
|    Authority    | RR
 -----------------
|    Additional   | RR
 -----------------
 But What we need to extract should just be the:
 1.) Answer
 2.) Authority
 3.)Additional
    */
    DNS_RESOURCE_RECORD Answer, Authority, Additional;
}

void socketInitiation(Callback2Input SocketInitiationError,Callback2Input SendToError,char* query,size_t DNS_HEADER_SIZE, size_t querySize,size_t DNS_QUESTION_SIZE,struct sockaddr_in destination){
    //DNS usually uses UDP socket
    /*
    (See if can do later) - DNS can also use TCP socket in some cases
    */
    int SOCKETENDPOINT = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(!SOCKETENDPOINT){
        //callback
        SocketInitiationError("Unable to initiate socket connection","Socket Error");
    }
    int success = sendto(s,(char*)query,(DNS_HEADER_SIZE+querySize+DNS_QUESTION_SIZE),0,(struct sockaddr*)&destination,sizeof(destination));
    if(!success){
        //callback
        SendToError("Unable to transmit message to socket","Sendto Error");
    }
    printf("DNS query Sent");
}

size_t simpleStrlen(const char* string){
    const char* pointer = string;
    while(*pointer){
        pointer++;
    }
    return pointer - string;
}

char* convertDomainIntoDNSDomainFormat(char* domain, char* query){
    //converts the Domain into a DNS DomainFormat
    /*
    Given www.google.com -> converts into
    3www.6google.3com0
    */
   size_t lock=0, i=0;
   for(;i<simpleStrlen((const char*)domain;i++)){
        if(*(domain+i) == '.'){
            ++query = i-lock;
            while(lock<i){
                lock++;
            }
        }else{
            ++query = *(domain+i);
        }
   }
   ++query = '\0';
}

void sendDNSQueryToUserISPRecursiveDNSServer(char* domain,Callback2Input callback){
    //get ISP recursive DNS server and send DNS query to it
    char* ISPRecursiveAddress = subStringExtractorAndTrim(readFileAndReturnRecursiveAddress(fileLocation,Callback2CharInput,Callback2CharInput,Callback2CharInput),10,Callback2CharInput,Callback2CharInput);

    char DNSQUERYINBUFFER[MAXOFUDPPACKET];
    char* actualQuery;

    //Create destination structure
    struct sockaddr_in DestinationAddress;

    DestinationAddress.sin_family = AF_INET;
    DestinationAddress.sin_port = htons(53); //DNS works on port 53
    //make use of inet_aton
    DestinationAddress.sin_port = inet_aton(ISPRecursiveAddress);
    
    //create DNS Header
    DNS_HEADER *dns_header = NULL;
    //create Question/query structure
    DNS_QUERY *dns_query = NULL;
    //configure buffer
    dns_header = (DNS_HEADER*)&DNSQUERYINBUFFER;

    dns_header->ID = generateRandom16BitNumber();
    dns_header->QDCOUNT = 0;
    dns_header->ANCOUNT = 0;
    dns_header->Flag.QR = 0;

    dns_header->Flag.OPCODE = 0;
    dns_header->Flag.AA = 0;
    dns_header->Flag.TC = 0;
    dns_header->Flag.RD = 1; //recursion allowed
    dns_header->Flag.RA = 0;
    dns_header->Flag.Z = 0;
    dns_header->Flag.RCODE = 0;

    dns_header->NSCOUNT = 0;
    dns_header->ARCOUNT = 0;

    //extract the configured
    actualQuery = (unsigned char*)&DNSQUERYINBUFFER[sizeof(DNS_HEADER)];
    //Create DNS Query/Message 
    convertDomainIntoDNSDomainFormat(actualQuery,domain);
    DNS_QUERY *dns_query_message = NULL;
    dns_query_message = (DNS_QUERY*)&buf[sizeof(DNS_HEADER)+(simpleStrlen((const char*)actualQuery))];

    dns_query_message->QTYPE = htons(1);
    dns_query_message->QCLASS = htons(1);

    //start to send packet
    socketInitiation(); //initiates and sends socket

    //receive response

}

short int generateRandom16BitNumber(){
    //For each of the DNS request, a random 16-bit number will be generated

    /*
    8 bits - 1byte
    16 bits - 2 byte
    */

    //initialise random number generator
    srand(time(0));
    short int random16BitNum = rand();
    return random16BitNum;
}

int isDomainNameFormatValidRegex(char* input,Callback2Input RegexCompileError){
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
        RegexCompileError("Regex Compilation Error","Regex Error");
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

void incorrectNumberOfInput(char* ErrMsg, char* ErrType, int NumOfInputs, callback3Input errorCallback){
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),ErrMsg,NumOfInputs);
    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = INCORRECTNUMBEROFINPUTS;
        printf("Error Type: %s\n",ErrType);
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }else{
        //another custom Error
        //bufferSizeTooSmall(n);
        errorCallback("Buffer Size Error. Expected 0 or 1024. Got: %d","Buffer Size too small\n",n);
    }
}

void treatInput(int numOfInputs, char* input,_callback3Input errorCallback){
    if(numOfInputs < NUMBEROFINPUTS || numOfInputs > NUMBEROFINPUTS){
        errorCallback("Number of Inputs must be 2, but you entered: %d","Incorrect Number of Input\n",numOfInputs);
    }
    //Make sure input is of correct format before sending to regex function
    printf("\n%s\n",input);
    int success = isDomainNameFormatValidRegex(input,Callback2CharInput);
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

#endif