#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define RECV_BUF_LEN 8096

void *con_handler(void *arg){
    int cfd = *(int*)arg;
    char *buf = calloc(RECV_BUF_LEN, sizeof(char));
   
    bzero(buf, RECV_BUF_LEN*sizeof(char));
    if(recv(cfd, buf, RECV_BUF_LEN, 0) < 1){
        perror("recv() failed");
        pthread_exit(NULL);
    }

    bzero(buf, RECV_BUF_LEN*sizeof(char));
    //*(buf + RECV_BUF_LEN - 1) = '\0';
    strcpy(buf, "HTTP/1.1 200 OK\r\n\r\nGood Morning, Mothafucka!");
    if(send(cfd, buf, strlen(buf), 0) < 0){
        perror("send() failed");
        pthread_exit(NULL);
    }

    close(cfd);
    free(buf);

    pthread_exit(NULL);
}

int main(int argc, char **argv){
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in *addr = calloc(1, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(42069);
    addr->sin_addr.s_addr = INADDR_ANY;

    if(bind(sfd, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) < 0){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    if(listen(sfd, 100) < 0){
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *client = calloc(1, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    int cfd = 0;

    while(1){
        bzero(client, sizeof(struct sockaddr_in));
        cfd = accept(sfd, (struct sockaddr*)client, &client_addr_len);
        if(cfd < 0){
            perror("accept() failed");
            exit(EXIT_FAILURE);
        }

        pthread_t thread;
        if(pthread_create(&thread, NULL, con_handler, &cfd) != 0){
            perror("pthread_create() failed:");
            exit(EXIT_FAILURE);
        }

        if(pthread_detach(thread) != 0){
            perror("pthread_detahc() failed");
            exit(EXIT_FAILURE);
        }

        char c = fgetc(stdin);
        if (c == 'q') {
            break;
        }
    }

    close(sfd);

    free(client);
    free(addr);

    return 0;
}