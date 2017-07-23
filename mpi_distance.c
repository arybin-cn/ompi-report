#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "mpi.h"

#define IN
#define OUT
#define TAG 1

static int pid;
static int procSize;

int dataFromFile(OUT int* pTotalCount,OUT int*  pRealCount,OUT int** pIds,OUT double (**pPoints)[3]);
void dataToFile(IN int rowSize,IN int colSize,IN int *ids,IN double **dist);

int main(int argc,char *argv[]) {
  MPI_Request req;
  MPI_Status sta;
  int i,j,k,totalCount,realCount,*ids;
  double buf[4],**dist,(*points)[3];

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);

  //从文件读入数据
  dataFromFile(&totalCount,&realCount,&ids,&points);

  dist=malloc_Array2D(realCount,totalCount);
  for(i=0;i<realCount;i++){
    //填充点id
    buf[0]=ids[i];
    //填充点xyz
    for(j=0;j<3;j++) buf[j+1]=points[i][j];
    //异步发送给其他的进程
    for(j=0;j<procSize;j++) if(j!=pid) MPI_Isend(buf,4,MPI_DOUBLE,j,TAG,MPI_COMM_WORLD,&req);
    //计算已知点之间的距离
    for(j=0;j<realCount;j++) dist[i][ids[j]-1]=distance(points[i],points[j],3);
  }
  //从其他进程接收其他的点
  for(i=totalCount-realCount;i>0;i--){
    //允许接收任意进程发送来的点数据
    MPI_Recv(buf,4,MPI_DOUBLE,MPI_ANY_SOURCE,TAG,MPI_COMM_WORLD,&sta);
    for(j=0;j<realCount;j++) dist[j][(int)buf[0]-1]=distance(points[j],buf+1,3);
    printf("Proc[%d] receive point[%d] from MPI.\n",pid,(int)buf[0]);
  }

  //保存距离数据至文件
  dataToFile(realCount,totalCount,ids,dist);

  //释放资源
  free(ids); free(points);
  free_Aarray2D((void **)dist);
  MPI_Finalize();
}

int dataFromFile(OUT int* pTotalCount,OUT int*  pRealCount,
    OUT int** pIds,OUT double (**pPoints)[3]){
  FILE *fp;
  int i,j,maxCount;
  int tmpPointID;
  double tmpPoint[3];

  if((fp=fopen("points.dat","r"))==NULL) return -1;
  fscanf(fp, "There are %d points\n", pTotalCount);
  maxCount=*pTotalCount/procSize+(*pTotalCount%procSize!=0);

  *pPoints=(double(*)[3])malloc(sizeof(double)*3*maxCount);
  *pIds=(int*)malloc(sizeof(int)*maxCount);

  for(i=0,*pRealCount=0; i<*pTotalCount; i++){   
    fscanf(fp, "%d %lf %lf %lf", &tmpPointID, tmpPoint, tmpPoint+1,tmpPoint+2);
    if(pid==(i%procSize)){
      printf("Proc[%d] receive point[%d] from file.\n",pid,tmpPointID);
      for(j=0;j<3;j++){
        (*pPoints)[*pRealCount][j]=tmpPoint[j];
      }
      (*pIds)[*pRealCount]=tmpPointID;(*pRealCount)++;
    }
  }   
  fclose(fp);
  return 0;
}

void dataToFile(IN int rowSize,IN int colSize,IN int *ids,IN double **dist){
  int i,j;
  FILE *fp;
  char filename[255];
  sprintf(filename, "dist_%d_np.dat",pid);
  fp=fopen(filename, "w+");
  for(i=0;i<rowSize;i++) {
    fprintf(fp, "%4d", ids[i]);
    for(j=0;j<colSize;j++)
    {
      fprintf(fp, " %.3f", dist[i][j]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}
