#include <stdio.h>
 int main(){

  int *ptr = NULL;
   
  printf("value at the invalid memory location: ");
  printf("%d\n", *ptr);
  return 0;
 }
 
 
