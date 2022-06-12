#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define LOCALHOST "127.0.0.1"

#define HOST 5566

typedef struct Client {
    struct  sockaddr_in address;
    char    username[40];
} client_t;

client_t client_array[100];
int counter = 0;

int clientCompare(struct sockaddr_in client1, struct sockaddr_in client2) {
    if (strncmp
                ((char *) &client1.sin_addr.s_addr, (char *) &client2.sin_addr.s_addr,
                 sizeof(unsigned long)) == 0) {
        if (strncmp((char *) &client1.sin_port, (char *) &client2.sin_port, sizeof(unsigned short))
            == 0) {
            if (strncmp
                        ((char *) &client1.sin_family, (char *) &client2.sin_family,
                         sizeof(unsigned short)) == 0) {

                return 1;
            }
        }
    }
    return 0;
}


int main(int argc, char *argv[]) {

    int serverSocket;
    long nBytes;
    char buffer[1024];

    struct sockaddr_in serverAddr;
    struct sockaddr_in serverStorage;

    socklen_t addr_size;
    char *ip;

    if (argc < 2) {
        printf("No ip provided! Using localhost: %s\n", LOCALHOST);
        ip = LOCALHOST;
    } else {
        ip = argv[1];
    }

    serverSocket = socket(PF_INET, SOCK_DGRAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(HOST);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) {
        perror("bind error");
        exit(1);
    }

    addr_size = sizeof serverStorage;

    while (recvfrom(serverSocket,buffer,1024,0,(struct sockaddr *) &serverStorage,&addr_size) != -1) {
        int number = -1;

        int isFound = 0;
        for (int i = 0; i < counter; ++i) {
            isFound = clientCompare(client_array[i].address, serverStorage);
            if (isFound == 1) {
                number = i;
                break;
            }
        }
        if (isFound == 0) {
            client_array[counter].address = serverStorage;
            strncpy(client_array[counter].username, buffer, 40);
            counter++;
        }

        client_t sent = client_array[number];

        if (number != -1) {
            char buffer2[1024];
            strncpy(buffer2, sent.username, 40);
            strcat(buffer2, ": ");
            strcat(buffer2, buffer);

            nBytes = (long )strlen(buffer2) + 1;
            for (int i = 0; i < counter; ++i) {
                if (clientCompare(sent.address, client_array[i].address) == 0) {
                    serverStorage = client_array[i].address;
                    sendto(serverSocket, buffer2, nBytes, 0, (struct sockaddr *) &serverStorage, addr_size);
                }
            }
        }
    }

    return 0;
}