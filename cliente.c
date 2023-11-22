#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

// ANSI color codes
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

void print_colored_message(const char *message, const char *color)
{
    printf("%s%s%s", color, message, COLOR_RESET);
}

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address / Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        return -1;
    }

    printf("%sConnected to server%s\n", COLOR_GREEN, COLOR_RESET);

    printf("Enter your username: ");
    char username[BUFFER_SIZE];
    fgets(username, BUFFER_SIZE, stdin);
    username[strcspn(username, "\n")] = '\0';
    send(sock, username, strlen(username), 0);

    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("\033[2J\033[H"); // Limpiar la pantalla

        printf("%s----- Menu -----%s\n", COLOR_CYAN, COLOR_RESET);
        printf("%s1. Send Public Message%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("%s2. View Public Messages%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("%s3. Exit%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("Enter your choice: ");

        int choice;
        scanf("%d", &choice);
        getchar(); // Clear input buffer

        switch (choice)
        {
        case 1:
        {
            printf("\n%s----- Send Public Message -----%s\n", COLOR_CYAN, COLOR_RESET);
            printf("Enter message to send publicly: ");
            char message[BUFFER_SIZE];
            fgets(message, BUFFER_SIZE, stdin);
            message[strcspn(message, "\n")] = '\0';

            char public_message[BUFFER_SIZE];
            snprintf(public_message, BUFFER_SIZE, "PUBLIC %s", message);

            send(sock, public_message, strlen(public_message), 0);

            printf("%sMessage sent.%s\n", COLOR_GREEN, COLOR_RESET);
            sleep(1); // Añadir un retraso para permitir la visualización del mensaje enviado
            break;
        }

        case 2:
        {
            printf("\n%s----- View Public Messages -----%s\n", COLOR_CYAN, COLOR_RESET);
            send(sock, "VIEW", strlen("VIEW"), 0);

            int end_of_messages = 0;
            int first_message = 1;

            while (!end_of_messages)
            {
                memset(buffer, 0, BUFFER_SIZE);
                int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_received > 0)
                {
                    char *token = strtok(buffer, "\n");

                    while (token != NULL)
                    {
                        if (strcmp(token, "END_PUBLIC_MESSAGES") == 0)
                        {
                            printf("\n%s------- No More Messages -------%s\n", COLOR_CYAN, COLOR_RESET);
                            printf("\n%sPress any key to continue...%s\n", COLOR_YELLOW, COLOR_RESET);

                            //Tocar boton para end of messages = 1
                            getchar();
                            end_of_messages = 1;
                            break;
                        }
                        else
                        {
                            if (!first_message)
                            {
                                printf("%s\n", token);
                            }
                            else
                            {
                                first_message = 0;
                            }
                        }
                        token = strtok(NULL, "\n");
                    }
                }
                else
                {
                    printf("%sServer disconnected or error in receiving messages.%s\n", COLOR_RED, COLOR_RESET);
                    return -1;
                }
            }
            break;
        }

        case 3:
            printf("\n%sExiting...%s\n", COLOR_YELLOW, COLOR_RESET);
            close(sock);
            return 0;

        default:
            printf("%sInvalid choice. Please enter a number between 1 and 3.%s\n", COLOR_RED, COLOR_RESET);
            break;
        }
    }

    return 0;
}
