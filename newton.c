#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <pthread.h>
#define M_PI 3.14159265358979323846


//GLOVAL VARIABLES
int * itarr;
int * carr;
int * items_done;
int maxiter=0, d, nr_threads, exponent;
size_t size;
double complex * roots;
double allerr=1e-3, maxval=1e10;
FILE * fp_conv, * fp_atr;


//FUNCTION DECLARATIONS
double complex intpow(double complex x);
void newt(size_t index, double complex x0);
void *newtwrapper(void * arg);
void *writeppm(void * arg);

int main(int argc, char *argv[]){

  char * ptr;

  for(size_t i = 1; i < argc; ++i){
    if ( strncmp(argv[i],"-t",2)==0 ){
      nr_threads = (int) strtol(argv[i]+2,&ptr,10);
    } else if ( strncmp(argv[i],"-l",2)==0 ){
      size = (size_t) strtol(argv[i]+2,&ptr,10);
    } else {
        d = (int) strtol(argv[i],&ptr,10);
    }
  }
  
  exponent = d-1;
  
  //create threads
  pthread_t threads[nr_threads];
  pthread_t write_thread;
  

  roots = (double complex*) malloc(sizeof(double complex)*size);
  
  for(int i=0; i<d; ++i){
    roots[i] = cos(2*M_PI*i/d)+I*sin(2*M_PI*i/d);
  }

  itarr = (int*) malloc(sizeof(int) * size*size);
  carr = (int*) malloc(sizeof(int) * size*size);
  items_done = (int*) calloc(size*size, sizeof(int));  



  int args[nr_threads];
  for(size_t ix=0; ix<nr_threads; ++ix){
    args[ix] = ix;
    pthread_create(&threads[ix], NULL, newtwrapper,(void*) &args[ix]);
  }
  pthread_create(&write_thread, NULL, writeppm, NULL); 

  for(size_t ix=0; ix<nr_threads; ++ix){
    pthread_join(threads[ix], NULL);
  }
  pthread_join(write_thread, NULL);

  free(carr);
  free(itarr);
  free(items_done);

  return 0;
}


double complex intpow(double complex x){

  double complex result = 1;    
  
  switch(exponent){    

  case 6 :
    return x*x*x*x*x*x;
    
  case 5 :
    return x*x*x*x*x;
  
  case 4 :
    return x*x*x*x;

  case 3 :
    return x*x*x;

  case 2 :
    return x*x;

  case 1 :
    return x;

  case 0 :
    return 1;
    
  default :
    
    for(;;){
      if (exponent & 1)
        result *= x;
      exponent >>= 1;
      if (!exponent)
        break;
      x *= x;
    }  
    
    return result;
  }


}

void newt(size_t index, double complex x0){

  double complex val;  
  
  for(int iteration=0;;++iteration){
    
    if((cabs(x0)-1)<allerr){
      for(int i = 0; i<d; ++i){
        if(cabs(x0-roots[i]) < allerr){
          carr[index] = i;
          itarr[index] = iteration;
          if(iteration>maxiter)
            maxiter = iteration;
          return;
        }
      }
    }

    if(cabs(x0)<allerr || creal(x0)>maxval || cimag(x0)>maxval){
      carr[index] = d;
      itarr[index] = iteration;
      if(iteration>maxiter)
        maxiter = iteration;
      return;
    }
            
    val = intpow(x0);
    x0 = x0 - (val*x0-1)/(d*val);
  }

}

void *newtwrapper(void * arg){

  int thread_id = *((int *) arg);
  
  double max = 2, min = -2;
  double interval = (max-min)/(size-1);
 
  size_t row, col;
  double complex x0;
  


  for(size_t i = thread_id; i<(size*size); i+=nr_threads){
    row = i/size;
    col = i%size;
    x0 = min+col*interval+I*(max-row*interval);
    newt(i, x0);
    items_done[i] = 1;
  }
  
  return NULL;
} 

void *writeppm(void * arg){
  
  char colors[17][12] = {
    "000 000 000",
    "001 000 103",
    "213 255 000",
    "255 000 086",
    "158 000 142",
    "014 076 161",
    "255 229 002",
    "000 095 057",
    "000 255 000",
    "149 000 058",
    "255 147 126",
    "164 036 000",
    "000 021 068",
    "145 208 203",
    "098 014 000",
    "107 104 130",
    "000 000 255",
  };
  
  //OPEN FILES
  char filename_conv[26];
  (void) sprintf(filename_conv, "newton_convergence_x%d.ppm", d);

  fp_conv = fopen(filename_conv, "w");
  
  (void) fprintf(fp_conv, "P2\n%d %d\nXXXX", size, size);

  char filename_atr[25];
  (void) sprintf(filename_atr, "newton_attractors_x%d.ppm", d);
  
  fp_atr = fopen(filename_atr, "w");
  
  (void) fprintf(fp_atr, "P3\n%d %d\n255", size, size);

  //WRITE FILES
  size_t i = 0;
  struct timespec ts = {0,1};
  while(i<(size*size)){

    if(items_done[i]!=0){

      if(i%size==0){
        (void) fprintf(fp_conv, "\n");
        (void) fprintf(fp_atr, "\n");
      }   

      (void) fprintf(fp_conv, "%d ", itarr[i]);
      (void) fprintf(fp_atr, "%s   ", colors[carr[i]%17]);
      ++i;
    } else {
      nanosleep(&ts,NULL);
    }
  }
  
  //CLOSE FILES
  int digits=0, num=size;
  long int position;

  while(num!=0){
    ++digits;
    num /= 10;
  }
  
  position = 5+2*digits;

  (void) fseek(fp_conv, position,SEEK_SET);
  (void) fprintf(fp_conv, "%04d\n", maxiter);
  
  (void) fclose(fp_conv);
  (void) fclose(fp_atr);

  
  return NULL;
}

