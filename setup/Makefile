setup : main.o crecs.o index.o
	cc -g -Wall -o setup  crecs.o index.o main.o
crecs.o : crecs.c setup.h
	cc -g -c crecs.c -std=gnu99
index.o : index.c setup.h
	cc -g -c index.c -std=gnu99
main.o : main.c setup.h
	cc -g -c main.c -std=gnu99
clean : 
	rm setup crecs.o index.o main.o
