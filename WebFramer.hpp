#ifndef WEBFRAMER_HPP
#define WEBFRAMER_HPP
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class WebFramer {
public:
    void append(string chars);
    void createMessages();
    // Does this buffer contain at least one complete message?
    bool hasMessage() const;

    // Returns the first instruction
    string topMessage() const;

    // Removes the first instruction
    void popMessage();

    void printStuff();

protected:
    string buffer;
    vector <string> messages;
    string instruction = "";

};

#endif // CALCFRAMER_HPP
