#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define LOCALHOST "127.0.0.1"

#define HOST 5566

int sock_fd;
pthread_mutex_t mutex;
struct sockaddr_in serverAddr;

void get_string(char* string, int len) {
    fgets(string, len, stdin);

    if (strlen(string) >= len - 1)
        while ((getchar()) != '\n');
}

void * listener(void *arg) {
    char buffer[1024];
    while (recvfrom(sock_fd, buffer, 1024, 0, NULL, NULL) != -1) {
        printf("%s", buffer);
    }
}

void * sender(void *arg) {
    long nBytes;
    socklen_t addr_size = sizeof serverAddr;
    char buffer[1024];
    while (1) {

        get_string(buffer, 1024);

        if (buffer[0] == 'q' && buffer[1] == '\n') {
            puts("Exiting...");
            exit(0);
        }

        nBytes = (long )strlen(buffer) + 1;
        if (sendto(sock_fd, buffer, nBytes, 0, (struct sockaddr *) &serverAddr, addr_size) == -1) {
            perror("sendto error");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    long nBytes;
    char buffer[1024];

    char *ip;

    if (argc < 2) {
        printf("No ip provided! Using localhost: %s\n", LOCALHOST);
        ip = LOCALHOST;
    } else {
        ip = argv[1];
    }

    socklen_t addr_size;

    pthread_mutex_init(&mutex, NULL);

    sock_fd = socket(PF_INET, SOCK_DGRAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(HOST);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    addr_size = sizeof serverAddr;

    printf("Type username: ");
    get_string(buffer, 40);
    buffer[strcspn(buffer, "\n")] = 0;
    nBytes = (long )strlen(buffer) + 1;

    sendto(sock_fd, buffer, nBytes, 0, (struct sockaddr *) &serverAddr, addr_size);

    pthread_t listen, send;

    pthread_create(&send, NULL,sender,NULL);
    pthread_create(&listen, NULL, listener, NULL);
    pthread_join(send, NULL);
    pthread_join(listen, NULL);

    return 0;
}