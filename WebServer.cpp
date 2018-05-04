#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <limits.h>
#include "WebServer.hpp"
#define MAXPENDING 5    /* Maximum outstanding connection requests */

void DieWithError(const char *errorMessage);  /* Error handling function */

void usage(char * argv0)
{
    cerr << "Usage: " << argv0 << " listen_port docroot_dir" << endl;
}

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    long int echoServPort;           /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }
    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    if (errno == EINVAL || errno == ERANGE) {
        usage(argv[0]);
        return 2;
    }

    if (echoServPort <= 0 || echoServPort > USHRT_MAX) {
        cerr << "Invalid port: " << echoServPort << endl;
        return 3;
    }
    string doc_root = argv[2]; /* Second arg: local port */

    start_httpd(echoServPort, doc_root);

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (::bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    while(true) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        thread (HandleTCPClient, clntSock, doc_root).detach();
    }
}
