/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: <Yutian Jing>
//McGill ID: <260680087>

 

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

sem_t sem;

int BUFFER_SIZE = 100; //size of queue

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{
    return ((queue->size ) >= queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    //printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print(struct Queue* queue){
    if (queue->size == 0){
        return;
    }
    
    for (int i = queue->front; i < queue->front +queue->size; i++){
        
        printf(" Element at position %d is %d \n ", i % (queue->capacity ), queue->array[i % (queue->capacity)]);
    }
    
}

struct Queue* queue;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void* FnAirplane(void* cl_id)
{
    int thread_numb = *( (int*) cl_id );
    int value;
    while(1){
        value = rand() % (10 + 1 - 5) + 5;
        sem_wait(&sem);
        printf("Airplane %d arrives with %d passengers\n", thread_numb, value);
        int i;
        for(i = 0; i < value; i++){
            if(!isFull(queue)){
                enqueue(queue, 1*1000000 + thread_numb*1000 + i);
                printf("Passenger 1%03d%03d of airplane %d arrives to platform\n", thread_numb, i, thread_numb);
            } else{
                printf("Platform is full: Rest of passengers of plane %d take the bus\n", thread_numb);
            }
        }
        sem_post(&sem);
        sleep(1);
    }
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void* pr_id)
{
    int thread_numb = *( (int*) pr_id );
    int value;
    while(1){
        value = 1;
        sem_wait(&sem);
        printf("Taxi driver %d arrives\n", thread_numb);
        if(!isEmpty(queue)){
            //dequeue(queue);
            printf("Taxi driver %d picked up client %d from the platform\n", thread_numb, dequeue(queue));
        } else{
            printf("Taxi driver %d waits for passengers to enter the platform\n", thread_numb);
        }
        sem_post(&sem);
	int sleep = rand() % (500000 + 1 - 166667) + 166667;
        usleep(sleep);
    }
}

int main(int argc, char *argv[])
{

  int num_airplanes;
  int num_taxis;

  num_airplanes=atoi(argv[1]);
  num_taxis=atoi(argv[2]);
  
  printf("You entered: %d airplanes per hour\n", num_airplanes);
  printf("You entered: %d taxis\n", num_taxis);
  
  
  //initialize queue
  queue = createQueue(BUFFER_SIZE);
  
  //declare arrays of threads and initialize semaphore(s)
  pthread_t airThread[num_airplanes];
  pthread_t taxiThread[num_taxis];
  //sem_init(&full_sem, 0, BUFFER_SIZE);
  sem_init(&sem, 0, 1);

  //create arrays of integer pointers to ids for taxi / airplane threads
  int *taxi_ids[num_taxis];
  int *airplane_ids[num_airplanes];

  //create threads for airplanes
  for(int i = 0; i < num_airplanes; i++){
    airplane_ids[i] = malloc(sizeof(int));
    *airplane_ids[i] = i;
    // Print an error if one occurs
    if (pthread_create(&airThread[i], NULL, FnAirplane, airplane_ids[i]) != 0 ){
        printf("Error with pthread create for plane %i\n", i);
    }
  }

  //create threads for taxis
  for(int i = 0; i < num_taxis; i++){
    taxi_ids[i] = malloc(sizeof(int));
    *taxi_ids[i] = i;
    // Print an error if one occurs
    if (pthread_create(&taxiThread[i], NULL, FnTaxi, taxi_ids[i]) != 0 ){
        printf("Error with pthread create for taxi %i\n", i);
    }
  }

  pthread_exit(NULL);
}
