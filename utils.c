#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

double **malloc_Array2D(int row, int col)  {  
  int size = sizeof(double);  
  int point_size = sizeof(double*);  
  double **arr = (double **) malloc(point_size * row + size * row * col);  
  if (arr != NULL) {     
    memset(arr, 0, point_size * row + size * row * col);  
    double *head = (double*)(arr + row);  
    while (row--)  
      arr[row] = head + row * col;  
  }  
  return (double**)arr;  
}  

void free_Aarray2D(void **arr)  {  
  if (arr != NULL)  free(arr);  
} 

double distance(double *pos1, double *pos2, int dim) {
  int i;
  double dist = 0.0;
  for(i=0;i<dim;i++)
    dist += pow((pos2[i]-pos1[i]), 2.0);

  return sqrt(dist);
}
