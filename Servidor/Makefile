all:rversionsd.o versions.o protocol.o
	gcc -o rversionsd rversionsd.o versions.o protocol.o

%.o:%.c
	gcc -c $< -o $@

clean:
	rm -rf *.o client rversionsd docs

doc:
	doxygen