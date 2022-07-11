#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <mutex>
#include "queue.h"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold
std::mutex client_lock;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *thread_func(void* new_fd){
    int fd = *(int*)(new_fd);
    char buff[1024];
    bzero(buff,sizeof(buff));
    read(fd, buff,sizeof(buff));
    node* newNode = create_Node(fd,buff);
   // printf("server: the sentdo number is :%d and the string is : %s\n",fd,newNode->paiR.str);
    enQ(newNode, pActiveObject1->q);
    return NULL;

}

int main(void) {
    queue *q1 = createQ(NULL);
    queue *q2 = createQ(NULL);
    queue *q3 = createQ(NULL);
    void *(*pSfuntion1)(node*) = startFunc1;
    void *(*pSfuntion2)(node*) = startFunc2;
    void *(*pSfuntion3)(node*) = startFunc3;
    void *(*pEndFuntion1)(node*) = endFunc1;
    void *(*pEndFuntion2)(node*) = endFunc2;
    void *(*pEndFuntion3)(node*) = endFunc3;
    activ_object ao1{pSfuntion1,pEndFuntion1,q1,1};
    activ_object ao2{pSfuntion2,pEndFuntion2,q2, 2};
    activ_object ao3{pSfuntion3,pEndFuntion3,q3, 3};
    pActiveObject1 = &ao1;
    pActiveObject2 = &ao2;
    pActiveObject3 = &ao3;


    arg arg1, arg2, arg3;
    arg1.ao = &ao1;
    arg1.num = 1;
    arg2.ao = &ao2;
    arg2.num = 2;
    arg3.ao = &ao3;
    arg3.num = 3;

    pthread_create(&ao1.t, NULL, newAO, &arg1);
    pthread_create(&ao2.t, NULL, newAO, &arg2);
    pthread_create(&ao3.t, NULL, newAO, &arg3);

    // Let us create three threads

    sleep(2);
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        printf("server: new fd is= %d\n", new_fd);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, thread_func, &new_fd);
    }
}
