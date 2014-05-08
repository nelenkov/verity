CC = gcc
CFLAGS = -I.
LDFLAGS = -lcrypto
DEPS = 
OBJ = pem2mincrypt.o
EXE = pem2mincrypt

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	gcc $(LDFLAGS) -o $(EXE) $(OBJ)

.PHONY: clean

clean: 
	rm $(OBJ) $(EXE)

