/**
 * @file
 * @brief Programa servidor
 * 
 * Este archivo contiene la implementación de un servidor que acepta conexiones de clientes y maneja la comunicación con ellos en hilos separados.
 * 
 * @details El servidor utiliza sockets para la comunicación y crea un nuevo hilo para cada cliente que se conecta. 
 * El servidor puede manejar múltiples clientes de manera concurrente.
 * El servicdor maneja peticiónes de adición, obtención y listado de archivos en un repositorio.
 * 
 * @version 1.0
 * 
 * @section LICENSE
 * MIT License
 * 
 * @section AUTHOR
 * Julian David Meneses <juliandavidm@unicauca.edu.co>
 */
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "versions.h"
#include <limits.h>

#define MAX_THREADS 100 ///< Máximo número de hilos que se pueden manejar
/**
 * @brief Inicializa el servidor creando el directorio de versiones si no existe
 */
void initialize_server();

/**
 * @brief Manejador de señales
 * 
 * Esta función se llama cuando se recibe una señal SIGINT o SIGTERM.
 * Se encarga de cerrar el servidor de manera ordenada.
 * 
 * @param signo Número de la señal recibida 
 */
void sig_handler(int signo);

/**
 * @brief Manejador de clientes
 * 
 * Esta función se ejecuta en un hilo separado para cada cliente que se conecta al servidor.
 * Se encarga de recibir y enviar saludos al cliente.
 * 
 * @param arg Puntero al socket del cliente
 * @return void* 
 */
void * client_handler(void * arg);

/**
 * @brief Recibir estructura
 * 
 * Esta función se encarga de recibir una estructura de un socket.
 * 
 * @param sockfd Socket del que se va a recibir la estructura
 * @param struct_ptr Puntero a la estructura donde se va a guardar la información
 * @param struct_size Tamaño de la estructura
 * @return ssize_t Cantidad de bytes leidos
 */
ssize_t recvs(int sockfd, void *struct_ptr, size_t struct_size) ;

/** 
 * @brief Estructura de Gestion de hilos
 * Guarda la información necesaria para gestionar los hilos del servidor.
*/
typedef struct {
    int thread_count; // Cantidad de hilos actauales
    int *threads; // Arreglo de sockets de hilos
} sserver_handler;

sserver_handler *server_handler; // Estructura de gestión de hilos
int server_socket; // Socket del servidor

int main(int argc, char *argv[])
{
    initialize_server();
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }   

    struct stat s;

        //Crear el directorio ".versions/" si no existe
    #ifdef __linux__
        mkdir(VERSIONS_DIR, 0755);
    #elif _WIN32
        mkdir(VERSIONS_DIR);
    #endif

	// Crea el archivo .versions/versions.db si no existe
	//if (stat(VERSIONS_DB_PATH, &s) != 0) {
	//	creat(VERSIONS_DB_PATH, 0755);
	//}

    struct sockaddr_in server_addr;
    server_handler = malloc(sizeof *server_handler);
    int port = atoi(argv[1]); 

    //0. Instalar los manejadores SIGINT, SIGTERM
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    //1. Obtener un conector
    if (!server_handler) {
        perror("Error allocating memory for server handler");
        exit(EXIT_FAILURE);
    }

    server_handler->thread_count = 0;
    server_handler->threads = malloc(MAX_THREADS*sizeof(int)); // Reservar memoria para los hilos
    if (!server_handler->threads) {
        perror("Error allocating memory for threads");
        exit(EXIT_FAILURE);
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    //2. Asociar una direccion al conector -bindb
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Escucha en todas las interfaces de red disponibles

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {// Asociar el socket a la dirección
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_THREADS) == -1) { // Escuchar conexiones
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a client...\n");
    while (1) // Aceptar conexiones
    {
        struct sockaddr_in client_addr; // Dirección del cliente
        socklen_t clilen = sizeof(struct sockaddr_in); // Tamaño de la dirección del cliente
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &clilen); // Aceptar la conexión
        printf("(después de aceptar la conexión)\n");

        if (client_socket < 0) { // Verificar errores
            perror("Error accepting connection");
            continue;
        }

        printf("Client %d connected\n", client_socket);
        int *client_socket_ptr = malloc(sizeof(int));
        if (!client_socket_ptr) { // Verificar errores
            perror("Error allocating memory");
            close(client_socket);
            continue;
        }

        *client_socket_ptr = client_socket;
        pthread_t thread_id;

        if (pthread_create(&thread_id, NULL, client_handler, (void *) client_socket_ptr) < 0) {
            perror("Error creating thread");
            close(client_socket);
            free(client_socket_ptr);
            continue;
        }

        server_handler->threads[server_handler->thread_count++] = client_socket;
        pthread_detach(thread_id);
    }
}

void sig_handler(int signo) {
    printf("Shutting down server...\n");
    for (int i = 0; i < server_handler->thread_count; i++) {
        printf("Closing client %d\n", server_handler->threads[i]);
        close(server_handler->threads[i]);
    }
    close(server_socket);
    free(server_handler->threads);
    free(server_handler);
    exit(EXIT_SUCCESS);
}

void * client_handler(void * arg)
{
    printf("estoy en client handler");
    int client_socket = *(int *)arg; // Socket del cliente
    free(arg); // Liberar memoria del argumento
    operation_type op_type ; // Codigo de operacion
    return_code result; // Resultado de la operacion
    ssize_t nread; // Cantidad de bytes leidos
    sadd sadd_request; // Estructura de solicitud de adición

    // Leer el nombre de usuario al conectarse
    nread = recv(client_socket, sadd_request.username, sizeof(sadd_request.username), 0);
    if (nread <= 0) {
        printf("Error reading username or client disconnected.\n");
        close(client_socket);
        return NULL;
    }

    // Generar la ruta de la base de datos del usuario
    char db_path[PATH_MAX];
    get_user_db_path(sadd_request.username, db_path, sizeof(db_path));
    
    printf("antes de");
    // Verificar si el archivo de base de datos del usuario existe, si no, crearlo
    struct stat st;
    if (stat(db_path, &st) != 0) { // Si el archivo no existe
        FILE *fp = fopen(db_path, "wb"); // Crear el archivo en modo binario
        if (fp) {
            printf("Database file %s created for user %s.\n", db_path, sadd_request.username);
            fclose(fp);
        } else {
            perror("Error creating user database file");
            close(client_socket);
            return NULL;
        }
    }
    printf("despues de");

    while(1) {
        nread = recv(client_socket,&op_type, sizeof(operation_type), 0); // Recibir el codigo de operacion
        if (nread < 0) {
            printf("Error reading operation type\n");
            break;
        }

        if (nread == 0) {
            printf("Client %d disconnected\n", client_socket);
            break;
        }

        switch (op_type)
        {
            case ADD:
                memset(&sadd_request, 0, sizeof(sadd));
                if((nread = recvs(client_socket, &sadd_request, sizeof(sadd))) < 0) {
                    perror("Error reading ADD request");
                    continue;
                }

                result = add(client_socket, &sadd_request); // Realizar la operación de adición
                if (result == VERSION_ALREADY_EXISTS)
                {
                    printf("Client %d requested ADD operation with an existing version\n", client_socket);
                    fake_local_copy(client_socket); // Simular la copia del archivo
                }
                else if (result == VERSION_ERROR)
                    printf("Client %d requested ADD operation but an error occurred\n", client_socket);
                else
                    printf("Client %d requested ADD operation and it was successful\n", client_socket);

                break;

            case GET:
                sget sget_request; // Estructura de solicitud de obtención
                nread = recv(client_socket, &sget_request, sizeof(sget),0); // Recibir la solicitud de obtención
                if(nread != sizeof(sget)){
                    perror("Error reading GET request");
                    break;
                }

                printf("Client %d requested GET operation\n", client_socket);

                result = get(client_socket, &sget_request); // Realizar la operación de obtención
                if (result == VERSION_NOT_FOUND)
                    printf("Client %d requested GET operation with a non-existing version\n", client_socket);
                else
                    printf("Client %d requested GET operation and it was successful\n", client_socket);

                break;

            case LIST:
                slist slist_request; // Estructura de solicitud de listado
                nread = recv(client_socket, &slist_request, sizeof(slist),0); // Recibir la solicitud de listado
                if(nread != sizeof(slist)){
                    perror("Error reading LIST request");
                    break;
                }

                printf("Client %d requested LIST operation\n", client_socket);

                list(client_socket, &slist_request); // Realizar la operación de listado
                printf("Client %d requested LIST operation and it ends\n", client_socket);
                break;

            default:
                printf("Client %d requested an unknown operation (%d)\n", client_socket, op_type);
                continue;
        }    
    }
    close(client_socket);
}

ssize_t recvs(int sockfd, void *struct_ptr, size_t struct_size) {
    ssize_t nread = 0;
    ssize_t n = 0;
    while (nread < struct_size) {
        n = recv(sockfd, struct_ptr + nread, struct_size - nread, 0);
        if (n < 0) {
            perror("Error reading from socket");
            return n;
        }
        nread += n;
    }
    return nread == struct_size ? nread : -1;
}

void initialize_server() {
    struct stat st = {0};
    if (stat(".versions", &st) == -1) {
        mkdir(".versions", 0700);
    }
}