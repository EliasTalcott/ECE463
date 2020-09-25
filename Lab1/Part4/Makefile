#INSTRUCTIONS:
#1.Type "make" into the command line to compile your code
#2.Type "make test-short" to run the binary with "path_short.txt"
#3.Type "make test-very-long" to run the binary with "path_very_long.txt"
#NOTE : Make sure to always run "make" after editing your ".c" file
CC = gcc
SERVER = "multi_service_server"
PORT_TCP = 2112
PORT_UDP = 3112

server: multi_service_server.*.c
	$(CC) -g multi_service_server.*.c -o $(SERVER)

listen:
	./$(SERVER) $(PORT_TCP) $(PORT_UDP)

testgetty:
	./multi_service_client localhost $(PORT_TCP) texts/getty.txt $(PORT_UDP) 4

testblocked:
	./multi_service_client localhost $(PORT_TCP) texts/blocked.txt $(PORT_UDP) 0

testnotfound:
	./multi_service_client localhost $(PORT_TCP) texts/notfound.txt $(PORT_UDP) 0

testshort:
	./multi_service_client localhost $(PORT_TCP) texts/test_short.txt $(PORT_UDP) 0

testlong:
	./multi_service_client localhost $(PORT_TCP) texts/test_very_long.txt  $(PORT_UDP) 0

clean:
	rm $(SERVER)
