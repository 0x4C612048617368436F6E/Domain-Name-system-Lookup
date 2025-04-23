/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// DNS header structure
struct DNS_HEADER {
    unsigned short id;       // ID
    unsigned char rd :1;     // Recursion Desired
    unsigned char tc :1;     // Truncated Message
    unsigned char aa :1;     // Authoritative Answer
    unsigned char opcode :4; // Purpose of message
    unsigned char qr :1;     // Query/Response Flag

    unsigned char rcode :4;  // Response Code
    unsigned char cd :1;     // Checking Disabled
    unsigned char ad :1;     // Authenticated Data
    unsigned char z :1;      // Reserved
    unsigned char ra :1;     // Recursion Available

    unsigned short q_count;  // Number of question entries
    unsigned short ans_count;
    unsigned short auth_count;
    unsigned short add_count;
};

// DNS question section
struct QUESTION {
    unsigned short qtype;
    unsigned short qclass;
};

// Convert domain name to DNS format (e.g., www.google.com → 3www6google3com0)
void convertDomainToDNSFormat(unsigned char *dns, const char *host) {
    int lock = 0, i;
    strcat((char *)host, "."); // make sure it ends with a dot

    for (i = 0; i < strlen(host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++; // skip the dot
        }
    }
    *dns++ = '\0';
}

int main() {
    unsigned char buffer[65536];
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    char domain[100];
    printf("Enter domain: ");
    scanf("%s", domain);

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53); // DNS port
    dest.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google DNS

    // Set up DNS header
    dns = (struct DNS_HEADER *)&buffer;
    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0;      // query
    dns->opcode = 0;  // standard
    dns->aa = 0; 
    dns->tc = 0;
    dns->rd = 1;      // recursion desired
    dns->ra = 0;
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); // one question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    // Query name section
    unsigned char *qname = (unsigned char*)&buffer[sizeof(struct DNS_HEADER)];
    convertDomainToDNSFormat(qname, domain);


    // Question structure
    qinfo = (struct QUESTION*)&buffer[sizeof(struct DNS_HEADER) + strlen((const char*)qname) + 1];
    qinfo->qtype = htons(1);  // A record
    qinfo->qclass = htons(1); // IN

    int queryLen = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);

    // Send the DNS query
    if (sendto(sock, buffer, queryLen, 0, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
        perror("Sendto failed");
        return 1;
    }

    // Receive the response
    int i = sizeof dest;
    int responseLen = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&dest, (socklen_t*)&i);
    if (responseLen < 0) {
        perror("Recvfrom failed");
        return 1;
    }

    printf("\nReceived %d bytes\n", responseLen);
    printf("Raw response (hex):\n");
    for (int i = 0; i < responseLen; i++) {
        printf("%02x ", buffer[i]);
        if ((i+1) % 16 == 0) printf("\n");
    }
    printf("\n");

    close(sock);
    return 0;
}
*/