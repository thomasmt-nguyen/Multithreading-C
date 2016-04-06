#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define ARR_SIZE 128
#define NUM_THREADS 16 
#define handle_error(err, msg)\
        do{ errno = err; perror(msg); exit(0);} while(0)

void copy(char * file1, char * file2);
void * thread();

struct thread_info{
  
  pthread_t thread_id;
  int thread_num;
  int remain;
  long read_sz;

};


int main(int argv, char ** argc){
  
  clock_t begin, end;
  struct thread_info *tinfo;
  double time_spent;
  int i, j, s, remain;
  long results[ARR_SIZE] = {0};
  long * letters;
  long sz; 
  void * res;
  FILE * fp;
  char * filename = "words.txt";
  
  begin = clock();
  
  /* open file for counting */
  fp = fopen( filename , "r" );
  
  if( fp == NULL)
    perror("file open");

  /* get size of file */
  fseek(fp, 0L, SEEK_END);
  sz = ftell(fp);
  remain = sz - (int)(sz/NUM_THREADS) * NUM_THREADS;
   
  /*Allocate memory for pthread_create() args*/
  tinfo = calloc(NUM_THREADS, sizeof(struct thread_info));
  
  /*create threads*/ 
  for(i = 0; i < NUM_THREADS; i++){  
    /*copy from file and pass info*/
    tinfo[i].read_sz = sz/NUM_THREADS;
    tinfo[i].thread_num = i;
    tinfo[i].remain = 0;
    
    /*give remaining to last thread*/
    if(i == NUM_THREADS - 1)
      tinfo[i].remain = remain;

    /*create threads*/
    s =  pthread_create(&tinfo[i].thread_id, (NULL),
                        &thread, &tinfo[i]);
    
    if( s != 0)
      handle_error(s, "pthread_create");
  }
 
  /*join threads*/
  for(i = 0; i< NUM_THREADS; i++){ 
    s = pthread_join(tinfo[i].thread_id, &res);
     
    if( s!= 0)
      handle_error(s, "pthread_join");

    letters = (long*)res;
    
    /*join results*/
    for(j = 0; j < ARR_SIZE; j++)
      results[j] += letters[j];        
  }
  
  end = clock();

  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  
  /*print results for each ASCII character*/
  for(j = 0; j < 32; j++)
    printf("%ld occurances of 0x%x\n", results[j], j); 
  
  for(j = 32; j < ARR_SIZE - 1; j++)
    printf("%ld occurances of '%c'\n", results[j], j); 
  
  printf("%ld occurances of 0x%x\n", results[j], j);

  printf("time of execution: %f\n", time_spent);

  /*close fp and deallocate*/
  fclose( fp );
  free(tinfo);
  free(letters);
  
  return 0;

}

/*WORKING ON OPENING FILE*/
void * thread(void * arg){
  
  /* FILE * fp; */
  struct thread_info * tinfo = arg;
  long * letter = calloc(ARR_SIZE, sizeof(long));
  char let[1];
  char * filename = "words.txt";
  int count;

  /* open file*/
  FILE * fp = fopen( filename , "r" );
  fseek(fp, tinfo->thread_num * tinfo->read_sz, SEEK_CUR);

  for(count = 0; count < tinfo->read_sz + tinfo->remain; count++){ 
    fread(let, 1, 1, fp);
    letter[ (int)let[0] ]++;
  }

  fclose(fp);

  return letter;
  
}
