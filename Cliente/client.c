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
void usage(const char *server_ip, int port, const char *username);


int client_socket; // Socket del cliente

int main(int argc, char *argv[])
{
    if (argc < 4) { // Verificar que se hayan pasado los argumentos necesarios
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

    // Validamos el nombre de ususario
    if (strlen(username) == 0) {
        fprintf(stderr, "Error: Username cannot be empty.\n");
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

        if(sscanf(line, "add %s \"%[^\"]\"",filename, comment) == 2)
        {
            sadd sadd_request;
            memset(&sadd_request, 0, sizeof(sadd));
            strcpy(sadd_request.username, username);//Incluye el username para que el servidor gestione
            if(create_sadd(filename, comment, &sadd_request) == VERSION_ERROR)
            {
                printf("The file does not exist or is not a regular file\n");
                continue;
            }

            if(add_request(client_socket, &sadd_request) == ERROR)
            {
                printf("Error sending sadd request\n");
                continue;
            }

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
            strcpy(slist_request.username, username);//Incluye el username para que el servidor gestione

            if(list_request(client_socket, &slist_request) == ERROR)
            {
                printf("Error sending slist request\n");
                continue;
            }

            if(recv(client_socket, &result, sizeof(return_code), 0) != sizeof(return_code))
            {
                printf("Error receiving result\n");
                continue;
            }

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
            strcpy(sget_request.username, username);//Incluye el username para que el servidor gestione
            sget_request.version = atoi(comment);

            if(get_request(client_socket, &sget_request) == ERROR)
            {
                printf("Error sending sget request\n");
                continue;
            }

            if(recv(client_socket, &result, sizeof(return_code), 0) != sizeof(return_code))
            {
                printf("Error receiving result\n");
                continue;
            }

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

            if(list_request(client_socket, &slist_request) == ERROR)
            {
                printf("Error sending slist request\n");
                continue;
            }

            if(recv(client_socket, &result, sizeof(return_code), 0) != sizeof(return_code))
            {
                printf("Error receiving result\n");
                continue;
            }

            if(result == VERSION_NOT_FOUND)
            {
                printf("version not found\n");
                continue;
            }

            print_list(client_socket);
            continue;
        }

        usage(server_ip, port, username);

    }
}

void sig_handler(int signo) {
    printf("Shutting down client...\n");
    close(client_socket);
    exit(EXIT_SUCCESS);
}

void usage(const char *server_ip, int port, const char *username) {
    printf("------------------Usage-----------------\n");
    printf("You are connected to server %s:%d as user '%s'\n", server_ip, port, username);
    printf("Commands:\n");
    printf("  add <filename> \"<comment>\"\n");
    printf("  get <version> <filename>\n");
    printf("  list <filename>(optional)\n");
}
