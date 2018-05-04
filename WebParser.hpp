#ifndef WEBPARSER_HPP
#define WEBPARSER_HPP
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

typedef struct HTTPState
{
    string doc_root;
    string method;
    string url;
    string clientHTMLVersion;
    string host;
    string response;
    string content;
    string type;
    string lastModified;
    long long size;
    bool respond;
    bool close;
    
} HTTPState;

class WebParser {
public:
    static void parse(string insstr, HTTPState* state);
private:
    static vector<string> splitter(string str, char delimiter);
    static string verifyURL(string url, HTTPState* state);
    static int fileExistence(const char *fileName);
    static long long fileSize(const char* file );
    static string getFileCreationTime(const char* path);
};

#endif // CALCPARSER_HPP
