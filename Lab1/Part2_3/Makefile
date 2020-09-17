#INSTRUCTIONS:
#1.Type "make" into the command line to compile your code
#2.Type "make test-short" to run the binary with "path_short.txt"
#3.Type "make test-very-long" to run the binary with "path_very_long.txt"
#NOTE : Make sure to always run "make" after editing your ".c" file
CC = gcc
SERVER = "httpserver"
PORT = 2112

server: httpserver.*.c
	$(CC) httpserver.*.c -o $(SERVER)

listen:
	./$(SERVER) $(PORT)

clean:
	rm httpserver	
