#include <iostream>
#include "httpd.hpp"

using namespace std;

void start_httpd(unsigned short port, string doc_root)
{
	cerr << "Starting server (port: " << port <<
		", doc_root: " << doc_root << ")" << endl;
    
}
