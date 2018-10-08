#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <complex.h>
#include <math.h>
#include <ctype.h>

void check_first_input(char *argv[]); void check_second_input(char *argv[]); void check_third_input(char *argv[]);
double complex newtons_method(double complex, int pol_degree, long ** matrix_pp, int a, int b, complex double * roots);
double complex multiplication_complex(double complex complex_nbr, int pol_degree);
void generate_poly_roots(double complex * roots, int pol_degree);

// creating an arg pointer
struct arg_pointer {
  int start_point;
  int end_point;
  int pol_degree;
  long ** count_matrix;
  complex double ** answer_matrix;
  int size_matrix;
  double sm;
  int * checkJobDone;
  complex double * roots;
};

struct arg_for_printer {
  int * checkJobDone;
  char file_name[32];
  char file_name2[32];
  long ** count_matrix;
  complex double ** answer_matrix;
  int size_matrix;
};

// Creating the function for the pthreads
void *newton_thread(void *arg){
  struct arg_pointer *arg_s = (struct arg_pointer *)arg;
  int size_matrix1 = arg_s->size_matrix;
  int end_point1 = arg_s->end_point;
  int start_point1 = arg_s->start_point;
  int pol_degree1 = arg_s->pol_degree;
  double sm = size_matrix1;
  long **count_matrix1= arg_s->count_matrix;
  
  for (int i= start_point1; i<=end_point1; i++){
    for (int j=0; j<size_matrix1; j+=4){
      complex double cpx_input = (4.0*i/sm-2.0) + (4.0*j/sm-2.0)*I; 
      arg_s->answer_matrix[i][j] = newtons_method(cpx_input, pol_degree1, count_matrix1, i, j, arg_s->roots);

      complex double cpx_input1 = (4.0*i/sm-2.0) + (4.0*(j+1)/sm-2.0)*I; 
      arg_s->answer_matrix[i][j+1] = newtons_method(cpx_input1, pol_degree1, count_matrix1, i, j+1, arg_s->roots);

      complex double cpx_input2 = (4.0*i/sm-2.0) + (4.0*(j+2)/sm-2.0)*I; 
      arg_s->answer_matrix[i][j+2] = newtons_method(cpx_input2, pol_degree1, count_matrix1, i, j+2, arg_s->roots);

      complex double cpx_input3 = (4.0*i/sm-2.0) + (4.0*(j+3)/sm-2.0)*I; 
      arg_s->answer_matrix[i][j+3] = newtons_method(cpx_input3, pol_degree1, count_matrix1, i, j+3,  arg_s->roots);
   
    }
    arg_s->checkJobDone[i]=1;
  }
  pthread_exit(0);
}
 
void *write_to_file(void *argP){
  struct arg_for_printer*arg_p = argP;
  int iCounter = 0;

  FILE * fp = fopen(arg_p->file_name,"w");
  fprintf(fp, "P6\n%d %d\n255\n", arg_p->size_matrix, arg_p->size_matrix);

   FILE * fp2 = fopen(arg_p->file_name2,"w");
   fprintf(fp2, "P6\n%d %d\n255\n", arg_p->size_matrix, arg_p->size_matrix);
     
  while(iCounter < arg_p->size_matrix){
    if (arg_p->checkJobDone[iCounter] == 1) {
      
      for (size_t ix=0; ix<arg_p->size_matrix; ++ix){
	
	static unsigned char color[3];
	color[0] = round(cabs((1-creal(arg_p->answer_matrix[iCounter][ix]))/2.0) * 250.0);
	color[1] = round(cabs((1-cimag(arg_p->answer_matrix[iCounter][ix]))/2.0) * 250.0);
	color[2] = round(cabs((creal(arg_p->answer_matrix[iCounter][ix])+cimag(arg_p->answer_matrix[iCounter][ix]))/2.0) *250.0);
	fwrite(color, 1, 3, fp);	
	}
     
      for (size_t ix=0; ix<arg_p->size_matrix; ++ix){
	static unsigned char color2[3];
	color2[0] = 255.0 * arg_p->count_matrix[iCounter][ix]/50.0;
	color2[1] = 180;
	color2[2] = 180;
	fwrite(color2, 1, 3, fp2);	
      }
      iCounter+=1;
    }
  
  }
  fclose(fp);
  fclose(fp2);

  pthread_exit(0);
}
  

int nbr_threads, size_matrix, pol_degree;

int main (int argc, char * argv[]){

  check_first_input(argv); check_second_input(argv); check_third_input(argv);

  // Allocating space for  different things that needs to be saved.
  // Allocatng a global matrix that can store all the answers from threads. 
  complex double * answer_pointer =  malloc(sizeof(complex double) * size_matrix*size_matrix);
  complex double ** answer_matrix =  malloc(sizeof(complex double) * size_matrix);
  for (size_t ix=0, jx=0; ix<size_matrix; ++ix, jx+=size_matrix){
    answer_matrix[ix] = answer_pointer +jx;
  }

  long * count_pointer = malloc(sizeof(long) * size_matrix*size_matrix);
  long ** count_matrix = malloc(sizeof(long) * size_matrix);
  for (size_t ix=0, jx=0; ix<size_matrix; ++ix, jx+=size_matrix){
    count_matrix[ix] = count_pointer +jx;
  }

  // Creating file name for ppm pictures
  char fn1[32]; char file_name[] = "newton_convergence_x"; char file_name2[] = "newton_attractors_x";  char fn2[] = ".ppm";
  strcpy(fn1, argv[3]);  strcat(fn1, fn2);
  strcat(file_name, fn1); strcat(file_name2, fn1);

  // Creating the strucs args pointer for thread funtion
  struct arg_pointer args[nbr_threads];
  int * checkJobDone = malloc(sizeof(int) * size_matrix);
  for (int i=0; i<size_matrix; i++){
    checkJobDone[i]=0;
  }

  complex double * roots = malloc(sizeof(complex double) *pol_degree);
  
  generate_poly_roots(roots, pol_degree);

  struct arg_for_printer argPrinter;
  argPrinter.answer_matrix = answer_matrix;
  argPrinter.count_matrix = count_matrix;
  strcpy(argPrinter.file_name,file_name);
  strcpy(argPrinter.file_name2,file_name2);
  argPrinter.size_matrix = size_matrix;
  argPrinter.checkJobDone = checkJobDone;

  
  // Creating threads for faster computations
  pthread_t tid[nbr_threads];
  pthread_t tidPrinter;
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  
  pthread_create(&tidPrinter, &attr, write_to_file, &argPrinter);

  int nbrCalculationsPerThread = size_matrix/nbr_threads;

  for (int i=0; i<nbr_threads; i++){
    args[i].pol_degree = pol_degree;
    args[i].count_matrix = count_matrix;
    args[i].answer_matrix = answer_matrix;
    args[i].size_matrix = size_matrix;
    args[i].checkJobDone = checkJobDone;
    args[i].start_point=i*nbrCalculationsPerThread;
    args[i].end_point=(i+1)*nbrCalculationsPerThread-1;
    args[i].roots = roots;
    
    if ( i+1 == nbr_threads) {
      args[i].end_point=(i+1)*nbrCalculationsPerThread-1+size_matrix%nbr_threads;
    }
    pthread_create(&tid[i], &attr, newton_thread, &args[i]);
  }

  pthread_join(tidPrinter, NULL);
  
  for (int i=0; i<nbr_threads; i++){
    pthread_join(tid[i], NULL);
  }
  /*
  FILE * fp = fopen(file_name,"w");
  fprintf(fp, "P6\n%d %d\n255\n", size_matrix, size_matrix);

  
  //FILE * fp2 = fopen(file_name2,"w");
  //fprintf(fp2, "P6\n%d %d\n255\n", size_matrix, size_matrix);
  for (size_t jx = 0; jx < size_matrix; ++jx){
    for (size_t ix=0; ix<size_matrix; ++ix){
      static unsigned char color[3];
      color[0] = round(cabs((1-creal(answer_matrix[jx][ix]))/2.0) * 250.0);
      color[1] = round(cabs((1-cimag(answer_matrix[jx][ix]))/2.0) * 250.0);
      color[2] = round(cabs((creal(answer_matrix[jx][ix])+cimag(answer_matrix[jx][ix]))/2.0) *250.0);
      fwrite(color, 1, 3, fp);
      
      /* static unsigned char color2[3];
      color2[0] = round(255.0 * count_matrix[jx][ix]/50.0);
      color2[1] = 0;
      color2[2] = 0;
      fwrite(color2, 1, 3, fp2);	
     
    }
  }
  fclose(fp);
  //fclose(fp2);
 */
  
  return 0;
}

// END OF MAIN FUNCTION

// Functions below

void check_first_input(char * argv[]) {
  char * input1 = argv[1];
  if (input1[0] == '-' && input1[1] == 't'){
    nbr_threads = atoi(input1+2);
  }
  else if ( input1[0] == '-' && input1[1] == 'l'){
    size_matrix = atoi(input1+2);
  }
  else {
    printf("Invalid argument passed for number of threads. Please write on the form -t# -l# #.\n");
    printf("You wrote: %s", argv[1]);
    nbr_threads = 0;
    exit(-1);
  }
}

void check_second_input(char * argv[]) {
  char * input2 = argv[2];
  if (input2[0] == '-' && input2[1] == 'l'){
    size_matrix = atoi(input2+2);
  }
  else if ( input2[0] == '-' && input2[1] == 't'){
    nbr_threads = atoi(input2+2);
  }
  else {
    printf("Invalid argument passed for matrix size. Please write on the form -t# -l# #.\n");
    size_matrix = 0;
    exit(-1);
  }
}

void check_third_input(char * argv[]) {
  char * input3 = argv[3];
  if (isdigit(input3[0])==0){
    printf("Invalid argument passed for degree of polynomial. Please write on the form -t# -l# #.\n");
    pol_degree = 0;
    exit(-1);
  }
  else {
    pol_degree = atoi(input3);
  }
}

double complex multiplication_complex(double complex complex_nbr, int pol_degree){
  double complex tmp_complex = complex_nbr;
  for (int ix=2; ix <=pol_degree; ++ix){
    complex_nbr = complex_nbr*tmp_complex;
  }
  if (pol_degree ==0){
    complex_nbr = 1;
  }
  return complex_nbr;
}


// Newton's method. Calculates the roots and the number of iterations to reach them. 
double complex newtons_method(double complex complex_nbr, int pol_degree, long **matrix_pp, int a, int b, double complex * roots) {
  double p_g = pol_degree;
  double i = 0.001;
  double complex polynomial_derivative;
  double complex polynomial;
  double divergence_stop = 10000000000;
  int nbr_iter_tmp = 0;
  int stop_check = 0;
  while (1) {
    nbr_iter_tmp +=1;
    polynomial_derivative = multiplication_complex(complex_nbr, pol_degree-1);

    complex_nbr = complex_nbr - complex_nbr/p_g+1.0/(p_g*polynomial_derivative);

    // if close enough to root, will stop.

    if (abs(cabs(complex_nbr)-1) < i ) {
      for ( int ix = 0; ix<pol_degree; ix++) { 
	if (cabs(creal(complex_nbr) - creal(roots[ix])) < i && cabs(cimag(complex_nbr) - cimag(roots[ix])) < i) {
	  matrix_pp[a][b] = nbr_iter_tmp;
	  return complex_nbr;
	  stop_check = 2; break;
      }
    }
    }
    /*
    if (cabs((polynomial-1.0)) < i) {
      matrix_pp[a][b] = nbr_iter_tmp;
      break;
     }
    */
    
    // check and breaks if values diverging
    if (cabs(creal(complex_nbr)) > divergence_stop ||  cabs(cimag(complex_nbr)) > divergence_stop){
      complex_nbr = 0.0 + 0.0*I;
      matrix_pp[a][b] = nbr_iter_tmp;
      break;
     }
    
     // checks and breaks if it's getting to close to origo.
    if (cabs(creal(complex_nbr)) < i && cabs(cimag(complex_nbr)) < i) {
      complex_nbr = 0.0 + 0.0*I;
      matrix_pp[a][b] = nbr_iter_tmp;
      break;
      }
   }
  return complex_nbr;
}

void generate_poly_roots(double complex * roots, int pol_degree) {
  for ( int i=0; i<pol_degree; i++) {
    roots[i] = cos(2*M_PI*i/pol_degree)- sin(2*M_PI*i/pol_degree)*I;
  }
}
