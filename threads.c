#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>

int check_first_input(char *argv[]); int check_second_input(char *argv[]); int check_third_input(char *argv[]);

int main (int argc, char * argv[]){
  int nbr_threads, size_matrix, pol_degree;
  nbr_threads=check_first_input(argv);
  size_matrix=check_second_input(argv);
  pol_degree=check_third_input(argv);
  printf("%d and %d and %d \n", nbr_threads, size_matrix, pol_degree);
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
