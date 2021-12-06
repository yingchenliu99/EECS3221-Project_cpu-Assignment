//Student name: YingChen Liu//
//Student number : //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"

struct node * head;
int total_waiting_time();
int total_burst_time();
int total_response_time();
//use to count the list length//
int length=0;

void add(char *name, int priority, int burst){
   struct task *newTask = malloc(sizeof(struct task));
   newTask->name = name;
   newTask->priority = priority;
   newTask->burst = burst;

   if (head == NULL){
     head = malloc(sizeof(struct node));
   }

   struct node *temp = head;

  //According to the principle of First-Come First-Served Scheduling, we put the Task in order//
   while(temp->next != NULL){
     temp = temp->next;
   }
   insert(&temp->next, newTask);
   //Each time a task is added, the length of the list is increased by 1//
   length++;
  
}

void schedule(){
  struct node *current = head->next;

  int t_w_t = total_waiting_time();
  //turnaround time = waiting time + brust time//
  int t_a_t = total_waiting_time() + total_burst_time();
  int t_r_t = total_response_time();


  while(current != NULL){
    //Run all the brusts of the current Task, and then run the next one//
    run(current->task, current->task->burst);
    delete(&current, current->task);  
  }

  printf("Average waiting time = %.2f\n", (float)t_w_t/(float)length);
  printf("Average turnaround time = %.2f\n", (float)t_a_t/(float)length);
  printf("Average response time = %.2f\n", (float)t_r_t/(float)length);

}

int total_waiting_time()
{
  int waiting_time[length];
  int t_waiting_time=0;

  struct node *last_task = head->next;
  //The total waiting time of First-Come First-Served Scheduling is calculated as follows//
  //The waiting time of P0 is equal to 0, //
  //the waiting time of P1 is equal to the brust of P0 //
  //the waiting time of P2 is equal to the brust of (P0+P1) //
  //the waiting time of P3 is equal to the brust of (P0+P1+P2) ...//
  //We can find the rule, the waiting time of P(n) is equal to the waiting time of P(n-1) plus its brust.//
  for(int i=0;i<length;i++)
  {
    if(i==0){
      waiting_time[0]=0;
    }

    else{
      waiting_time[i] = waiting_time[i-1]+last_task->task->burst;
      t_waiting_time = t_waiting_time+waiting_time[i];
      last_task = last_task->next;
    }
  }
  return t_waiting_time;

}

int total_burst_time(){
  int t_burst_time=0;

  struct node *current = head->next;
  //total brust time is the sum of the brusts of all tasks//
  while(current != NULL){
    t_burst_time=t_burst_time+current->task->burst;
    current = current->next;
  }

  return t_burst_time;

}

//Response_time is the time when the Task first runs//
//The first Task we know is run directly, so its response time must be 0. //
//Because in First-Come First-Served Scheduling, Task is run all the brust at once. //
//The response time of the rest Task must be the completion_time of the previous Task(response time+its brust time)//
int total_response_time(){
  int response_time[length];
  int t_response_time=0;

  struct node *current = head->next;

  for(int i=0;i<length;i++)
  {
     if(i==0){
      response_time[0]=0;
    }
    else{
      response_time[i] = response_time[i-1]+current->task->burst;
      t_response_time = t_response_time+response_time[i];
      current = current->next;
    }
  }

  return t_response_time;

}
