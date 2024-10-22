/**
 * @file
 * @brief Interfaz del protocolo de solicitud al servidor
 * @author Julian David Meneses <juliandavidm@unicauca.edu.co>
 * @author Andrea Carolina Realpe Munoz <andrearealpe@unicauca.edu.co>
 * @copyright MIT Liscense
 */
#include <sys/stat.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>

#include "protocol.h"
#include "sha256.h"

/**
 * @brief Crea una estructura de solicitud de adición acorde al protocolo
 * 
 * @param filename Nombre del archivo
 * @param comment Comentario de la version
 * @param result Estructura de version
 * 
 * @return Codigo de la operacion (VERSION_CREATED si la operacion es exitosa, VERSION_ERROR si ocurre un error)
 */
return_code create_sadd(char * filename, char * comment, sadd * result);

/**
 * @brief Crea un archivo con el resultado de la operacion get
 * 
 * @param socket   Socket de comunicacion
 * @return return_code  Resultado de la operacion (result), si ocurre un error al escribir retorna ERROR
 */
return_code make_getted_file(int socket);

/**
 * @brief Lee la respuesta de una solicitud de listado de versiones y la imprime en consola
 * 
 * @param socket Socket de comunicacion
 * 
 * @return int Resultado de la operacion (VERSION_ERROR si ocurre un error, VERSION_CREATED si la operacion es exitosa)
 */
return_code print_list(int socket);

/**
 * @brief Peticion de operacion add al servidor
 * 
 * @param socket Socket de comunicacion
 * @param request  Estructura de operacion de adicion
 * @return int  Resultado de la operacion (result), si ocurre un error al escribir retorna ERROR
 *           si la operacion es exitosa, retorna SUCCESS
 */
return_code add_request(int socket, sadd * request);

/**
 * @brief Peticion de operacion get al servidor
 * 
 * @param socket Socket de comunicacion
 * @param request  Estructura de operacion de obtencion
 * @return int  Resultado de la operacion (result), si ocurre un error al escribir retorna ERROR
 *           si la operacion es exitosa, retorna SUCCESS
 */
return_code get_request(int socket, sget * request);

/**
 * @brief Peticion de operacion list al servidor
 * 
 * @param socket Socket de comunicacion
 * @param request  Estructura de operacion de listado
 * @return int  Resultado de la operacion (result), si ocurre un error al escribir retorna ERROR
 *           si la operacion es exitosa, retorna SUCCESS
 */
return_code list_request(int socket, slist * request);
