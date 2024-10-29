/**
 * @file
 * @brief Programa cliente
 * 
 * Este archivo contiene la implementación de un cliente que se conecta a un servidor y envía solicitudes de adición, obtención y listado de archivos en un repositorio.
 * 
 * @details El cliente utiliza sockets para la comunicación y envía solicitudes de adición, obtención y listado de archivos en un repositorio.
 * El cliente crea el hash del archivo al que desea realizar un add 
 * El cliente genera el archivo que obtiene mediante el coket al realizar una operacion get
 * 
 * @version 1.0
 * 
 * @section LICENSE
 * MIT License
 * 
 * @section AUTHOR
 * Julian David Meneses <juliandavidm@unicauca.edu.co>
 */
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>

#include "request.h"

/**
 * @brief Manejador de señales
 * 
 * Esta función se llama cuando se recibe una señal SIGINT o SIGTERM.
 * Se encarga de cerrar el cliente de manera ordenada.
 * 
 * @param signo Número de la señal recibida 
 */
void sig_handler(int signo);

/**
 * @brief Muestra el mensaje de uso del cliente
 */
void usage();

int client_socket; // Socket del cliente

int main(int argc, char *argv[])
{
    if (argc < 3) { // Verificar que se hayan pasado los argumentos necesarios
        fprintf(stderr,"Usage: %s <server id> <port> <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Manejamos las señales SIGINT y SIGTERM
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    // Obtenemos la id y el puerto del servidor
    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    char *username = argv[3];

    // Verificamos que el nombre de usuario no sea muy largo
    if(strlen(username) > COMMENT_SIZE)
    {
        fprintf(stderr, "Username too long\n");
        exit(EXIT_FAILURE);
    }

    // Creamos el socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Creamos la estructura de dirección del servidor
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convertimos la dirección IP a binario
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Error converting IP address");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Conectamos al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server %s:%d\n", server_ip, port);

    int LINESIZE = 512;
    char line[LINESIZE], filename[HASH_SIZE], comment[COMMENT_SIZE];
    size_t version;
    return_code result;

    while (1) {
        fgets(line, LINESIZE, stdin);
        line[strlen(line) - 1] = '\0';

        // Procesamos la línea de petición add
        if(sscanf(line, "add %s \"%[^\"]\"",filename, comment) == 2)
        {
            sadd sadd_request;
            memset(&sadd_request, 0, sizeof(sadd));
            // Verificamos que el archivo exista y sea un archivo regular
            if(create_sadd(filename, comment, &sadd_request, username) == VERSION_ERROR)
            {
                printf("The file does not exist or is not a regular file\n");
                continue;
            }

            // Hacemos la peticion de adición
            if(add_request(client_socket, &sadd_request) == ERROR)
            {
                printf("Error sending sadd request\n");
                continue;
            }

            // Recibimos el resultado de la petición
            if(recv(client_socket, &result, sizeof(return_code), 0) != sizeof(return_code))
            {
                printf("Error receiving result\n");
                continue;
            }

            if(result == VERSION_ALREADY_EXISTS)
            {
                printf("Version already exists\n");
                continue;
            }

            if(result == VERSION_ERROR)
            {
                printf("Error adding version\n");
                continue;
            }

            printf("Version added\n");
            continue;
        }

        if (sscanf(line, "list %s", filename) == 1)
        {
            slist slist_request;
            memset(&slist_request, 0, sizeof(slist));
            strcpy(slist_request.filename, filename);
            strcpy(slist_request.username, username);

            // Hacemos la petición de listado
            if(list_request(client_socket, &slist_request) == ERROR)
            {
                printf("Error sending slist request\n");
                continue;
            }
            
            // Recibimos el resultado de la petición
            if(recv(client_socket, &result, sizeof(return_code), 0) != sizeof(return_code))
            {
                printf("Error receiving result\n");
                continue;
            }

            // Si la versión no existe, mostramos un mensaje
            if(result == VERSION_NOT_FOUND)
            {
                printf("Version not found\n");
                continue;
            }

            print_list(client_socket);
            continue;
        }

        if(sscanf(line, "get %s %s", comment, filename) == 2)
        {
            sget sget_request;
            memset(&sget_request, 0, sizeof(sget));
            strcpy(sget_request.filename, filename);
            strcpy(sget_request.username, username);
            sget_request.version = atoi(comment);

            // Hacemos la petición de obtención
            if(get_request(client_socket, &sget_request) == ERROR)
            {
                printf("Error sending sget request\n");
                continue;
            }

            // Recibimos el resultado de la petición
            if(recv(client_socket, &result, sizeof(return_code), 0) != sizeof(return_code))
            {
                printf("Error receiving result\n");
                continue;
            }

            // Si la versión no existe, mostramos un mensaje
            if(result == VERSION_NOT_FOUND)
            {
                printf("Version not found\n");
                continue;
            }


            local_copy(client_socket, filename);

            continue;
        }

       if(strcmp(line, "list") == 0)    
        {
            slist slist_request;
            memset(&slist_request, 0, sizeof(slist));
            slist_request.filename[0] = '\0';
            strcpy(slist_request.username, username);

            // Hacemos la petición de listado
            if(list_request(client_socket, &slist_request) == ERROR)
            {
                printf("Error sending slist request\n");
                continue;
            }

            // Recibimos el resultado de la petición
            if(recv(client_socket, &result, sizeof(return_code), 0) != sizeof(return_code))
            {
                printf("Error receiving result\n");
                continue;
            }

            // Si la versión no existe, mostramos un mensaje
            if(result == VERSION_NOT_FOUND)
            {
                printf("version not found\n");
                continue;
            }

            // Imprimimos la lista
            print_list(client_socket);
            continue;
        }

        usage();
    }
}

void sig_handler(int signo) {
    printf("Shutting down client...\n");
    close(client_socket);
    exit(EXIT_SUCCESS);
}

void usage() {
    printf("Usage:\n");
    printf("add <filename> \"<comment>\"\n");
    printf("get <version> <filename>\n");
    printf("list <filename>(optional)\n");
}