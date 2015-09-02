CC = gcc
CFLAGS = -I.
LDFLAGS = -lcrypto
DEPS = 
OBJ = pem2mincrypt.o
EXE = pem2mincrypt

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	gcc -o $(EXE) $(OBJ) $(LDFLAGS)

.PHONY: clean

clean: 
	rm -f $(OBJ) $(EXE)

