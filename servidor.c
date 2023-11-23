//--------- LIBRERIAS ---------//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

//--------- CONSTANTES ---------//
#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MAX_MESSAGES 100
#define MESSAGE_SIZE 256

//--------- ESTRUCTURAS ---------//
//Mensaje publico con nombre de usuario y mensaje
typedef struct {
    char username[50];
    char message[MESSAGE_SIZE];
} PublicMessage;

//Cliente con nombre de usuario y socket asociado
typedef struct {
    char username[50];
    int socket;
} Client;

//Recuento de clientes y mensajes publicos
Client clients[MAX_CLIENTS];
int num_clients = 0;

PublicMessage public_messages[MAX_MESSAGES];
int num_public_messages = 0;


//Declaracion de Mutex para manejar concurrencia
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


//--------- FUNCIONES ---------//
//Funcion para enviar mensaje publico, acceso exclusivo a la lista de mensajes públicos utilizando un mutex
//Si la lista no está llena, agrega el mensaje; de lo contrario, implementa un mecanismo de reemplazo de mensajes más antiguos.
void send_public_message(char *message, char *username) {
    pthread_mutex_lock(&mutex);

    if (num_public_messages < MAX_MESSAGES) {
        strcpy(public_messages[num_public_messages].username, username);
        strcpy(public_messages[num_public_messages].message, message);
    } else {
        int index = num_public_messages % MAX_MESSAGES;
        strcpy(public_messages[index].username, username);
        strcpy(public_messages[index].message, message);
    }
    num_public_messages++;

    // Enviar mensajes a los clientes conectados
    for (int i = 0; i < num_clients; ++i) {
        char message_with_username[BUFFER_SIZE];
        snprintf(message_with_username, BUFFER_SIZE, "%s: %s\n", username, message);
        send(clients[i].socket, message_with_username, strlen(message_with_username), 0);
    }

    pthread_mutex_unlock(&mutex);
}

//Funcion para enviar mensajes publicos a un cliente
//Envía mensajes públicos almacenados al cliente especificado por su socket. Utiliza el mutex tambien.
void send_public_messages_to_client(int client_socket) {
    pthread_mutex_lock(&mutex);

    int start_index = (num_public_messages > MAX_MESSAGES) ? num_public_messages - MAX_MESSAGES : 0;
    for (int i = start_index; i < num_public_messages; ++i) {
        char message_with_username[BUFFER_SIZE];
        snprintf(message_with_username, BUFFER_SIZE, "%s: %s\n", public_messages[i].username, public_messages[i].message);
        send(client_socket, message_with_username, strlen(message_with_username), 0);
    }

    char end_message[] = "END_PUBLIC_MESSAGES\n";
    send(client_socket, end_message, strlen(end_message), 0);

    pthread_mutex_unlock(&mutex);
}

//Funcion para manejar clientes, se ejecuta en un hilo separado para cada cliente conectado al servidor
//Recibe el nombre de usuario del cliente y lo agrega a la lista de clientes conectados
//Recibe comandos del cliente y los procesa
//Si el cliente se desconecta, elimina el cliente de la lista de clientes conectados y limpia recursos
void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char username[BUFFER_SIZE];
    recv(client_socket, username, BUFFER_SIZE, 0);

    Client new_client;
    strcpy(new_client.username, username);
    new_client.socket = client_socket;

    pthread_mutex_lock(&mutex);
    clients[num_clients++] = new_client;
    pthread_mutex_unlock(&mutex);

    printf("New client connected: %s\n", username);

    char buffer[BUFFER_SIZE];
    while (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0) {
        if (strncmp(buffer, "PUBLIC", 6) == 0) {
            send_public_message(buffer + 7, username);
        } else if (strncmp(buffer, "VIEW", 4) == 0) {
            send_public_messages_to_client(client_socket);
        } else {
            printf("Invalid command from client '%s'. Use PUBLIC to send a message or VIEW to see public messages.\n", username);
        }
        //Limpiar el buffer
        memset(buffer, 0, BUFFER_SIZE);
    }

    printf("Client disconnected: %s\n", username);

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; ++i) {
        if (strcmp(clients[i].username, username) == 0) {
            for (int j = i; j < num_clients - 1; ++j) {
                clients[j] = clients[j + 1];
            }
            num_clients--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(client_socket);
    pthread_exit(NULL);
}

//--------- MAIN ---------//
//Funcion principal, crea el socket del servidor y acepta conexiones de clientes
//Crea un hilo separado para cada cliente conectado
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, (void *)&new_socket) != 0) {
            perror("Thread creation failed");
            close(new_socket);
        }

        pthread_detach(tid);
    }

    return 0;
}