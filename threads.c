#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <complex.h>

int check_first_input(char *argv[]); int check_second_input(char *argv[]); int check_third_input(char *argv[]);
float complex newtons_method(float complex, int pol_degree);
float complex multiplication_complex(float complex complex_nbr, int pol_degree);

int main (int argc, char * argv[]){
  int nbr_threads, size_matrix, pol_degree;
  nbr_threads=check_first_input(argv);
  size_matrix=check_second_input(argv);
  pol_degree=check_third_input(argv);
  //printf("%d and %d and %d \n", nbr_threads, size_matrix, pol_degree);
  float complex a,b,d;
  //a = newtons_method(10+20*I);
  a = 10 + 20*I;
  int c = cabs(a);
  printf("\n\nRe: %.2f Im: %.2fi\n abs value of a %d\n", creal(a), cimag(a), c);

  b= multiplication_complex(a, 3);
  printf("Re %f, Im %f \n", creal(b), cimag(b));
  d = newtons_method(a, 3);
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
  printf("\n\nInside function. Re: %f and Im: %f\n\n", creal(complex_nbr), cimag(complex_nbr));
  for (int ix=2; ix <=pol_degree; ++ix){
    complex_nbr = complex_nbr*tmp_complex;
  }
  return complex_nbr;
}

float complex newtons_method(float complex complex_nbr, int pol_degree) {
  float i = 0.001;
  float complex polynomial;
  long divergence_stop = 1e10;
  while (1 ) {
    polynomial = multiplication_complex(complex_nbr, pol_degree);
    complex_nbr = complex_nbr - complex_nbr/(pol_degree-1)+1/((pol_degree-1)*polynomial);
    if (  cabs(creal(complex_nbr)) > divergence_stop ||  cabs(cimag(complex_nbr)) > divergence_stop){
      printf("Not converging\n");
      return complex_nbr;
      break;
    }
    //if ( cabs(complex_nbr) < i) {
    //  printf("Too close to origin");
    //  break;
    // }
  }
  return complex_nbr;
}
