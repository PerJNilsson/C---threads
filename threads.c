#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <complex.h>
#include <math.h>

int check_first_input(char *argv[]); int check_second_input(char *argv[]); int check_third_input(char *argv[]);
float complex newtons_method(float complex, int pol_degree);
float complex multiplication_complex(float complex complex_nbr, int pol_degree);
void generate_cpx_numbers(float * allocated_vector, int size_matrix);

int main (int argc, char * argv[]){
  int nbr_threads, size_matrix, pol_degree;
  nbr_threads=check_first_input(argv);
  size_matrix=check_second_input(argv);
  pol_degree=check_third_input(argv);

  // Allocating space for the matrix of different start points
  float * cpx_vector = (float*) malloc(sizeof(float) * size_matrix);
  /*float * cpx_pointer = (float *) malloc(sizeof(float) * 2*size_matrix*size_matrix);
  float ** cpx_matrix = (float **) malloc(sizeof(float) * size_matrix);
  for (size_t ix=0, jx=0; ix<size_matrix; ++ix, jx+=size_matrix){
    cpx_matrix[ix] = cpx_pointer +2*jx;
  }
  */
  char file_name[128];
  strcpy(file_name, argv[3]); strcat(file_name, "-power.ppm");
  printf("%s\n", file_name);
  FILE * fp = fopen(file_name,"wb");
  (void) fprintf(fp, "P6\n%d %d\n255\n", size_matrix, size_matrix);
  generate_cpx_numbers(cpx_vector, size_matrix);
  int check = 0;
  for (size_t ix=0; ix<size_matrix; ++ix){
    for (size_t jx=0; jx<size_matrix; ++jx){
      float complex d; 
      d = newtons_method(cpx_vector[ix]+cpx_vector[jx]*I, pol_degree);
      static unsigned char color[3];
      color[0] = cabs((1-creal(d))/2.0) * 250.0;
      color[1] = cabs((1-cimag(d))/2.0) * 250.0;
      color[2] = cabs((creal(d)+cimag(d))/2.0) *250.0;
      (void) fwrite(color, 1, 3, fp);	
      //printf("ix = %f, jx = %f\n", cpx_vector[ix], cpx_vector[jx]);
    }
  }
  (void)fclose(fp);
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

float complex newtons_method(float complex complex_nbr, int pol_degree) {
  float p_g = pol_degree;
  float i = 0.001;
  float complex polynomial_derivative;
  float complex polynomial;
  float divergence_stop = 10000000000;
  if (pol_degree ==1 ) {
    while(1) {
      complex_nbr = complex_nbr - complex_nbr/1.0+1.0;
      polynomial = multiplication_complex(complex_nbr, pol_degree);

      if ((float)cabs(creal(complex_nbr)) > divergence_stop ||  (float)cabs(cimag(complex_nbr)) > divergence_stop){
        return complex_nbr = 0.0+0.0*I;
	break;
      }
    
      if ((float)cabs(creal(complex_nbr)) < i && (float)cabs(cimag(complex_nbr)) < i) {
	complex_nbr = 0.0+0.0*I;
	break;
      }
      if ((float) cabs(creal(polynomial))-1.0 < i && (float)cabs(cimag(polynomial)) < i) {
        break;
    }   
      //return complex_nbr;
    }
  }
  
 else {
   int cancel_calculation = 0;
   while (cancel_calculation < 1000000) {
     polynomial_derivative = multiplication_complex(complex_nbr, pol_degree-1);

     complex_nbr = complex_nbr - complex_nbr/p_g+1.0/(p_g*polynomial_derivative);

     polynomial = multiplication_complex(complex_nbr, pol_degree);
     
     // check and breaks if values diverging
     if ((float)cabs(creal(complex_nbr)) > divergence_stop || (float) cabs(cimag(complex_nbr)) > divergence_stop){
       complex_nbr = 0.0 + 0.0*I;
       break;
     }
     // checks and breaks if it's getting to close to origo.
     if ((float)cabs(creal(complex_nbr)) < i && (float)cabs(cimag(complex_nbr)) < i) {
       complex_nbr = 0.0 + 0.0*I;
       break;
     }
     // if close enough to root, will stop.
     if ((float) cabs((polynomial-1.0)) < i) {
       break;
        /*&& (float)cabs(cimag(polynomial)) < i*/
     }
     //printf("%f and %f\n", creal(complex_nbr), cimag(complex_nbr));
     cancel_calculation +=1;
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
  /*
  for (size_t ix=0; ix<size_matrix; ++ix){
    for (size_t jx=0; jx<size_matrix; ++jx) {
      float complex element = (4.0*ix/sm-2.0)+(4.0*jx/sm-2.0)*I;
      printf("first %f, sencond %f\n\n", creal(element), cimag(element));
      allocated_matrix[ix][jx] = element;
    }
  }
  }*/
void generate_poly_roots(float complex * roots, int pol_degree) {
  for (size_t ix = 0; ix<pol_degree; ++ix){
    
  }
}
