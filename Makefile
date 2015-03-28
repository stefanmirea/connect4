# Completati va rog urmatoarele 3 linii cu informatiile voastre personale.
# Daca aveti mai multe prenume, alegeti doar unul dintre ele.
NUME=Mirea
PRENUME=Stefan
GRUPA=311CC
# ^ Doar cele trei de sus.

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lcurses

.PHONY: build run compilegen generate pack clean cleanall

build:
	$(CC) $(CFLAGS) sursa*.c -o Connect4 $(LDFLAGS)

run: build
	./Connect4

compilegen:
	$(CC) $(CFALGS) generate.c -o Generate

generate: compilegen
	./Generate

pack: cleanall
	zip -r $(GRUPA)_$(NUME)_$(PRENUME)_tema3.zip *

clean:
	rm -f *.zip *.o Connect4 Generate
	clear

cleanall: clean
	rm -f last_conf.ini settings.ini alltime.dat saved.dat
	clear

