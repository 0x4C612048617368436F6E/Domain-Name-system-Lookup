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

6.) Implement response

*/
#if defined(WIN_32)
//Implement WINDOWS version after
#include <window>

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
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

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

#define MAXOFUDPPACKET 65536
//UDP can hold a MAX of 64KB which equates to 6400 bytes

//generate a 16 bit number
#define _16BITRANDOMNUM 65536

/*
1 byte = 0.001 KB
*/

typedef void (*Callback2Input)(char*,char*);
typedef void (*callback3Input)(char*,char*);
typedef void (*_callback3Input)(char*,char*);

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
    uint16_t ID;
    uint16_t flags;
    uint16_t QDCOUNT;
    uint16_t ANCOUNT;
    uint16_t NSCOUNT;
    uint16_t ARCOUNT;
} DNS_HEADER;

//configure query

typedef struct{
    //DNS_DOMAIN DOMAIN;
    unsigned short int QTYPE;
    unsigned short int QCLASS; 
}DNS_QUERY;

typedef struct{
    unsigned char Domain[BUFFERSIZE];
}DNS_DOMAIN;

//configure resource record
typedef struct{
    unsigned short int TYPE;
    unsigned short int CLASS;
    unsigned int TTL;
    unsigned short int DATA_LEN; 
}DNS_RESOURCE_DATA;

typedef struct{
    DNS_DOMAIN DOMAIN;
    DNS_RESOURCE_DATA RESOURCE;
    unsigned char RESOURCE_DATA[BUFFERSIZE]; 
}DNS_RESOURCE_RECORD;

char* fileLocation = "/etc/resolv.conf";

void Callback2CharInput(char* ErrMsg,char* ErrType);

void Callback3CharInput(char* ErrMsg,char* ErrType);

void Callback3CharInputVariation(char* ErrMsg,char* ErrType);

void treatInput(int numOfInputs, char* input,_callback3Input errorCallback);

int isDomainNameFormatValidRegex(char* input,Callback2Input RegexCompileError);

short int generateRandom16BitNumber();

void sendDNSQueryToUserISPRecursiveDNSServer(char* domain,Callback2Input callback);

void convertDomainIntoDNSDomainFormat(unsigned char* query, char* domain);

void DNSResponse();

size_t simpleStrlen(const unsigned char* string);

char* readFileAndReturnRecursiveAddress(char* fileName,Callback2Input errorCallback,Callback2Input errorCallbackMalloc, Callback2Input errorCallbackRealloc);

char* subStringExtractorAndTrim(char* actualString, size_t initialPos, Callback2Input errorCallbackMalloc, Callback2Input errorCallbackRealloc);

int socketInitiation(Callback2Input SocketInitiationError,Callback2Input SendToError,unsigned char* DNSQUERYINBUFFER,int dnsPacketSize,struct sockaddr_in DestinationAddress);

int receiveRespone(Callback2Input RecvError,unsigned char* DNSQUERYINBUFFER,struct sockaddr_in destination,int Socket);

void Callback2CharInput(char* ErrMsg,char* ErrType){
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),"%s\n",ErrMsg);

    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = BUFFERSIZEERROR;
        printf("Error Type: %s\n",ErrType);
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not go through, still exit
    exit(EXIT_FAILURE);
}

void Callback3CharInput(char* ErrMsg,char* ErrType){
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),"%s\n",ErrMsg);
    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = INCORRECTNUMBEROFINPUTS;
        printf("Error Type: %s\n",ErrType);
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }else{
        //another custom Error
        //bufferSizeTooSmall(n);
        exit(EXIT_FAILURE);
    }
}

void Callback3CharInputVariation(char* ErrMsg,char* ErrType){
    CUSTOMERROR customError;
    int n = snprintf(customError.message,sizeof(customError.message),"%s\n",ErrMsg);
    if(n>=0 && n <= BUFFERSIZE){
        customError.errorCode = BUFFERSIZEERROR;
        printf("Error Type: %s\n",ErrType);
        printf("Error Message: %s\n",customError.message);
        printf("Error code: %d",customError.errorCode);
        exit(EXIT_FAILURE);
    }
    //even if condition does not go throug, still exit
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

            isFound = 0;
            if((pos2-pos1) == 9){
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
    return returnedSubString;
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

int socketInitiation(Callback2Input SocketInitiationError,Callback2Input SendToError,unsigned char* DNSQUERYINBUFFER,int dnsPacketSize,struct sockaddr_in DestinationAddress){
    //DNS usually uses UDP socket
    /*
    (See if can do later) - DNS can also use TCP socket in some cases
    */
   //IPPROTO_UDP
   int SOCKETENDPOINT = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
   if(SOCKETENDPOINT < 0){
       //callback
       printf("\nUnable to initiate socket connection\n");
    }

   int success = sendto(SOCKETENDPOINT,DNSQUERYINBUFFER,dnsPacketSize,0,(struct sockaddr*)&DestinationAddress,sizeof(DestinationAddress));
   printf("\n%d\n",success);
   if(success < 0){
       //callback
       printf("\nUnable to transmit message\n");
       return -1;
       //SendToError("Unable to transmit message to socket","Sendto Error");
    }else{
       printf("Sent %d bytes\n", success);
    }
    printf("DNS query Sent: %d",SOCKETENDPOINT);

    return SOCKETENDPOINT;
}

size_t simpleStrlen(const unsigned char* string){
    const unsigned char* pointer = string;
    while(*pointer){
        pointer++;
    }
    return pointer - string;
}

void convertDomainIntoDNSDomainFormat(unsigned char* query, char *host) {
    //Given www.google.com -> converts into
    //3www6google3com0
    //add a . to then end of the host
    strcat((char*)host,".");
    int i=0, lock = 0;
    for(;i<strlen(host);i++){
        if(host[i] == '.'){
            *query++ = i-lock;
            for(;lock<i;lock++){
                *query++ = host[lock];
            }
            lock++; //skip period
        }
    }
    *query++ = '\0';
}

int receiveRespone(Callback2Input RecvError,unsigned char* DNSQUERYINBUFFER,struct sockaddr_in destination,int Socket){
    socklen_t i = sizeof(destination);

    int pending;
    ioctl(Socket, SIOCOUTQ, &pending);

    printf("Check: -> %d",pending);

    char buffer;
    int result = recv(Socket, &buffer, 1, MSG_PEEK | MSG_DONTWAIT);
    if (result == 0) {
        // Connection has been closed
            printf("\nConnection has been closed\n");
        return 0;
    } else if (result < 0) {
        if (errno == EAI_AGAIN || errno == EWOULDBLOCK) {
            // No data available, but socket is still alive
            printf("\nNo data available, but socket is alive\n");
        } else {
            // An error occurred
            printf("\nAn error occured\n");
        }
    }
    int len = recvfrom(Socket,DNSQUERYINBUFFER,MAXOFUDPPACKET,0,(struct sockaddr*)&destination,&i);
    //int len = recv(Socket,(char*)query,MAXOFUDPPACKET,MSG_PEEK | MSG_DONTWAIT);
    printf("\n Len: %d \n",len);
    if(len == -1){
        RecvError("Unable to receive data","Recv Error");
    }
    printf("\nLen:%d\n",len);
    return len;
}

void sendDNSQueryToUserISPRecursiveDNSServer(char* domain,Callback2Input callback){
    //get ISP recursive DNS server and send DNS query to it
    //char* ISPRecursiveAddress = subStringExtractorAndTrim(readFileAndReturnRecursiveAddress(fileLocation,Callback2CharInput,Callback2CharInput,Callback2CharInput),10,Callback2CharInput,Callback2CharInput);
    char* ISPRecursiveAddress = "8.8.8.8";

    unsigned char DNSQUERYINBUFFER[MAXOFUDPPACKET];
    unsigned char* actualQuery, *reader;

    //Create destination structure
    struct sockaddr_in DestinationAddress;

    //in address structure
    struct in_addr addr;

    DestinationAddress.sin_family = AF_INET;
    DestinationAddress.sin_port = htons(53); //DNS works on port 53

    DestinationAddress.sin_addr.s_addr = inet_addr(ISPRecursiveAddress); 

    
    //create DNS Header
    DNS_HEADER *dns_header = NULL;
    //create Question/query structure
    DNS_QUERY *dns_query = NULL;
    //configure buffer

    dns_header = (DNS_HEADER *)DNSQUERYINBUFFER;

    dns_header->ID = htons(getpid() & 0xFFFF);

    uint16_t flags = 0;
    flags |= (0 << 15); // QR = 0 (query)
    flags |= (0 << 11); // OPCODE = 0 (standard query)
    flags |= (0 << 10); // AA = 0
    flags |= (0 << 9);  // TC = 0
    flags |= (1 << 8);  // RD = 1 (recursion desired)
    flags |= (0 << 7);  // RA = 0
    flags |= (0 << 5);  // Z = 0
    flags |= (0 << 4);  // AD = 0
    flags |= (0 << 3);  // CD = 0
    flags |= (0);       // RCODE = 0

    dns_header->flags = htons(flags);
    dns_header->QDCOUNT = htons(1);
    dns_header->ANCOUNT = 0;
    dns_header->NSCOUNT = 0;
    dns_header->ARCOUNT = 0;

    printf("\nSizeof dns_header: %ld\n",sizeof(DNS_HEADER));

    //extract the configured
    actualQuery = (unsigned char*)&DNSQUERYINBUFFER[sizeof(DNS_HEADER)];
    //DNS_HEADER
    //Create DNS Query/Message 
    convertDomainIntoDNSDomainFormat(actualQuery,domain);

    DNS_QUERY *dns_query_message = NULL;
    //DNS_HEADER
    dns_query_message = (DNS_QUERY*)&DNSQUERYINBUFFER[sizeof(DNS_HEADER)+strlen((const char*)actualQuery)+1];

    dns_query_message->QTYPE = htons(1);
    dns_query_message->QCLASS = htons(1);

    //start to send packet

    int dnsPacketSize = sizeof(DNS_HEADER) + (strlen((const char*)actualQuery) + 1) + sizeof(DNS_QUERY);

    int sock = socketInitiation(Callback2CharInput, Callback2CharInput,DNSQUERYINBUFFER,dnsPacketSize,DestinationAddress);
    

    printf("\nReceiving Response: %d\n",socket);
    int S = receiveRespone(Callback2CharInput,(unsigned char*)DNSQUERYINBUFFER,DestinationAddress,sock);
    if(S != 0){
        printf("Received response");
    }
    printf("\nResponse size: %d\n",S);
    
    dns_header = (DNS_HEADER*)&DNSQUERYINBUFFER;
    reader = &DNSQUERYINBUFFER[sizeof(DNS_HEADER)+(simpleStrlen((const unsigned char*)actualQuery))];
    //ntohs
    printf("\nThe response contains\n");
    printf("\n%d Questions.", dns_header->QDCOUNT);
    printf("\n %d Answers.",dns_header->ANCOUNT);
	printf("\n %d Authoritative Servers.",dns_header->NSCOUNT);
	printf("\n %d Additional records.\n\n",dns_header->ARCOUNT);
    
}


short int generateRandom16BitNumber(){
    //For each of the DNS request, a random 16-bit number will be generated

    /*
    8 bits - 1byte
    16 bits - 2 byte
    */

    //initialise random number generator
    srand(time(0));
    short int random16BitNum = rand() % _16BITRANDOMNUM;
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
        errorCallback("Buffer Size Error. Expected 0 or 1024","Buffer Size too small\n");
    }
}

void treatInput(int numOfInputs, char* input,_callback3Input errorCallback){
    if(numOfInputs < NUMBEROFINPUTS || numOfInputs > NUMBEROFINPUTS){
        errorCallback("Number of Inputs must be 2","Incorrect Number of Input\n");
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
    sendDNSQueryToUserISPRecursiveDNSServer(input,Callback2CharInput);
}

int main(int argc, char** args){
    treatInput(argc,args[1],incorrectNumberOfInput);
    return 0;
}

#endif