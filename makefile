CC = g++

CLIBS = `pkg-config libxml-2.0 --cflags --libs`

master: master.cpp 
	$(CC) master.cpp -o master.out -lpthread -lcxxtools -lcxxtools-json $(CLIBS) 

clean: 
	rm -f *.o *.bin