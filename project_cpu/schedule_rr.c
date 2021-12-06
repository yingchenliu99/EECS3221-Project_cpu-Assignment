//Student name: YingChen Liu//
//Student number : //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"

struct node * head;
int total_completion_time();
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

  //According to the Round Robin principle, we still put the tasks in order.//
   while(temp->next != NULL){
     temp = temp->next;
   }

   insert(&temp->next, newTask);
   //Each time a task is added, the length of the list is increased by 1//
   length++;

}

void schedule(){
  struct node *current = head->next;

  //Waiting time  = Turn Around Time - Brust Time =(Completion Time - Arrivial time) - Brust Time//
  int t_w_t = total_completion_time()-total_burst_time();
  //Turn Around Time = Completion Time - Arrivial time//
  //Here the Arrivial time of all Tasks is 0//
  int t_a_t = total_completion_time();
  int t_r_t = total_response_time();


  while(current != NULL){
    //If the current Task's brust is smaller than QUANTUM, then it can run through all the brusts.//
    if(current->task->burst<=QUANTUM){
      run(current->task, current->task->burst);
      delete(&current, current->task);
    }
    //If the current Task's brust is greater than QUANTUM, then it can only run QUANTUM number of brusts//
    else{
      run(current->task, QUANTUM);
      current->task->burst=current->task->burst-QUANTUM;
      add(current->task->name, current->task->priority, current->task->burst);
      //Because each time add() is used, length is added by one. 
      //But in this case it is not a new task that is added, so length cannot be added by one. //
      //So length-- is used to offset the length++ of the add() call.//
      length--;
      delete(&current, current->task);
    }

  }
  printf("Average waiting time = %.2f\n", (float)t_w_t/(float)length);
  printf("Average turnaround time = %.2f\n", (float)t_a_t/(float)length);
  printf("Average response time = %.2f\n", (float)t_r_t/(float)length);


}

int total_completion_time(){
  struct node *current = head->next;

  int each_burst[length];
  int total_burst_time=0;
  int finish_time[length];
  int total_finish_time=0;

  //First record the brust of each task and set their completion time to 0 first.//
  for(int i=0;i<length;i++)
  {
    each_burst[i]= current->task->burst;
    finish_time[i]=0;
    total_burst_time=total_burst_time+each_burst[i];
    current = current->next;
  }

  //If total_burst_time is not 0, loop repeatedly//
  //My idea is that, when the current Task is run. //
  //The completion time of this Task and all remaining Tasks must increase the number of bursts run by this Task.//
  while(total_burst_time>0){
    for(int i=0;i<length;i++){
     //Here there are two cases, one is the current Task's brust is greater than QUANTUM, and the other is less than QUANTUM//
      if(each_burst[i]<=QUANTUM && each_burst[i]>0){
        for(int j=0;j<length;j++){
          if(each_burst[j]>0){
            finish_time[j]=finish_time[j]+each_burst[i];
          }
        }
        total_burst_time=total_burst_time-each_burst[i];
        each_burst[i]=0;
      }

      else if(each_burst[i]>QUANTUM){
        for(int j=0;j<length;j++){
          if(each_burst[j]>0){
           finish_time[j]=finish_time[j]+QUANTUM;
          }
        }
        total_burst_time=total_burst_time-QUANTUM;
        each_burst[i]=each_burst[i]-QUANTUM;
      }
    }
  }

  for(int i=0;i<length;i++){
    total_finish_time=total_finish_time+finish_time[i];
  }

  return total_finish_time;

}
  
//total brust time is the sum of the brusts of all tasks//
int total_burst_time(){
  struct node *current = head->next;

  int each_burst[length];
  int total_burst_time=0;

  for(int i=0;i<length;i++)
  {
    each_burst[i]= current->task->burst;
    total_burst_time=total_burst_time+each_burst[i];
    current = current->next;
  }
  return total_burst_time;
}

//Response_time is the time when the Task first runs//
//The first Task we know is run directly, so its response time must be 0. //
//Because in Round Robin Scheduling, Task is run sequentially//
//However, the brust of Task is not run at once in Round Robin Scheduling//
//If Task's brust is greater than QUANTUM, then run QUANTUM number of brusts, if less than QUANTUM then run all brusts //
//So the response time of the remaining Task is, the response time of the previous Task plus the number of brusts it has run//
int total_response_time(){

  struct node *current = head->next;

  int each_burst[length];
  int each_response[length];
  int total_response_time=0;

  for(int i=0;i<length;i++)
  {
    each_burst[i]= current->task->burst;
    each_response[i]=0;
    current = current->next;
  }


  for(int i=0;i<length;i++){
    if(i==0){
      each_response[0]=0;
    }

    else{
      if(each_burst[i-1]<=QUANTUM){
        each_response[i]=each_response[i-1]+each_burst[i-1];
      }

      else{
        each_response[i]=each_response[i-1]+QUANTUM;
      }
    }

    total_response_time=total_response_time+each_response[i];
  }

   return total_response_time;
 }



