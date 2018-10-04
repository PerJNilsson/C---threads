#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <complex.h>
#include <math.h>

int check_first_input(char *argv[]); int check_second_input(char *argv[]); int check_third_input(char *argv[]);
float complex newtons_method(float complex, int pol_degree, long ** matrix_pp, int a, int b);
float complex multiplication_complex(float complex complex_nbr, int pol_degree);
void generate_cpx_numbers(float * allocated_vector, int size_matrix);


// creating an arg pointer
struct arg_pointer {
  int start_point;
  int end_point;
  int pol_degree;
  long ** count_matrix;
  complex float ** answer_matrix;
  int size_matrix;
  float sm;
};

// Creating the function for the pthreads
void *newton_thread(void *arg){
  struct arg_pointer *arg_s = (struct arg_pointer *)arg;
  int size_matrix1 = arg_s->size_matrix;
  int end_point1 = arg_s->end_point;
  int pol_degree1 = arg_s->pol_degree;
  float sm = size_matrix1;
  int start_point1 = arg_s->start_point;
  complex float ** answer_matrix1 = arg_s->answer_matrix;
  long **count_matrix1= arg_s->count_matrix;
  for (int i= start_point1; i<=end_point1; i++){
    for (int j=0; j<size_matrix1; j++){
      float re_input = (4.0*i/sm-2.0);
      float im_input = (4.0*j/sm-2.0);
      complex float cpx_input = re_input + im_input*I; 
      answer_matrix1[i][j] = newtons_method(cpx_input, pol_degree1, count_matrix1, i, j);
    }
  }
  pthread_exit(0);
}

int main (int argc, char * argv[]){
  int nbr_threads, size_matrix, pol_degree;
  nbr_threads=check_first_input(argv);
  size_matrix=check_second_input(argv);
  pol_degree=check_third_input(argv);

  // Allocating space for  different things that needs to be saved.
  float * cpx_vector = (float*) malloc(sizeof(float) * size_matrix);

  // Allocatng a global matrix that can store all the answers from threads. 
  complex float * answer_pointer = (complex float *) malloc(sizeof(complex float) * size_matrix*size_matrix);
  complex float ** answer_matrix = (complex float **) malloc(sizeof(complex float) * size_matrix);
  for (size_t ix=0, jx=0; ix<size_matrix; ++ix, jx+=size_matrix){
    answer_matrix[ix] = answer_pointer +jx;
  }

  long * count_pointer = (long *) malloc(sizeof(long) * size_matrix*size_matrix);
  long ** count_matrix = (long **) malloc(sizeof(long) * size_matrix);
  for (size_t ix=0, jx=0; ix<size_matrix; ++ix, jx+=size_matrix){
    count_matrix[ix] = count_pointer +jx;
  }

  
  // Creating file name for ppm pictures
  char fn1[128];  char file_name[] = "newton_convergence_x"; char file_name2[] = "newton_attractors_x";  char fn2[] = ".ppm";
  strcpy(fn1, argv[3]);  strcat(fn1, fn2);
  strcat(file_name, fn1); strcat(file_name2, fn1);

  // To do here, count start and endpoints
  
  // Creating the strucs args pointer for thread funtion
  struct arg_pointer args;
  args.start_point = 0;
  args.end_point = size_matrix;
  args.pol_degree = pol_degree;
  args.count_matrix = count_matrix;
  args.answer_matrix = answer_matrix;
  args.size_matrix = size_matrix;

  // Creating threads for faster computations
  pthread_t tid[nbr_threads];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  for (int i=0; i<nbr_threads; i++){

    pthread_create(&tid[i], &attr, newton_thread, &args);
  }
  // TODO: arguments to pretty much newton.
  
  FILE * fp = fopen(file_name,"wb");
  (void) fprintf(fp, "P6\n%d %d\n255\n", size_matrix, size_matrix);
  generate_cpx_numbers(cpx_vector, size_matrix);
  for (size_t ix=0; ix<size_matrix; ++ix){
    for (size_t jx=0; jx<size_matrix; ++jx){
      float complex d;
      //long *check;
      d = newtons_method(cpx_vector[ix]+cpx_vector[jx]*I, pol_degree, count_matrix, ix, jx);
      static unsigned char color[3];
      color[0] = cabs((1-creal(d))/2.0) * 250.0;
      color[1] = cabs((1-cimag(d))/2.0) * 250.0;
      color[2] = cabs((creal(d)+cimag(d))/2.0) *250.0;
      (void) fwrite(color, 1, 3, fp);	
      }
  }
  (void)fclose(fp);

  for (int i=0; i<nbr_threads; i++){
    pthread_join(tid[i], NULL);
  }

  return 0;
}

int check_first_input(char * argv[]) {
  char * input1 = argv[1];
  int nbr_threads;
  if (input1[0] == '-' && input1[1] == 't'){
    nbr_threads = atoi(input1+2);
  }
  else {
    printf("Invalid argument passed for number of threads. Please write on the form -t# -l# #.\n");
    nbr_threads = 0;
    exit(-1);
  }
  return nbr_threads;
}


int check_second_input(char * argv[]) {
  char * input2 = argv[2];
  int size_matrix;
  if (input2[0] == '-' && input2[1] == 'l'){
    size_matrix = atoi(input2+2);
  }
  else {
    printf("Invalid argument passed for matrix size. Please write on the form -t# -l# #.\n");
    size_matrix = 0;
    exit(-1);
  }
  return size_matrix;
}


int check_third_input(char * argv[]) {
  char * input3 = argv[3];
  int pol_degree;
  if (isdigit(input3[0])==0){
    printf("Invalid argument passed for degree of polynomial. Please write on the form -t# -l# #.\n");
    pol_degree = 0;
    exit(-1);
  }
  else {
    pol_degree = atoi(input3);
  }
  return pol_degree;
}

float complex multiplication_complex(float complex complex_nbr, int pol_degree){
  float complex tmp_complex = complex_nbr;
  for (int ix=2; ix <=pol_degree; ++ix){
    complex_nbr = complex_nbr*tmp_complex;
  }
  if (pol_degree ==0){
    complex_nbr = 1;
  }
  return complex_nbr;
}

float complex newtons_method(float complex complex_nbr, int pol_degree, long **matrix_pp, int a, int b) {
  float p_g = pol_degree;
  float i = 0.001;
  float complex polynomial_derivative;
  float complex polynomial;
  float divergence_stop = 10000000000;
  long nbr_iter_tmp = 0;
  while (1) {
    nbr_iter_tmp +=1;
    polynomial_derivative = multiplication_complex(complex_nbr, pol_degree-1);

    complex_nbr = complex_nbr - complex_nbr/p_g+1.0/(p_g*polynomial_derivative);

    polynomial = multiplication_complex(complex_nbr, pol_degree);
     
     // check and breaks if values diverging
    if ((float)cabs(creal(complex_nbr)) > divergence_stop || (float) cabs(cimag(complex_nbr)) > divergence_stop){
      complex_nbr = 0.0 + 0.0*I;
      matrix_pp[a][b] = nbr_iter_tmp;
      break;
     }
     // checks and breaks if it's getting to close to origo.
    if ((float)cabs(creal(complex_nbr)) < i && (float)cabs(cimag(complex_nbr)) < i) {
      complex_nbr = 0.0 + 0.0*I;
      matrix_pp[a][b] = nbr_iter_tmp;
      break;
     }
     // if close enough to root, will stop.
    if ((float) cabs((polynomial-1.0)) < i) {
      matrix_pp[a][b] = nbr_iter_tmp;
      break;
     }
   }
  return complex_nbr;
}

void generate_cpx_numbers(float * allocated_vector, int size_matrix) {
  float sm = size_matrix;
    for (size_t ix=0; ix<size_matrix; ++ix){  
    float complex element = (4.0*ix/sm-2.0);
    allocated_vector[ix] = element;
   }
  }

void generate_poly_roots(float complex * roots, int pol_degree) {
  for (size_t ix = 0; ix<pol_degree; ++ix){
    
  }
}
