

BLU3=gcc -g 

OUTPUT=sod

SRC=src

CFLAGS = -I include -Wall  $(shell pkg-config --cflags --libs libpipewire-0.3)

LIBS=-lm

$(OUTPUT): main.o profile.o mute.o mic.o capture.o
	$(BLU3) -o $(OUTPUT) main.o profile.o mute.o mic.o capture.o  $(CFLAGS) $(LIBS)


main.o: $(SRC)/main.c 
	@$(BLU3) -c $(SRC)/main.c -o main.o $(CFLAGS)


profile.o: $(SRC)/sod_profile.c
	@$(BLU3) -c $(SRC)/sod_profile.c -o profile.o $(CFLAGS)




mute.o: $(SRC)/sod_mute.c
	@$(BLU3) -c $(SRC)/sod_mute.c -o  mute.o  $(CFLAGS)


mic.o: $(SRC)/sod_mic_controller.c
	@$(BLU3) -c $(SRC)/sod_mic_controller.c -o mic.o $(CFLAGS)

capture.o : $(SRC)/sod_capture_mic.c
	@$(BLU3) -c $(SRC)/sod_capture_mic.c -o capture.o $(CFLAGS) 
clean: 
	@rm -f *.o $(OUTPUT) 

