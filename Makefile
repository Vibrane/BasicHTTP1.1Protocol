CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic -pthread

H_FILES=WebFramer.hpp WebParser.hpp httpd.hpp WebServer.hpp
FILES=WebServer.cpp DieWithError.cpp HandleTCPClient.cpp WebFramer.cpp WebParser.cpp httpd.cpp

all: httpd

httpd: $(FILES) $(H_FILES)
	$(CC) $(CFLAGS) -o httpd $(FILES)

clean:
	rm -rf httpd
