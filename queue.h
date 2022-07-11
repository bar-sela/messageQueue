#include <mutex>
#ifndef EX6_QUEUE_H
#define EX6_QUEUE_H



typedef struct pair {
    char str[1024];
    int sendTo;
    int level ;
}pair;
typedef struct node {
    struct node* next = NULL;
    pair paiR ;
}node;

typedef struct queue {
    node *head = NULL, *tail =NULL;
    pthread_t t ;
}queue;

typedef struct activ_object {
    void* (*start_func)(node*);
    void* (*endFunc)(node*);
    queue* q;
    int Qlevel;
    pthread_t t ;
}activ_object;

typedef struct arguments{
    void* ao;
    int num;
}arg;

void* endFunc1(node*);
void* endFunc2(node*);
void* endFunc3(node*);
void* startFunc1(node*);
void* startFunc2(node*);
void* startFunc3(node*);
void* newAO(void* arg);
//void* endFunc3();

void* enQ(void* newNodee,queue* q);
void* deQ(queue* q, int level);
node* create_Node(int sentTo , char* val );
struct queue* createQ(queue* q);
extern activ_object* pActiveObject1;
extern activ_object* pActiveObject2;
extern activ_object* pActiveObject3;

void* destoryQ(queue* q);

#endif //EX6_QUEUE_H

