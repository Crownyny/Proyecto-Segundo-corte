all:server.o versions.o protocol.o
	gcc -o server server.o versions.o protocol.o

%.o:%.c
	gcc -c $< -o $@

clean:
	rm -rf *.o client server docs

doc:
	doxygen