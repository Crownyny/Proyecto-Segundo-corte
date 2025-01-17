/**
 * @file
 * @brief Interfaz del protocolo de comunicacion
 * @author Julian David Meneses <juliandavidm@unicauca.edu.co>
 * @author Andrea Carolina Realpe Munoz <andrearealpe@unicauca.edu.co>
 * @copyright MIT Liscense
 */
#pragma once

#include <sys/types.h>
#include <linux/limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>

#define HASH_SIZE 256 /**< Longitud del hash incluyendo NULL*/
#define COMMENT_SIZE 80 /** < Longitud del comentario */
#define BUFFSIZE 4086 /**< Tamaño del buffer de lectura/escritura. */

/**
 * @brief Codigo de retorno de operacion
 */
typedef enum {
	VERSION_ERROR, /*!< Error no especificado */
	VERSION_CREATED, /*!< Version creada */
	VERSION_ADDED, /*!< Version agregada */
	VERSION_ALREADY_EXISTS, /*!< Version ya existe */
	VERSION_NOT_FOUND, /*!< Version no encontrada */
	FILE_ADDED, /*<! Archivo adicionado  */
    SUCCESS, /*<! Comunicacion exitosa */
    ERROR /*<! Error de comunicacion */
	/* .. */
}return_code;

/**
 * @brief Codigo de operacion a realizar
 */
typedef enum {
	ADD, /*!< Adicionar un archivo */
    GET, /*!< Obtener una version de un archivo */
    LIST /*!< Listar versiones de un archivo */
}operation_type;

 
/**
 * @brief Estructura de operacion de adicion
 * Para realizar una  petición de adición de un archivo al repositorio
 * se envía el nombre del archivo, el hash de su contenido y un comentario
 * 
 * La respuesta se enviara mediante el socket
 * en el caso de que la operación sea exitosa, se envía el código de retorno VERSION_ADDED
 * en caso de que la versión ya exista, se envía el código de retorno VERSION_ALREADY_EXISTS
 * en caso de que ocurra un error, se envía el código de retorno VERSION_ERROR
 */
typedef struct {
	char username[50];
	char filename[PATH_MAX]; /**< Nombre del archivo original. */
	char hash[HASH_SIZE];           /**< Hash del contenido del archivo. */
	char comment[COMMENT_SIZE];	   /**< Comentario del usuario. */
} sadd;

/**
 * @brief Estructura de operacion de obtención
 * Para realizar una petición de obtención de una versión de un archivo
 * se envía el nombre del archivo y la versión que se desea obtener
 * 
 * La respuesta se enviara mediante el socket
 * en el caso de que la operación sea exitosa, se envía el código de retorno VERSION_CREATED
 * luego el tamaño del archivo en bytes, siendo esta la cantidad que se debe leer del socket,
 * en las siguientes lineas se envía el contenido del archivo
 * en caso de que la versión no exista, se envía el código de retorno VERSION_NOT_FOUND
 */
typedef struct {
	char username[50];
    char filename[HASH_SIZE]; /**< Nombre del archivo original. */
    size_t version; /**< Version del archivo a obtener */
} sget;

/**
 * @brief Estructura de operacion de listado
 * Para realizar una petición de listado de versiones de un archivo
 * se envía el nombre del archivo del cual se desean listar sus versiones 
 * o NULL si se desea listar todos los archivos del repositorio
 * 
 * La respuesta se enviara mediante el socket
 * en el caso de que la operación sea exitosa, se envía el código de retorno VERSION_CREATED
 * y luego una lista de versiones en las líneas siguientes
 * con el formato "filename hash(Primeros y ultimos 3 caracteres) comment"
 * cuando se pasa un filename diferente de NULL, adicionalmente se envía el número de versión
 * en caso de que no se encuentren versiones, se envía el código de retorno VERSION_NOT_FOUND
 */
typedef struct {
	char username[50];
    char filename[HASH_SIZE];  /**< Nombre del archivo original. */
} slist;


/**
 * @brief Copia de un socket hacia un archivo local
 * @param socket Socket de comunicacion
 * @param destination Archivo destino
 *
 * @return Resultado de la operacion (VERSION_ERROR o VERSION_CREATED)
 */
return_code local_copy(int socket, char * destination);

/**
 * @brief Copia un archivo hacia un socket de comunicacion
 *
 * @param source Archivo fuente
 * @param socket Socket de comunicacion
 *
 * @return Resultado de la operacion (VERSION_ERROR o VERSION_CREATED)
 */
return_code remote_copy(char * source, int socket);

/**
 * @brief Obtiene los datos del socket sin copiarlos
 *
 * @param source Archivo fuente
 * @param socket Socket de comunicacion
 *
 * @return Resultado de la operacion (VERSION_ERROR o VERSION_CREATED)
 */
void fake_local_copy(int socket);

/**
 * @brief Genera la ruta de la base de datos de versiones para un usuario específico
 *
 * Esta función genera la ruta completa del archivo de versiones para un usuario,
 * basándose en el nombre del usuario y en el directorio general de versiones.
 * 
 * @param username Nombre del usuario
 * @param db_path Buffer para almacenar la ruta generada
 * @param size Tamaño máximo del buffer db_path
 */
void get_user_db_path(const char *username, char *db_path, size_t size);

