//Student name: YingChen Liu//
//Student number : 217532128//
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
  //According to the Priority_RR principle, we put the tasks into the list in the order of Priority from the largest to the smallest//
   while((temp->next != NULL) && (newTask->priority <= temp->next->task-> priority)){
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
    //When the current Task is the last one, it can directly run through all the Brusts//
    if(current->next == NULL){
      run(current->task, current->task->burst);
      delete(&current, current->task);
    }
    //When the current Task not is the last one//
    else{
      //When the Brust of the current Task is less than QUANTUM, it can directly run through the remaining Brusts//
      if(current->task->burst<=QUANTUM){
       run(current->task, current->task->burst);
       delete(&current, current->task);
      }
      //When the Brust of the current Task is larger than QUANTUM//
      else{
        //When the current Task is not the last Task of the current priority, it can only run QUANTUM number of brusts at a time//
        if(current->next->task->priority==current->task->priority){
          run(current->task, QUANTUM);
          current->task->burst=current->task->burst-QUANTUM;
          add(current->task->name, current->task->priority, current->task->burst);
          //Because each time add() is used, length is added by one. 
          //But in this case it is not a new task that is added, so length cannot be added by one. //
          //So length-- is used to offset the length++ of the add() call.//
          length--;
          delete(&current, current->task);
        }
        //When the current Task is the last Task of the current priority, it can directly run through all the remaining Brusts//
        else{
          run(current->task, current->task->burst);
          delete(&current, current->task);
        }
      }
    }
  }

  printf("Average waiting time = %.2f\n", (float)t_w_t/(float)length);
  printf("Average turnaround time = %.2f\n", (float)t_a_t/(float)length);
  printf("Average response time = %.2f\n", (float)t_r_t/(float)length);


}

//First of all, the principle of priority_rr is to run the Task with the largest priority first, and then run the Task with the second largest// 
//There may be more than one Task equal to the maximum priority, so each Task needs to be run in turn.//
//When the brust of a Task is larger than QUANTUM, only the QUANTUM number of brusts can be run.//
//If the Task's brust is smaller than QUANTUM, then all brusts can be run.//
//If the current Task is the last Task in the same priority case, all the brusts can be run.//
//So my idea is to first find the Task equal to the current maximum priority value and calculate their brust sum.
//Then run the Task equal to the current maximum priority value in the same way as Round Robin principle.
//When the total brust of the current maximum priority value is equal to 0, it means we need to find the second largest priority value Task//
//And so on, after running all the Task//
int total_completion_time(){
  struct node *current = head->next;

  int each_burst[length];
  int total_burst_time=0;
  int each_priority[length];
  int finish_time[length];
  int total_finish_time=0;

  //Record each Task's priority, brust and its finish_time. and the total brust time.//
  for(int i=0;i<length;i++)
  {
    each_priority[i]= current->task->priority;
    each_burst[i]= current->task->burst;
    finish_time[i]=0;
    total_burst_time=total_burst_time+each_burst[i];
    current = current->next;
  }

  //Because the task is ranked by the size of the priority, the first one must be the largest//
  int max_priority=each_priority[0];
  int current_max_priority_total_burst=0;

  //Calculate the sum of the brusts with the same priority as the current task.//
  for(int i=0;i<length;i++){
    if(each_priority[i]==max_priority){
      current_max_priority_total_burst=current_max_priority_total_burst+each_burst[i];
    }
  }

  while(total_burst_time>0){
    //Run while the total brust of the maximum Priority still exists.//
    if(current_max_priority_total_burst>0){
      for(int i=0;i<length;i++){
        //Runing when the value of the current Task's priority is equal to the current maximum priority//
        if(each_priority[i]==max_priority){
          //Run when the current Task's brust is greater than QUANTUM//
          if(each_burst[i]>QUANTUM){
            //Run when the current Task is not the last one of the current maximum priority//
            if(each_burst[i]<current_max_priority_total_burst){
              for(int j=0;j<length;j++){
                if(each_burst[j]>0){
                  finish_time[j]=finish_time[j]+QUANTUM;
                }
              }
            total_burst_time=total_burst_time-QUANTUM;
            current_max_priority_total_burst=current_max_priority_total_burst-QUANTUM;
            each_burst[i]=each_burst[i]-QUANTUM;
            }
            //Run when the current Task is the last one of the current maximum priority//
            else if(each_burst[i]==current_max_priority_total_burst){
              for(int j=0;j<length;j++){
                if(each_burst[j]>0){
                  finish_time[j]=finish_time[j]+each_burst[i];
                }
              }
              total_burst_time=total_burst_time-each_burst[i];
              current_max_priority_total_burst=0;
              each_burst[i]=0;
            }
          } 
          //Run when the current Task's brust is smaller than QUANTUM//
          else if(each_burst[i]<=QUANTUM && each_burst[i]>0){
            for(int j=0;j<length;j++){
              if(each_burst[j]>0){
                finish_time[j]=finish_time[j]+each_burst[i];
              }
            }
            total_burst_time=total_burst_time-each_burst[i];
            current_max_priority_total_burst=current_max_priority_total_burst-each_burst[i];
            each_burst[i]=0;
          }
        }
      }
    }
    //When all brusts of all Tasks equal to the current maximum priority have been run.//
    else if(current_max_priority_total_burst==0){
      //Find the new maximum priority value//
      max_priority=0;
      for(int i=0;i<length;i++){
        if(each_burst[i]>0){
          if(each_priority[i] >= max_priority){
            max_priority=each_priority[i];
          }
        }
      }
    //Calculate the sum of the brusts of the Task equal to the new maximum priority value//
      for(int i=0;i<length;i++){
        if(each_priority[i]==max_priority){
          current_max_priority_total_burst=current_max_priority_total_burst+each_burst[i];
        }
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


int total_response_time(){

  struct node *current = head->next;

  int each_burst[length];
  int each_priority[length];
  int each_response[length];
  int total_response_time=0;
  int last_max_priority;
  int current_max_priority_total_burst=0;

  //First record the brust andpriority of all Task//
  for(int i=0;i<length;i++)
  {
    each_burst[i]= current->task->burst;
    each_priority[i]= current->task->priority;
    each_response[i]=0;
    current = current->next;
  }
  //Because the task is ranked by the size of the priority, the first one must be the largest//
  int max_priority=each_priority[0];
  //Calculate the sum of the brusts with the same priority as the current task.//
  for(int i=0;i<length;i++){
    if(each_priority[i]==max_priority){
      current_max_priority_total_burst=current_max_priority_total_burst+each_burst[i];
    }
  }

  for(int i=0;i<length;i++){
    //The response time of the first Task must be 0.//
    if(i==0){
      each_response[i]=0;
    }
    //In a Task equal to the current maximum priority//
    //its response time must be the response time of the previous Task plus the number of Brusts it run.//
    else if(each_priority[i]==max_priority){
      if(each_burst[i-1]<=QUANTUM){
        each_response[i]=each_response[i-1]+each_burst[i-1];
      }
      else{
        each_response[i]=each_response[i-1]+QUANTUM;
      }
    }
    //After processing all the tasks equal to the current maximum priority//
    //the response time of the first task with the second largest priority must be equal to the sum of the brusts of all the tasks with the previous priority.//
    //Then we calculate the response time of the other Task equal to the second largest priority.//
    //And so on, after running all the Task//
    else{
      each_response[i]=current_max_priority_total_burst;
      last_max_priority=max_priority;
      max_priority=0;
      for(int i=0;i<length;i++){
        if(each_priority[i] >= max_priority && each_priority[i]<last_max_priority){
          max_priority=each_priority[i];
        }
      }
      for(int i=0;i<length;i++){
        if(each_priority[i]==max_priority){
          current_max_priority_total_burst=current_max_priority_total_burst+each_burst[i];
        }
      }
    }

   }

   for(int i=0;i<length;i++)
   {
     total_response_time=total_response_time+each_response[i];
   }

   return total_response_time;
 }
