all:client.o request.o protocol.o sha256.o
	gcc -o client client.o request.o protocol.o sha256.o

%.o:%.c
	gcc -c $< -o $@

clean:
	rm -rf *.o client server docs

doc:
	doxygen