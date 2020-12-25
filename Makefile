.phony all:
all: pman inf

pman: pman.c
	gcc pman.c -Wall -lreadline -o PMan

inf: inf.c
	gcc -o inf inf.c

.PHONY clean:
clean:
	-rm -rf *.o *.exe
