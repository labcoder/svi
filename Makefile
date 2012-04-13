all: svi
svi: svi.c
	gcc -o svi svi.c
clean:
	rm svi
