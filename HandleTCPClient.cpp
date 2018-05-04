#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <assert.h>
#include "WebServer.hpp"
#include "WebFramer.hpp"
#include "WebParser.hpp"
#include <queue>
#include <string.h>
#define RCVBUFSIZE 512  /* Size of receive buffer */
#define SNDBUFSIZE 512  /* Size of receive buffer */
using namespace std;

void DieWithError(char *errorMessage);  /* Error handling function */
void HandleTCPClient(int clntSocket, string doc_root)
{
    uint8_t readBuffer[RCVBUFSIZE];
    ssize_t recvMsgSize;/* Size of received message */
    string response = "";

    WebFramer framer;
    WebParser parser;
    HTTPState message; // keeps track of full request headers + response headers
    message.doc_root = doc_root;
    bool done = false;

    queue<HTTPState> completeStateVector; // appends all completed requests per client-server interaction
   
    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, readBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0) /* zero indicates end of transmission */
    {
        string x = "";
        for (int i = 0; i < recvMsgSize; i++)
            x += readBuffer[i];

        framer.append(x);

        while (framer.hasMessage())
        {
            parser.parse(framer.topMessage(), &message); // this will parse the top message

            if (message.respond == true)
            {
                completeStateVector.push(message); // push a complete message
                message = HTTPState(); //this resets the HTTPInstruction for next request
                message.doc_root = doc_root;
            }

            framer.popMessage();
        } //end of framer check

        while (!completeStateVector.empty())
        {
            auto state = completeStateVector.front();
            completeStateVector.pop();
            //do a quick check for host header
            if (state.host.length() == 0)
                state.response = "400 Client Error";
            string response = "";
            if (state.response == "200 OK")
            {
                string data = "";
                ifstream ifs(state.url, ios::in | ios::binary);
                if (ifs.is_open())
                {
                    char c = ifs.get();
                    while (ifs.good())
                    {
                        data += c;
                        c = ifs.get();
                    }
                }

                response =
                "HTTP/1.1 " + state.response + "\r\n" + // initial response line
                "Server: Pokemon\r\n" +
                "Last-Modified: " + state.lastModified + "\r\n" +
                "Content-Type: " + state.type + "\r\n" +
                "Content-Length: " + to_string(state.size) + "\r\n\r\n" + data;

            } // end of the 200 request !!!!!!!!!

            else // 400, 403, 404 error
            {
                response =
                "HTTP/1.1 " + state.response + "\r\n" +  // initial response line
                "Server: Pokemon\r\n\r\n";
                state.close = true;
            }

            uint8_t sendBuffer [response.length()];
            for (int i = 0; i < response.length(); i++)
                sendBuffer[i] = response.at(i);
//            fails for large
            if (send(clntSocket, &sendBuffer , sizeof(sendBuffer) , 0) != sizeof(sendBuffer))
                DieWithError("send() failed");
            
            if (state.close) // if the Connection: close is true, then close the connection
            {
                done = true;
                break;
            }
        } // end of while loop

        if (done)
            break;

        if ((recvMsgSize = recv(clntSocket, readBuffer, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
    }

    close(clntSocket);    /* Close client socket */
}
