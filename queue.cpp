#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>


int x = 0 ;

pthread_mutex_t lock;
pthread_cond_t cond1, cond2, cond3;

activ_object* pActiveObject1;
activ_object* pActiveObject2;
activ_object* pActiveObject3;

void* newAO(void* arg1) {
    struct arguments* a = (struct arguments*)arg1;
    activ_object* current = (activ_object*)a->ao;
    while (1) {
        node* n = (node*)deQ((current->q), a->num);
        sleep(5);
        (node*) current->start_func(n);
        printf("what moved is: string = %s, level = %d\n\n", n->paiR.str, n->paiR.level);
        current->endFunc(n);
        //free(element);
        }
}

void destroyAO(struct activ_object a ){
    destoryQ(a.q);
    pthread_cancel(a.t);
}

void* destoryQ(queue* q){
    pthread_mutex_lock(&lock);
    node* temp = q->head;
    while(q->head != NULL){
        q->head = q->head->next;
        free(temp);
        temp = q->head;
    }
    free(q);
    pthread_mutex_unlock(&lock);
    return  NULL;
}

void* startFunc1(node* n) {
    pair* paiR = &(n->paiR);
    printf("********* stratFunc1 *********\n");
    printf(" the sendTo number is %d, the string is %s\n",n->paiR.sendTo,n->paiR.str);
    printf("Details: string = %s, level = %d\n\n", n->paiR.str, n->paiR.level);
    char* str = paiR->str;
    printf("Got message: %s\n", str);
    int shift = 1 ;
    int i = 0;
    while (str[i] != '\n') {
        if (str[i] >= 'A' && str[i]<= 'Z') {
            char c = str[i] - 'A';
            c += shift;
            c = c % 26;
            str[i] = c + 'A';
        }
        if (str[i] >= 'a' && str[i]<= 'z') {
            char c = str[i] - 'a';
            c += shift;
            c = c % 26;
            str[i] = c + 'a';
        }
        i++;
    }
   // printf("Encrypted message: %s\n", str);
    //printf("**********************************\n");
    return n;
}

void* startFunc2(node* n) {
    printf("********* stratFunc2 **********\n");
    printf(" the sendTo number is %d, the string is %s",n->paiR.sendTo,n->paiR.str);
    pair* paiR = &(n->paiR);
    char* str = paiR->str;
   // printf("Func2, before change:%s\n", str);
    for(int i = 0 ; i < strlen(str)-1; i++){
        if(str[i] >= 'a' && str[i] <= 'z')
            str[i] = str[i] - 32;
        else if(str[i] >= 'A' && str[i] <= 'Z')
             str[i] = str[i] + 32; /* add 32 to str character to change into lowercase */
    }
    return 0;
}

void* startFunc3(node* n){
  //  printf("**********startfunc3************\n");
    //printf(" the sendTo number is %d, the string is %s\n",n->paiR.sendTo,n->paiR.str);
    pair* paiR = &(n->paiR);
    write(paiR->sendTo, paiR->str, sizeof(paiR->str) );
    return 0;
}

void* endFunc1(node* n) {
    // pair* paiR = &(n->paiR);
    // node* newNode = create_Node(paiR->sendTo,paiR->str); ///CREATE NODE WITH  COPY OF PAIR(DIFFERENT PAIR IN MEMORY )
   // printf("********************************** end Func*************** 1\n ");
  //  printf("endFunc1 --> String = %s, Level = %d\n", n->paiR.str, n->paiR.level);
    enQ(n,pActiveObject2->q);
    return 0;
}

void* endFunc2(node* n) {
   // printf("***************************end Func 2***********\n ");
    pair* paiR = &(n->paiR);
    enQ(n,pActiveObject3->q);
    return 0;
}

void* endFunc3(node* n) {
    pair* paiR = &(n->paiR);
  return NULL;
}


struct queue* createQ(queue* q)
{
    pthread_mutex_lock(&lock);
    q = (struct queue*)malloc(sizeof(struct queue));
    if(!q) {
        printf("failed with malloc\n");
        exit(0);
    }
    q->head = NULL;
    q->tail = NULL;
    pthread_mutex_unlock(&lock);
    return q;
}
//////////////////////////// ququqe fucntions :

node* create_Node(int sentTo, char* val){
    node *newNode = (node *) malloc(sizeof(node));
    if(!newNode) {
        printf("malloc failed\n");
        exit(1);
    }
    strcpy(newNode->paiR.str, (char*)val);  ////////// copy to the given string
    newNode->paiR.sendTo = sentTo;
    newNode->paiR.level = 0 ;
    //std::cout<< << std:: endl;
    return newNode;
}


// The function to add a key k to q
void* enQ(void* Nnode, queue* q) {
 //   printf("***** enQ --> String = %s, Level = %d\n\n", ((node*)Nnode)->paiR.str, ((node*)Nnode)->paiR.level);
    //printf("enQ\n");
    if(pthread_mutex_lock(&lock) != 0 )
        printf("failed lock\n");
    node* newNode = (node*) Nnode;
    newNode->paiR.level++; ////// LEVEL UP !
    int level = newNode->paiR.level;
  //  printf("enQ but now is: --> String = %s, Level = %d\n\n", ((node*)Nnode)->paiR.str, ((node*)Nnode)->paiR.level);
    // printf("level is: %d\n", level);
    // If queue is empty, then new node is front and rear both
    if (q->head == NULL) {
        q->head = newNode;
        q->tail = newNode;
    }
    else {
        printf("is not null ");
        // Add the new node at the end of queue and change rear
        q->tail->next = newNode;
        q->tail = q->tail->next;
        sleep(1);
    }

    if(level == 1){pthread_cond_signal(&cond1);}
    if(level == 2){pthread_cond_signal(&cond2);}
    if(level == 3){pthread_cond_signal(&cond3);}

    pthread_mutex_unlock(&lock);
    return NULL;
}

// Function to remove a key from given queue q
void* deQ(queue* q, int indexQ){
  //  printf("******** deQueue ********\n");
    pthread_mutex_lock(&lock);
    // If queue is empty, return NULL.
  //  printf("index is = %d, if its null go to sleep!\n", indexQ);
    if(q->head == NULL) {  ////////// while!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (indexQ == 1) { pthread_cond_wait(&cond1, &lock); }
        if (indexQ == 2) { pthread_cond_wait(&cond2, &lock); }
        if (indexQ == 3) { pthread_cond_wait(&cond3, &lock); }
    //    printf("wake up queue number %d\n", indexQ);
    }
    node* element = q->head;
    //printf("last element in queue is : %s\n",(char*)element->str);
    q->head = q->head->next;
    if(q->head == NULL)   /// pointing both to last element
        q->tail= NULL;
    pthread_mutex_unlock(&lock);

    return element;
}

void* destroyQ(queue* q){
    pthread_mutex_lock(&lock);
    node* temp = q->head;
    while(q->head != NULL){
        q->head = q->head->next;
        free(temp);
        temp = q->head;
    }
    free(q);
    pthread_mutex_unlock(&lock);
    return  NULL;
}

void *myThreadFun(void *vargp) {
    // Store the value argument passed to this thread
    int *myid = (int *) vargp;

    // Let us create a static variable to observe its changes
    static int s = 0;

    // Change static and global variables
    ++s;

    return 0;
}


