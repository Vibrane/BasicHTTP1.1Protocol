#include <assert.h>
#include "WebFramer.hpp"
using namespace std;

void WebFramer::append(string chars)
{
    buffer = chars;
    createMessages();
}

void WebFramer::createMessages()
{
     for (auto character : buffer)
     {
         if (character == 13 and instruction.size() > 0) //means start of <CR><LF> with
         {
             messages.push_back(instruction);
             instruction = "";
         }
         // this wouuld mean that <CRLF> is on a line by itself.
         else if (character == 13 and instruction.size() == 0)
             messages.push_back("new line");

         else if (character == 10)
             continue;
         else
             // add the character to the instruction. Parser will take care of space.
             // you dont want counter here because if the instruction is not complete,
             // do not want to remove from buffer
             instruction += character;
     }

     buffer.clear(); // done parsing the parser
}

bool WebFramer::hasMessage() const
{
    return messages.size() > 0 ? true : false;
    
}

string WebFramer::topMessage() const
{
    return messages.at(0);
}

void WebFramer::popMessage()
{
    messages.erase(messages.begin(), messages.begin() + 1);
}
