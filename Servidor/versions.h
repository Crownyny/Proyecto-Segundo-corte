/**
 * @file
 * @brief API de gestion de versiones
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @author Julian David Meneses <juliandavidm@unicauca.edu.co>
 * @author Andrea Carolina Realpe Munoz <andrearealpe@unicauca.edu.co>
 * @copyright MIT License
*/
#pragma once

#ifndef VERSIONS_H
#define VERSIONS_H

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/limits.h>
#include <libgen.h>


#include "protocol.h"

#define VERSIONS_DB "versions.db" /**< Nombre de la base de datos de versiones. */
#define VERSIONS_DIR ".versions" /**< Directorio del repositorio. */
#define VERSIONS_DB_PATH VERSIONS_DIR "/" VERSIONS_DB /**< Ruta completa de la base de datos.*/

#define EQUALS(s1, s2) (strcmp(s1, s2) == 0) /**< Verdadero si dos cadenas son iguales.*/

/**
 * @brief Adiciona un archivo al repositorio.
 *
 * @param filename Nombre del archivo a adicionar
 * @param comment Comentario de la version actual
 *
 * @return Codigo de la operacion
 */
return_code add(int socket, sadd * request);

/**
 * @brief Lista las versiones de un archivo.
 *
 * @param filename Nombre del archivo, NULL para listar todo el repositorio.
 */
void list(int socket, slist * request);

/**
 * @brief Obtiene una version del un archivo.
 * Sobreescribe la version existente.
 *
 * @param filename Nombre de archivo.
 * @param version Numero secuencial de la version.
 *
 * @return 1 en caso de exito, 0 si ocurre un error.
 */
return_code get(int socket, sget * request);

#endif
