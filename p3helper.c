/* p3helper.c
 * Thomas Pipitone
 * Program 3
 * CS570
 * Professor Carroll
 * SDSU
 * 18 October 2022
 *
 */
#include "p3.h"
#include <fcntl.h> 
/* You may put any semaphore (or other global) declarations/definitions here: */

#define SHOOTER 1
#define JOGGER 0
#define NONE -1

void close_sems(void); 

sem_t gym_access; 
sem_t j_mutex; //mutex for jogger count 
sem_t s_mutex; //mutex for shooter count
int j_count; // jogger count
int s_count; //shooter count

/* General documentation for the following functions is in p3.h
 *
 */

void initstudentstuff(void) {
	// init gym_access to 1 
	if(sem_init(&gym_access, 1, 1) != 0){ 
		perror("error in gym_access creation");
		exit(-1); 
	}

	// init mutexes to 1
	if(sem_init(&j_mutex, 1, 1) != 0 ){  
		perror("error in jogger mutex creation"); 
		exit(-1); 
	}

	if(sem_init(&s_mutex, 1, 1) != 0){
		perror("error in shooter mutex creation");
		exit(-1);
	}
 	
	s_count = 0; 
	j_count = 0; 
}

/* 
 * PROLOG : giving shooter/jogger permission to enter.  
*/

void prolog(int kind) { 

	if(kind == JOGGER){  
		// mutex so two threads not incrimenting simaltaniously
		sem_wait(&j_mutex); 
		j_count++;
		if(j_count == 1){
			sem_wait(&gym_access); 
		}
		sem_post(&j_mutex); 
		//same standard for shooters, using seperate mutex
	} else if (kind == SHOOTER ) {	
		sem_wait(&s_mutex);
		s_count++;
		if(s_count == 1){
			sem_wait(&gym_access);
		}
		sem_post(&s_mutex); 

	} else {
		fprintf(stderr, "%s", "ERROR: unexpected 'kind' entering gym\n");
		exit(-1);  
	}

}

/* EPILOGUE: shooter/jogger leave */ 


void epilog(int kind) {
	if(kind == JOGGER){	
		CHK(sem_wait(&j_mutex)); 
		j_count--; // jogger leaves	 
		
		if(j_count == 0){ // once the total number of joggers is zero, we give up acces
			
			CHK(sem_post(&gym_access)); 
		} // if the last jogger leaves, open up gym access
		CHK(sem_post(&j_mutex)); 

	} else if ( kind == SHOOTER){
		CHK(sem_wait(&s_mutex));
		s_count--; 	
		
		if(s_count == 0){
			 
			CHK(sem_post(&gym_access)); 
		} // if the last shooter leaves, open up gym access 
		CHK(sem_post(&s_mutex)); 

	} else {
		fprintf(stderr, "%s", "ERROR: unexpected 'kind' exiting gym\n"); 
	}
}  


