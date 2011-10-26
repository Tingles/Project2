lookup : main.o crecs.o index.o parsers.o header.o
	cc -g -Wall -o lookup  crecs.o index.o parsers.o header.o main.o
crecs.o : crecs.c setup.h
	cc -g -Wall -c crecs.c -std=gnu99
index.o : index.c setup.h
	cc -g -Wall -c index.c -std=gnu99
parsers.o : parsers.c setup.h
	cc -g -Wall -c parsers.c -std=gnu99
header.o : header.c setup.h
	cc -g -Wall -c header.c -std=gnu99
main.o : main.c setup.h
	cc -g -Wall -c main.c -std=gnu99
clean : 
	rm lookup lookup.o crecs.o index.o parsers.o header.o