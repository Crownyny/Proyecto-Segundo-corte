/**
 * @file
 * @brief  API de gestion de versiones
 * @author Julian David Meneses <juliandavidm@unicauca.edu.co>
 * @author Andrea Carolina Realpe Munoz <andrearealpe@unicauca.edu.co>
 * @copyright MIT Liscense
 */

#include "versions.h"


/**
 * @brief Escribe en el socket el resultado de la peticion add
 * 
 * @param socket   Socket de comunicacion
 * @param result  Resultado de la operacion que se escribira en la primera linea del socket 
 * @return return_code  Resultado de la operacion (result), si ocurre un error al escribir retorna ERROR
 */
return_code add_result(int socket, return_code result);

/**
 * @brief Escribe en el socket el resultado de la peticion get
 * 
 * @param socket   Socket de comunicacion
 * @param result  Resultado de la operacion que se escribira en la primera linea del socket 
 * @param hash Hash del archivo a devolver, si la operacion es exitosa
 * @return return_code  Resultado de la operacion (result), si ocurre un error al escribir retorna ERROR
 */
return_code get_result(int socket, return_code result, char * hash);

/**
 * @brief Verifica si existe una version para un archivo
 *
 * @param filename Nombre del archivo
 * @param hash Hash del contenido
 *
 * @return 1 si la version existe, 0 en caso contrario.
 */
int version_exists(const char *db_path, char * filename, char * hash);


/**
* @brief Almacena un archivo en el repositorio
*
* @param socket Socket de comunicacion
* @param filename Nombre del archivo
* @param hash Hash del archivo: nombre del archivo en el repositorio
*
* @return Resultado de la operacion
*/
return_code store_file(int socket, char * filename, char * hash);

/**
 * @brief Adiciona una nueva version de un archivo.
 *
 * @param filename Nombre del archivo.
 * @param comment Comentario de la version.
 * @param hash Hash del contenido.
 *
 * @return 1 en caso de exito, 0 en caso de error.
 */
return_code add_new_version(const char *db_path, sadd * req);


/**
* @brief Recupera un archivo del repositorio
*
* @param socket Socket de comunicacion
* @param hash Hash del archivo: nombre del archivo en el repositorio
*
* @return Resultado de la operacion
*/
return_code retrieve_file(int socket, char * hash);


return_code add(int socket, sadd * request) {
	char db_path[PATH_MAX];
    get_user_db_path(request->username, db_path, sizeof(db_path)); // Ruta específica del usuario

    // Verifica si ya existe una version con el mismo hash
	// Retorna VERSION_ALREADY_EXISTS si ya existe
	//version_exists(filename, v.hash)

	if(version_exists(db_path, request->filename, request->hash)  == VERSION_ALREADY_EXISTS)
		return add_result(socket, VERSION_ALREADY_EXISTS);

	// Almacena el archivo en el repositorio.
	// El nombre del archivo dentro del repositorio es su hash (sin extension)
	// Retorna VERSION_ERROR si la operacion falla
	if(store_file(socket, request->filename, request->hash) == VERSION_ERROR)
		return add_result(socket, VERSION_ERROR);

	// Agrega un nuevo registro al archivo versions.db
	// Si no puede adicionar el registro, se debe borrar el archivo almacenado en el paso anterior
	// Si la operacion falla, retorna VERSION_ERROR
	if(add_new_version(db_path, request) == VERSION_ERROR) {
		//char file_path[PATH_MAX];
		//snprintf(file_path, PATH_MAX, "%s/%s", VERSIONS_DIR, request->hash);
		//if (remove(request->filename) != 0) {
		//	perror("Remove failed");
		//}
		//else {
		//	printf("File %s deleted successfully", request->filename);
		//}

		//remove(file_path);
		remove(db_path);
		return add_result(socket, VERSION_ERROR);
	}
    
	// Si la operacion es exitosa, retorna VERSION_ADDED
	return add_result(socket, VERSION_ADDED);
}

return_code add_new_version(const char *db_path, sadd * req) {
	FILE * fp = fopen(VERSIONS_DB_PATH, "ab");
	
	if(!fp) return VERSION_ERROR;
	// Adiciona un nuevo registro (estructura) al archivo versions.db
	fwrite(req, sizeof *req, 1, fp);	
	fclose(fp);
	return VERSION_CREATED;
}


void list(int socket, slist * request) {
	char db_path[PATH_MAX];
	get_user_db_path(request->username, db_path, sizeof(db_path)); // Ruta específica del usuario
	FILE * fp; //Archivo de versiones

	if (!fp) {
        return_code result = VERSION_NOT_FOUND;
        send(socket, &result, sizeof(result), 0);
        return;
    }

    sadd sadd_buffer; //Estructura de versiones
    char buffer[PATH_MAX]; //Buffer para enviar los datos al cliente
	int version_count = 0; //Contador de versiones
	//ssize_t read_count; //Cantidad de bytes leidos
	//return_code result = VERSION_CREATED	; //Resultado de la operacion
	int msg_size; //Tamaño del mensaje
	
	//Abre el la base de datos de versiones (versions.db)
	//if(!(fp = fopen(VERSIONS_DB_PATH, "r"))) return ;

	//Muestra los registros cuyo nombre coincide con filename.
	//Si filename es NULL, muestra todos los registros.
    // while(read_count = fread(&sadd_buffer, sizeof(sadd), 1, fp), read_count > 0) {
	// 	if(request->filename[0] == '\0') //Si filename es NULL, muestra todos los registros
    //     {
	// 		if(version_count == 0) send(socket, &result, sizeof(return_code), 0); //Envia VERSION_CREATED si se encuentra el archivo

    //         snprintf(buffer,sizeof(buffer),"%s %.3s...%.3s %s\n", sadd_buffer.filename, sadd_buffer.hash, 
    //         (sadd_buffer.hash + strlen(sadd_buffer.hash) - 3), sadd_buffer.comment); //Guarda el registro en el buffer
	// 		msg_size = strlen(buffer);
	// 		send (socket, &msg_size, sizeof(int), 0); //Envia el tamaño del mensaje
    //         send(socket, buffer, msg_size, 0); //Envia el buffer al cliente
	// 		version_count++;
    //         continue;
    //     }

	// 	if(EQUALS(sadd_buffer.filename, request->filename)) //Si filename no es NULL, muestra los registros cuyo nombre coincide con filename
	// 	{
	// 		if(version_count == 0) send(socket, &result, sizeof(return_code), 0); //Envia VERSION_CREATED si se encuentra el archivo

    //         snprintf(buffer,sizeof(buffer),"%s %.3s...%.3s %s (%d)\n", sadd_buffer.filename, sadd_buffer.hash,  
    //         (sadd_buffer.hash + strlen(sadd_buffer.hash) - 3), sadd_buffer.comment, version_count++); //Guarda el registro en el buffer
	// 		msg_size = strlen(buffer);
	// 		send (socket, &msg_size, sizeof(int), 0); //Envia el tamaño del mensaje
    //         send(socket, buffer, msg_size, 0); //Envia el buffer al cliente
	// 	}
	// }

	while (fread(&sadd_buffer, sizeof(sadd), 1, fp) > 0) {
        if (request->filename[0] == '\0' || EQUALS(sadd_buffer.filename, request->filename)) {
            snprintf(buffer, sizeof(buffer), "%s %.3s...%.3s %s\n", 
                     sadd_buffer.filename, sadd_buffer.hash, 
                     (sadd_buffer.hash + strlen(sadd_buffer.hash) - 3), 
                     sadd_buffer.comment);
            msg_size = strlen(buffer);
            send(socket, &msg_size, sizeof(int), 0);
            send(socket, buffer, msg_size, 0);
            version_count++;
        }
    }

	if(version_count == 0) 
	{
		return_code result = VERSION_NOT_FOUND;
		send(socket, &result, sizeof(result), 0); //Si no se encuentra el archivo, envia VERSION_NOT_FOUND
	}

	fclose(fp);
}

int version_exists(const char *db_path, char * filename, char * hash) {
	FILE *fp;
	ssize_t read_count;
	sadd sadd_buffer;
	
	if(!(fp = fopen(VERSIONS_DB_PATH, "r"))) return -1;

	while(read_count = fread(&sadd_buffer, sizeof sadd_buffer, 1, fp), read_count > 0) {	
		if (EQUALS(filename, sadd_buffer.filename) && EQUALS(hash, sadd_buffer.hash)) {
			fclose(fp);
			return VERSION_ALREADY_EXISTS;
		}
	}

	fclose(fp);
    return 1;  
}

return_code get(int socket, sget * request) {
	char db_path[PATH_MAX];
    get_user_db_path(request->username, db_path, sizeof(db_path)); // Ruta específica del usuario

	sadd sadd_buffer; //Estructura de versiones que guardara temporalmente los registros leidos
	int count =0; //Contador de versiones
	//ssize_t nread; //Cantidad de bytes leidos

	FILE * fp = fopen(VERSIONS_DB_PATH, "r"); //Abre el archivo de versiones
	if(!fp) return get_result(socket, VERSION_NOT_FOUND, NULL); //Si no se puede abrir el archivo de versiones, retorna VERSION_NOT_FOUND

	//while (nread = fread(&sadd_buffer, sizeof sadd_buffer, 1, fp), nread > 0) //Lee el archivo de versiones
	while (fread(&sadd_buffer, sizeof(sadd), 1, fp) > 0)
	{
		printf("Filename: %s %s\n",request->filename, sadd_buffer.filename);
		if(EQUALS(request->filename, sadd_buffer.filename) && count++ == request->version)
        {
			fclose(fp);
			return get_result(socket, VERSION_CREATED, sadd_buffer.hash); //Si se encuentra la version solicitada, retorna VERSION_CREATED
        }
	}

	fclose(fp);
    return get_result(socket, VERSION_NOT_FOUND, NULL); //Si no se encuentra la version solicitada retorna VERSION_NOT_FOUND
}

return_code store_file(int socket, char * filename, char * hash){
	char dst_filename[PATH_MAX];
	snprintf(dst_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	return local_copy(socket, dst_filename);
}

return_code add_result(int socket, return_code result) {
    int bytes_sent = send(socket, &result, sizeof(return_code), 0);
    if (bytes_sent < 0) {
        perror("Error sending result");
        return ERROR;
    }

    return result;
}

return_code get_result(int socket, return_code result, char * hash) {
    int bytes_sent = send(socket, &result, sizeof(return_code), 0);
    if (bytes_sent < 0) {
        perror("Error sending result");
        return ERROR;
    }

    if (result != VERSION_CREATED) return result;

    return retrieve_file(socket, hash);
}

return_code retrieve_file(int socket, char * hash) {
	char src_filename[PATH_MAX]; 
	snprintf(src_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	return remote_copy(src_filename, socket); //Copia el archivo del repositorio al socket
}
