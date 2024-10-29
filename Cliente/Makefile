all:rversions.o request.o protocol.o sha256.o
	gcc -o rversions rversions.o request.o protocol.o sha256.o

%.o:%.c
	gcc -c $< -o $@

clean:
	rm -rf *.o rversions server docs

doc:
	doxygen