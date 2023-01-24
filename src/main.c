// GNU GPLv3 License at the bottom of this file
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
    char *buf = calloc(RECV_BUF_LEN, sizeof(char)); // ! TODO: create separate buffer for sending

    bzero(buf, RECV_BUF_LEN*sizeof(char));
    if(recv(cfd, buf, RECV_BUF_LEN, 0) < 1){
        perror("recv() failed");
        pthread_exit(NULL);
    }

    bzero(buf, RECV_BUF_LEN*sizeof(char));
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
    // Initialize socket and socket file descriptor(sfd)
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    // Create and initialize server address
    struct sockaddr_in *addr = calloc(1, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET; addr->sin_port = htons(42069); addr->sin_addr.s_addr = INADDR_ANY;

    // bind socket to address specified in addr struct
    if(bind(sfd, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) < 0){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // listen to incoming connections
    if(listen(sfd, 100) < 0){
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    // client addresses will be stored here
    struct sockaddr_in *client = calloc(1, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    // handle connections in a loop
    while(1){
        // set client to 0
        bzero(client, sizeof(struct sockaddr_in));
        // accept connection from client:
        //      client address is stored within client
        //      cfd holds the file descriptor
        int cfd = accept(sfd, (struct sockaddr*)client, &client_addr_len);
        if(cfd < 0){
            perror("accept() failed");
            exit(EXIT_FAILURE);
        }

        // create a separate thread for every connection
        pthread_t thread;
        if(pthread_create(&thread, NULL, con_handler, &cfd) != 0){
            perror("pthread_create() failed:");
            exit(EXIT_FAILURE);
        }
        // detach thread from main process
        if(pthread_detach(thread) != 0){
            perror("pthread_detahc() failed");
            exit(EXIT_FAILURE);
        }

        // quit once 'q' or 'Q' is received
        char c = fgetc(stdin);
        if (c == 'q' || c == 'Q') {
            break;
        }
    }

    close(sfd);

    free(client);
    free(addr);

    return 0;
}
/**
hyper - small yet fast webserver with multithreading
    Copyright (C) 2023 Simon Peter Pflaumer aka. spflaumer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
**/
