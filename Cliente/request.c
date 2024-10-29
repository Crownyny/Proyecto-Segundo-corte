
/**
 * @file
 * @brief Implementacion del protocolo de solicitud 
 * @author Julian David Meneses <juliandavidm@unicauca.edu.co>
 * @author Andrea Carolina Realpe Munoz <andrearealpe@unicauca.edu.co>
 * @copyright MIT Liscense
 */
#include "request.h"

/**
 * @brief Obtiene el hash de un archivo
 * 
 * @param filename Nombre del archivo
 * @param hash Buffer para almacenar el hash
 * 
 * @return Referencia al buffer, NULL si ocurre un error
 */
char *get_file_hash(char * filename, char * hash);


return_code create_sadd(char * filename, char * comment, sadd * result, char * username) {

    struct stat s;
    char hash[HASH_SIZE];

    // Verifica que el archivo exista y sea un archivo regular
    if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
        return VERSION_ERROR;
    }

	// Obtiene el HASH del archivo y construye la ruta nueva
	get_file_hash(filename, hash);

	// Limpia la estructura de version
	memset(result,0,sizeof *result);

	// Copia los datos a la estructura
	strcpy(result->filename, filename);
	strcpy(result->hash, hash);
	strcpy(result->comment, comment);
    strcpy(result->username, username);

    return VERSION_CREATED;
}

return_code add_request(int socket, sadd * request) {
    ssize_t nwrite;
    operation_type op = ADD;


    // Envia el codigo de operacion
    nwrite = send(socket,&op, sizeof(operation_type), 0);
    if (nwrite != sizeof(operation_type)) {
        return ERROR;
    }

    // Envia la estructura de solicitud
    nwrite = send(socket, request, sizeof(sadd), 0);
    if (nwrite != sizeof(sadd)) {
        return ERROR;
    }

    if (remote_copy(request -> filename, socket) == VERSION_ERROR) {
        return ERROR;
    }

    return SUCCESS;
}

return_code list_request(int socket, slist * request) {
    ssize_t nwrite;
    operation_type op = LIST;

    // Envia el codigo de operacion
    nwrite = send(socket,&op, sizeof(operation_type), 0);
    if (nwrite != sizeof(operation_type)) {
        return ERROR;
    }

    // Envia la estructura de solicitud
    nwrite = send(socket, request, sizeof(slist), 0);
    if (nwrite != sizeof(slist)) {
        return ERROR;
    }

    return SUCCESS;
}

return_code get_request(int socket, sget * request) {
    ssize_t nwrite;
    operation_type op = GET;

    // Envia el codigo de operacion
    nwrite = send(socket,&op, sizeof(operation_type), 0);
    if (nwrite != sizeof(operation_type)) {
        return ERROR;
    }

    // Envia la estructura de solicitud
    nwrite = send(socket, request, sizeof(sget), 0);
    if (nwrite != sizeof(sget)) {
        return ERROR;
    }

    return SUCCESS;
}

return_code print_list(int socket) {
    char buffer[BUFFSIZE];
    char temp_buffer[BUFFSIZE];
    int nread;
    int total_bytes = 0;
    int msg_size = 0;

    printf("List of versions:\n");

    recv(socket, &msg_size, sizeof(int),0); // Espera el tamaño del primer mensaje
    do {
        memset(buffer, 0, sizeof(buffer));
        nread = recv(socket, buffer, msg_size, MSG_DONTWAIT);
        
        if (nread > 0) {
            printf("%s", buffer); // Solo imprimir si hay datos leídos
        } else if (nread == -1) {
            // Verificar si el error es EAGAIN o EWOULDBLOCK
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("recv"); // Solo imprimir error si es un error diferente
            }
        }
        
        // Lee el tamaño del siguiente mensaje
        if (recv(socket, &msg_size, sizeof(int), MSG_DONTWAIT) <= 0) {
            break; // Salir si hay un error o si no hay más datos
        }

    } while (1);

    return SUCCESS;
}

char *get_file_hash(char * filename, char * hash) {
	char *comando;
	FILE * fp;

	struct stat s;

	//Verificar que el archivo existe y que se puede obtener el hash
	if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
		perror("stat");
		return NULL;
	}

	sha256_hash_file_hex(filename, hash);

	return hash;
}
