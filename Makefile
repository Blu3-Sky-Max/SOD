

BLU3=gcc -g 

OUTPUT=sod

SRC=src

CFLAGS = -I include -Wall  $(shell pkg-config --cflags --libs libpipewire-0.3)

LIBS=-lm

$(OUTPUT): main.o profile.o mute.o source.o capture.o signal.o sink.o 
	$(BLU3) -o $(OUTPUT) main.o profile.o mute.o source.o capture.o signal.o sink.o  $(CFLAGS) $(LIBS)


main.o: $(SRC)/main.c 
	@$(BLU3) -c $(SRC)/main.c -o main.o $(CFLAGS)


profile.o: $(SRC)/sod_profile.c
	@$(BLU3) -c $(SRC)/sod_profile.c -o profile.o $(CFLAGS)




mute.o: $(SRC)/sod_mute.c
	@$(BLU3) -c $(SRC)/sod_mute.c -o  mute.o  $(CFLAGS)


source.o: $(SRC)/sod_source_mic.c
	@$(BLU3) -c $(SRC)/sod_source_mic.c -o source.o $(CFLAGS)

capture.o : $(SRC)/sod_capture_mic.c
	@$(BLU3) -c $(SRC)/sod_capture_mic.c -o capture.o $(CFLAGS)

signal.o : $(SRC)/sod_signal.c
	@$(BLU3) -c $(SRC)/sod_signal.c -o signal.o $(CFLAGS)

sink.o: $(SRC)/sod_sink.c
	@$(BLU3) -c $(SRC)/sod_sink.c -o sink.o $(CFLAGS)
clean: 
	@rm -f *.o $(OUTPUT) 

