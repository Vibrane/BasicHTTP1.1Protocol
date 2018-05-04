#include <assert.h>
#include "WebParser.hpp"

void WebParser::parse(string instruction, HTTPState* state)
{
    if (instruction == "new line")
        state->respond = true;
    else if (instruction[0] == 'G' and instruction[1] == 'E' and instruction[2] == 'T')
    {
        vector<string> split = splitter(instruction, ' '); // this should always split into three
        state->method = split.at(0);
        if (state->method == "GET") // first is properly GET
        {
            if (split.at(1) == "/") //checks if default
                state->url = state->doc_root + "/index.html";
            else
                state->url = state->doc_root + split.at(1);
            
            state->clientHTMLVersion = split.at(2);
            if (state->clientHTMLVersion == "HTTP/1.1") // make sure it is 1.1 protocol
                state->response = verifyURL(state->url, state);
            else
                state->response = "400 Client Error";
        }
        
        else
            state->response = "400 Client Error"; // it is not GET
        
    }

    else if (instruction.find(':') != string::npos)
    {
        vector<string> split = splitter(instruction, ':');
        string key = split.at(0);
        string value = split.at(1);

        if (split.size() > 2)
            for (auto i = 2; i < split.size(); i++)
            {
                value += ':'; // needed for localhost:8000
                value += split.at(i);
            }
        if (key == "Host")
            state->host = value;
        else if (key == "Connection")
            state->close = (value == " close") ? true : false;
    }
    else
        state->response = "400 Client Error"; // no colons found to seperate the key : value
}

vector<string> WebParser::splitter(string instruction, char delimiter)
{
    vector<string> splitVector;
    stringstream ss(instruction); // Turn the string into a stream.
    string token;
    while(getline(ss, token, delimiter))
        splitVector.push_back(token);
    return splitVector;
}

string WebParser::verifyURL(string url, HTTPState* state)
{
    //if url is only / -> write code to figure out what to do  in that scenario
    if (url.length() == 1)
        return "400 Client Error";
    
    vector<string> split = splitter(url.substr(1), '/');
    string content = split.at(split.size() - 1);
    state->content = content;
    vector<string> contentSplit = splitter(content, '.');
    if (contentSplit.at(1) == "jpg")
        state->type = "image/jpeg";
    else if (contentSplit.at(1) == "html")
        state->type = "text/html";
    else if (contentSplit.at(1) == "png")
        state->type = "image/png";
    //time to get the doc_root split
    int doc_split_size = (int) splitter(state->doc_root.substr(1), '/').size();
    
//    for (int i = 0; i < doc_split_size; i++)
    split.erase(split.begin(), split.begin() + doc_split_size); //root popped off
    
    split.pop_back(); // content popped off
    int depth = 0;
    
    for (auto i = 0; i < split.size(); i++)
    {
        if (split.at(i) == ".")
            continue;
        else if (split.at(i) == "..")
            --depth;
        else
            ++depth;
    }

    if (depth >= 0)
    {
        int val = fileExistence(&url[0]);
        if (val == -1)
             return "404 Not Found"; // file was not found
        else if (val == 0)
            return "403 Forbidden"; // file exists but no read right
        else if (val == 1)
        {
            state->size = fileSize(&url[0]);
            state->lastModified = getFileCreationTime(&url[0]);
            return "200 OK";
        }
    }
    
    return "403 Forbidden"; // depth is less than 0 -> past the root directory
}   

long long WebParser::fileSize(const char* fileName){
    ifstream mySource;
    mySource.open(fileName, ios_base::binary);
    mySource.seekg(0,ios_base::end);
    long long size = mySource.tellg();
    mySource.close();
    return size;
}

// http://minix1.woodhull.com/manpages/man2/access.2.html
int WebParser::fileExistence(const char* fileName)
{
    //checks for existence of file and whether readable
    if (access(fileName, F_OK) == -1)
        return -1; // file itself does not exist
    else if (access(fileName, R_OK) == -1)
        return 0; // means no read access
    else //access(fileName, R_OK) == 0)
        return 1; // file exists and has read permission
}

string WebParser::getFileCreationTime(const char *path) {
    struct stat attr;
    stat(path, &attr);
    //printf("Last modified time: %s", ctime(&attr.st_mtime)); // this is how to print the date
    vector<string> split = splitter(ctime(&attr.st_mtime), ' '); // split the components by space
    string dayName      = split.at(0) + ", ";
    string day          = split.at(2) + " ";
    string month        = split.at(1) + " ";
    string year         = split.at(4);
    year.pop_back(); // need to remove the extraneous /n that is in the year somehow
    year += " ";
    string time         = split.at(3) + " GMT";
    // Last modified time: Mon Apr 30 22:05:22 2018 -> sample format from ctime(&attr.st_mtime)
    return dayName + day + month + year + time;
    
}
