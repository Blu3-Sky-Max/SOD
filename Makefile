

BLU3=gcc -g 

OUTPUT=sod

SRC=src

CFLAGS = -I include -Wall  $(shell pkg-config --cflags --libs libpipewire-0.3)

LIBS=-lm

$(OUTPUT): main.o
	$(BLU3) -o $(OUTPUT) main.o $(CFLAGS)  $(LIBS)    	


main.o: $(SRC)/main.c 
	$(BLU3) -c $(SRC)/main.c -o main.o $(CFLAGS) 

Clean: 
	rm -f *.o $(OUTPUT) 


