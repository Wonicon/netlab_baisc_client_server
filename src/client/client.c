#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const char *SERVER_IP = "114.212.191.33";
const unsigned short SERVER_PORT = 4321;

int main(void)
{
    struct sockaddr_in client_address = {};
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    client_address.sin_port = htons(SERVER_PORT);

    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket_fd == -1) {
        perror("Failed to create socket for client");
    }

    if (connect(client_socket_fd, (struct sockaddr *)&client_address, sizeof(client_address))) {
        perror("Failed to connect to server");
    }
}
