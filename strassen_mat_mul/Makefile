CC = G:\mingw64\bin\gcc.exe
CFLAGS = -std=gnu23 -O3
LDFLAGS = -lkernel32

bench.exe: strassen.c ubench.h
	$(CC) $(CFLAGS) -o $@ strassen.c $(LDFLAGS)

clean:
	-del /Q bench.exe

.PHONY: clean
