/************************************************************************************************************
 * File:main.c
 * Author: Anthony(Jinu) Lee
 * Modification history:
 *  1) modified by Anthony(Jinu) Lee
 *      a. modified date: 10-21-2021
 *      b. Modified description: Changed using mutex into using semaphore to control fork variable.
 *  2) modified by Anthony (Jinu) Lee
 *      a. modified date: 10-22-2021
 *      b. Modified description: Added random time generator and changed the data type of the philosopher from
 *      int to struct.
 * Procedures:
 *Main: -It initializes philosopher struct array and fork semaphor. Then it creates multiple philosopher pthread
 *RND : It takes a two range variable and generate a random number within the range.
 *Done: It detects the end of the timer and terminates thread.
 *Think: Increment the philosopher's think amount and time, then it sleep with calculated time.
 *Eat:increment the philosopher's eat amount and time, then it sleep with calculated time.
 *Philosopher: This is a routine of each philosopher thread.
 * **********************************************************************************************************/
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
//This defines the number of philosophers
#define Number 5
/*************************************
 * struct phil
 * Author: Anthony(Jinu) Lee
 * Date: 22 October 2021
 * Description: This contains each philosopher's id, amount of eat and think, and amount of time that took to think and eat.
 * 
 * variable:
 *  id: Each philosopher's unique id.
 *  think_num: The amount that philosopher think.
 *  eat_num: The amount that philosopher eat.
 *  think_time: The amount of total think time done by philosopher
 *  eat_time: The amount of total eat time done by philosopher
 * ***********************************/
struct phil
{
    int id;
    int think_num;
    int eat_num;
    int think_time;
    int eat_time;
};
//This variable declares timer start condition.
sig_atomic_t volatile  over = 0;
//This is a semaphore for seats.
sem_t seat;
//This is a semaphore for forks.
sem_t forks[Number];
//This is a pthread array for philosophers.
pthread_t philosophers[Number];
/*********************************
 * int RND(int a, int b)
 * Author: Anthony(Jinu) Lee
 * Date: 22 october 2021
 * Description: This function takes the range a and b, 
 *              and generate random number fells within that range.
 * parameters:
 *  int a: lower bound
 *  int b: upper bound
 * *******************************/
int RND(int a, int b)
{
    //Generate random variable by using rand function with upper and lower bounds.
    int k = (rand() % (b-a+1))+ a;
    return k;
}
/********************************
 * void done(int a)
 * Author: Anthony(jinu) Lee
 * Date: 22 october 2021
 * Description: This function takes timer 
 *              variable and terminates philosopher threads. 
 *              Then it raise that timer is done.
 * Paramenter:
 *  int a: This is a timer variable.
 * ******************************/
void done(int a)
{
    //Terminates each pthread.
    for(int b = 0; b< Number; b++)
    {
        pthread_cancel(philosophers[b]);
    }
    //marks timer as complete
    over = 1;
}
/**********************************
 * void think(struct phil* a)
 * Author: Anthony(Jinu) Lee
 * Date: 22 october 2021
 * Description: This function takes philosopher
 *              thread and calculates random amount of time to think
 *              then it pauses the thread with given time and increment each
 *              philosopher's  think amount and its time.
 * Parameters:
 *  struct phil* a: A structure that holds philosopher's information.
 * ********************************/
void think(struct phil* a)
{
    //Generates random time
    int o = RND(25,49);
    //Update the time to  milisecond.
    int t_time = o * 1000;
    //pause thread for given amount of time
    usleep(t_time);
    //Increment philosopher's amount of think and it's time taken.
    a->think_num++;
    a->think_time = a->think_time + o;
    
}
/**********************************
 * void eat(struct phil* a)
 * Author: Anthony(Jinu) Lee
 * Date: 22 october 2021
 * Description: This function takes philosopher
 *              thread and calculates random amount of time to eat
 *              then it pauses the thread with given time and increment each
 *              philosopher's  eat amount and its time.
 * Parameters:
 *  struct phil* a: A structure that holds philosopher's information.
 * ********************************/
void eat(struct phil* a)
{
    //Generates random time
    int o = RND(25,49);
    //Update the time to  milisecond.
    int e_time = o*1000;
    //pause thread for given amount of time
    usleep(e_time);
    //Increment philosopher's amount of think and it's time taken.
    a->eat_num++;
    a->eat_time = a->eat_time + o;
}
/**************************************************************
 * void *philosopher(void* num)
 * Author: Anthony(Jinu) Lee
 * Date: 22 october 2021
 * Description: This is a general routine for each philosophers,
 *              it contains take a seat- think-eat-and leave the seat.
 * Parameters:
 *  void * num: This is a vague type which will hold structure of philosopher.
 * ************************************************************/
void* philosopher(void* num)
{
    //Runs loop infinitely.
    while(1) 
    {
        //Converts parameter into struct variable.
        struct phil* q = num;
        //Process think function.
        think(q);
        //occupies seat.
        sem_wait(&seat);
        //Pick up left fork
        sem_wait(&forks[q->id]);
        //Pick up right fork
        sem_wait(&forks[(int)(q->id+1)%Number]);
        //Process eat function
        eat(q);
        //Put down right fork
        sem_post(&forks[(int)(q->id+1)%Number]);
        //Put down left fork.
        sem_post(&forks[q->id]);
        //leaves the seat.
        sem_post(&seat);
    }

}

int main(int argc, char *argv)
{
    //This is a structure array which holds informations of each philosophers
    struct phil Listo[5];
    //Syncs the random number generator with current time.
    srand(time(0));
    //This is used for for loop.
    int a;
    //Temporary variable to hold struct information.
    struct phil q;
    //Setup 300 second timer. When timer is over, it calls done function.
    signal(SIGALRM,done);
    alarm(300);
    //While timer is not over, run the code below.
    while(!over)
    {
    //initialize seat semaphore as number of philosopher-1.
    sem_init(&seat,0,Number-1);
    //initialize the fork semaphores.
    for (a =0; a < Number; a++)
    {
        sem_init(&forks[a],0,1);
    }
    //Initialize each philosopher's default information and creates pthread of each philosopher.
    for (a = 0; a < Number; a++)
    {
        Listo[a].id = a +1;
        Listo[a].think_num = 0;
        Listo[a].eat_num = 0;
        Listo[a].think_time = 0;
        Listo[a].eat_time = 0;
        pthread_create (&philosophers[a], NULL, &philosopher,&Listo[a]);
    }
    //For each philosopher thread, allow process to wait for other process to terminate.
   {
    for (a = 0; a < Number; a++)
    {
        pthread_join(philosophers[a],NULL);
    }
    
   }
    }
    //Outputs result for each professor.
    for (int a = 0; a < Number; a++)
    {
        printf("philosopher %d thought %d this much time over %d and eat %d this much time over %d\n", a,Listo[a].think_num,Listo[a].think_time,Listo[a].eat_num,Listo[a].eat_time);
    }
}
