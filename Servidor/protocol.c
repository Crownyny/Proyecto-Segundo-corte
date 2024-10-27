/**
 * @file
 * @brief Implementacion del protocolo de comunicacion
 * @author Julian David Meneses <juliandavidm@unicauca.edu.co>
 * @author Andrea Carolina Realpe Munoz <andrearealpe@unicauca.edu.co>
 * @copyright MIT Liscense
 */

#include "protocol.h"
#include "versions.h"

return_code local_copy(int socket, char * destination) {
	// Copia el contenido de source a destination (se debe usar open-read-write-close, o fopen-fread-fwrite-fclose)
	FILE *fd; // Archivos fuente y destino
	char buffer[BUFFSIZE]; // Buffer de lectura/escritura, mas 1 para el caracter nulo
	ssize_t filesz, nread; // Cantidad de bytes leidos
	if(recv(socket, &filesz, sizeof(filesz), 0) != sizeof(filesz)) return VERSION_ERROR; // Recibe el tamaño del archivo
	// Abre los archivos fuente y destino y retorna VERSION_ERROR si no se pueden abrir

	if(!(fd = fopen(destination, "w"))) return VERSION_ERROR;

	printf("File %s created\n", destination);
	printf("File size: %ld\n", filesz);
	while (filesz != 0) // Lee el archivo fuente
	{
		memset(buffer, 0, BUFFSIZE);
		// Determinar tamaño de lectura (si filesz es negativo, usar su valor absoluto) 
		ssize_t to_read = (filesz > 0) ? 
					(filesz<BUFFSIZE) ? filesz :BUFFSIZE : -filesz; //(si el filesz es menor que BUFFSIZE, usar filesz)
		
		
		// Recibe el contenido del archivo
		ssize_t total_read = 0;
		ssize_t nread = recv(socket, &buffer, to_read, 0);
		total_read += nread;
		while (total_read < to_read)
		{
			nread = recv(socket, buffer + total_read, to_read - total_read, 0);
			
			if (nread < 0)
			{
				fclose(fd);
				printf("Error reading file\n");
				return VERSION_ERROR;
			}
			
			if (nread == 0) // Se alcanzó el final de la transmisión
			{
				break;
			}

			total_read += nread;
		}

		if (total_read != to_read)
		{
			fclose(fd);
			printf("Incomplete file read\n");
			return VERSION_ERROR;
		}

		// Escribe el archivo destino
		if(fwrite(buffer, sizeof(char), to_read, fd) != to_read)
		{
			fclose(fd);
			printf("Error writing file\n");

			return VERSION_ERROR;
		}

		// Reducir filesz según el número de bytes leídos
		filesz -= to_read;
	}

	fclose(fd);
	printf("File %s copied\n", destination);
	return VERSION_CREATED;
}

return_code remote_copy(char * source, int socket) {
	FILE *fr; // Archivo fuente
	char buffer[BUFFSIZE + 1]; // Buffer de lectura/escritura, mas 1 para el caracter nulo
	ssize_t nread; // Cantidad de bytes leidos
    struct stat st; // Estructura de estadisticas de archivos

	if(!(fr = fopen(source, "r"))) return VERSION_ERROR; // Abre el archivo fuente y retorna VERSION_ERROR si no se puede abrir
	
	if(stat(source, &st) != 0) return VERSION_ERROR; // Obtiene las estadisticas del archivo fuente y retorna VERSION_ERROR si no se puede obtener

	if(send(socket, &st.st_size, sizeof(st.st_size), 0) != sizeof(st.st_size) ) return VERSION_ERROR; // Envía el tamaño del archivo al socket

	while(nread = fread(buffer , sizeof(char), BUFFSIZE, fr), nread > 0) // Lee el archivo fuente
	{
		if(send(socket, buffer, nread, 0) != nread) // Envía el contenido del archivo al socket
		{
			fclose(fr);
			return VERSION_ERROR;
		}
	}

	if (nread == 0) // Si se leyó todo el archivo, retorna VERSION_CREATED
	{
		fclose(fr);
		return VERSION_CREATED;
	}

	return VERSION_ERROR;
}

void fake_local_copy(int socket) {
// Copia el contenido de source a destination (se debe usar open-read-write-close, o fopen-fread-fwrite-fclose)
	char buffer[BUFFSIZE]; // Buffer de lectura/escritura, mas 1 para el caracter nulo
	ssize_t filesz, nread; // Cantidad de bytes leidos
	if(recv(socket, &filesz, sizeof(filesz), 0) != sizeof(filesz)) return ; // Recibe el tamaño del archivo
	// Abre los archivos fuente y destino y retorna VERSION_ERROR si no se pueden abrir

	printf("File size: %ld\n", filesz);
	while (filesz != 0) // Lee el archivo fuente
	{
		memset(buffer, 0, BUFFSIZE);
		// Determinar tamaño de lectura (si filesz es negativo, usar su valor absoluto) 
		ssize_t to_read = (filesz > 0) ? 
					(filesz<BUFFSIZE) ? filesz :BUFFSIZE : -filesz; //(si el filesz es menor que BUFFSIZE, usar filesz)
		
		
		// Recibe el contenido del archivo
		ssize_t nread = recv(socket, &buffer, to_read, 0);
		if (nread != to_read)
		{
			printf("Error reading file\n");

			return ;
		}

		// Reducir filesz según el número de bytes leídos
		filesz -= to_read;
	}

	return ;
}
void get_user_db_path(const char *username, char *db_path, size_t size) {
    snprintf(db_path, size, "%s/versions_%s.db", VERSIONS_DIR, username);
}