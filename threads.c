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
  FILE * fp = fopen("ppm-file.ppm","wb");
  (void) fprintf(fp, "P6\n%d %d\n255\n", size_matrix, size_matrix);
  generate_cpx_numbers(cpx_vector, size_matrix);
  float complex d; 
  for (size_t ix=0; ix<size_matrix; ++ix){
    for (size_t jx=0; jx<size_matrix; ++jx){
      d = newtons_method(cpx_vector[ix]+cpx_vector[jx]*I, pol_degree);
      //printf("Roots in (%f + %fi)\n", creal(d), cimag(d));
      static unsigned char color[3];
      color[0] = cabs((1-creal(d))/2.0) * 256.0;
      color[1] = cabs((1-cimag(d))/2.0) * 256.0;
      color[2] = cabs((creal(d)+cimag(d))/2.0) *256.0;
      //(void) fprintf(fp, "%f %f %f", color[0], color[1], color[2]);
      (void) fwrite(color, 1, 3, fp);
    }
    //(void) fprintf(fp, "\n"); 
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
  }
  return size_matrix;
}


int check_third_input(char * argv[]) {
  char * input3 = argv[3];
  int pol_degree;
  if (isdigit(input3[0])==0){
    printf("Invalid argument passed for degree of polynomial. Please write on the form -t# -l# #.\n");
    pol_degree = 0;
    }
  else {
    pol_degree = atoi(input3);
  }
  return pol_degree;
}

float complex multiplication_complex(float complex complex_nbr, int pol_degree){
  float complex tmp_complex = complex_nbr;
  //printf("\n\nInside function. Re: %f and Im: %f\n\n", creal(complex_nbr), cimag(complex_nbr));
  for (int ix=2; ix <=pol_degree; ++ix){
    complex_nbr = complex_nbr*tmp_complex;
  }
  if (pol_degree ==1){
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
      polynomial = multiplication_complex(complex_nbr, pol_degree);

      complex_nbr = complex_nbr - complex_nbr/1.0+1.0;

      if (cabs(creal(complex_nbr)) > divergence_stop ||  cabs(cimag(complex_nbr)) > divergence_stop){
	//printf("Not converging(%f,%fi)\n", creal(complex_nbr), cimag(complex_nbr));
	complex_nbr = 0+0*I;
	break;
      }
    
      if ((float)cabs(creal(complex_nbr)) < i && (float)cabs(cimag(complex_nbr)) < i) {
	//printf("Too close to origin. Re %f, Im %f\n", creal(complex_nbr), cimag(complex_nbr));
	complex_nbr = 0+0*I;
	break;
      }
      if ((float) cabs(creal(polynomial))-1.0 < i && (float)cabs(cimag(polynomial)) < i) {
	//printf("Roots in (%f,%fi)\n\n", creal(complex_nbr), cimag(complex_nbr));
	break;
    }   
      return complex_nbr;
    }
  }
  
 else {
   while (1) {
     polynomial = multiplication_complex(complex_nbr, pol_degree);
     polynomial_derivative = multiplication_complex(complex_nbr, pol_degree-1);

     complex_nbr = complex_nbr - complex_nbr/(p_g-1.0)+1.0/((p_g-1.0)*polynomial_derivative);

     if (cabs(creal(complex_nbr)) > divergence_stop ||  cabs(cimag(complex_nbr)) > divergence_stop){
       //printf("Not converging(%f,%fi)\n", creal(complex_nbr), cimag(complex_nbr));
       complex_nbr = 0 +0*I;
       break;
     }
    
     if ((float)cabs(creal(complex_nbr)) < i && (float)cabs(cimag(complex_nbr)) < i) {
       //printf("Too close to origin. Re %f, Im %f\n", creal(complex_nbr), cimag(complex_nbr));
       complex_nbr = 0+0*I;
       break;
     }
     if ((float) cabs(creal(polynomial))-1.0 < i && (float)cabs(cimag(polynomial)) < i) {
       //printf("Roots in (%f,%fi)\n\n", creal(complex_nbr), cimag(complex_nbr));
       break;
     }   
   }
   return complex_nbr;
 }
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
