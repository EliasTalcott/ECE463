#INSTRUCTIONS:
#1.Type "make" into the command line to compile your code
#2.Type "make test-short" to run the binary with "path_short.txt"
#3.Type "make test-very-long" to run the binary with "path_very_long.txt"
#NOTE : Make sure to always run "make" after editing your ".c" file
CC = gcc
SERVER = "httpserver"
PORT = 2112

server: httpserver.*.c
	$(CC) -g httpserver.*.c -o $(SERVER)

listen:
	./$(SERVER) $(PORT)

testgetty:
	./shift_client localhost $(PORT) texts/getty.txt 4

testblocked:
	./shift_client localhost $(PORT) texts/blocked.txt 0

testnotfound:
	./shift_client localhost $(PORT) texts/notfound.txt 0

testshort:
	./shift_client localhost $(PORT) texts/test_short.txt 9

testlong:
	./shift_client localhost $(PORT) texts/test_very_long.txt 0

clean:
	rm httpserver	
