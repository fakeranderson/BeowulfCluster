/******************************************************************************
 * AUTHOR: Blaise Barney. Adapted from Ros Leibensperger, Cornell Theory
 *   Center. Converted to MPI: George L. Gusciora, MHPCC (1/95)
 * LAST REVISED: 04/13/05
 ******************************************************************************/
//found from https://computing.llnl.gov/tutorials/mpi/samples/C/mpi_mm.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 50 //matrix size square
#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

int main()
{
	//clock_t start, end, totalTime = 0;
	struct timeval start, end;
	long useconds;
	int	numtasks,              /* number of tasks in partition */
			taskid,                /* a task identifier */
			numworkers,            /* number of worker tasks */
			source,                /* task id of message source */
			dest,                  /* task id of message destination */
			mtype,                 /* message type */
			rows,                  /* rows of matrix A sent to each worker */
			averow, extra, offset, /* used to determine rows sent to each worker */
			rc = 0;					           /* misc */
	static double a[N][N], b[N][N], c[N][N];
	srand(time(0));



	MPI_Status status;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	if (numtasks < 2 ) {
		printf("Need at least two MPI tasks. Quitting...\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		exit(1);
	}
	numworkers = numtasks-1;
	/**************************** master task ************************************/
	if (taskid == MASTER)
	{
		//printf("Initializing %dx%d arrays...\n", N, N);
		for (int i=0; i<N; i++)
		{
			for (int j=0; j<N; j++)
			{
				a[i][j]= rand() % 101;
				b[i][j]= rand() % 101;
			}
		}
		//printf("mpi_mm has started with %d tasks.\n",numtasks);
		gettimeofday(&start, NULL);
		/* Send matrix data to the worker tasks */
		averow = N/numworkers;
		extra = N%numworkers;
		offset = 0;
		mtype = FROM_MASTER;
		for (dest=1; dest<=numworkers; dest++)
		{
			rows = (dest <= extra) ? averow+1 : averow;
			//printf("Sending %d rows to task %d offset=%d\n",rows,dest,offset);
			MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&a[offset][0], rows*N, MPI_DOUBLE, dest, mtype,MPI_COMM_WORLD);
			MPI_Send(&b, N*N, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
			offset = offset + rows;
		}
		/* Receive results from worker tasks */
		mtype = FROM_WORKER;
		for (int i=1; i<=numworkers; i++)
		{
			source = i;
			MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
			MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
			MPI_Recv(&c[offset][0], rows*N, MPI_DOUBLE, source, mtype,MPI_COMM_WORLD, &status);
			//printf("Received results from task %d\n",source);
		}
		gettimeofday(&end, NULL);
		//seconds  = end.tv_sec  - start.tv_sec;
		useconds = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);

		//double seconds = ((double)totalTime)/CLOCKS_PER_SEC;
		printf("%ld\n", useconds);


	}
	/**************************** worker task ************************************/
	if (taskid > MASTER)
	{
		int workerNum;
		MPI_Comm_rank(MPI_COMM_WORLD,&workerNum);
		mtype = FROM_MASTER;
		MPI_Recv(&offset, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&a, rows*N, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&b, N*N, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status); 
		for (int k=0; k<N; k++)  //was NCB
		{
			for (int i=0; i<rows; i++)
			{
				c[i][k] = 0.0;
				for (int j=0; j<N; j++) //was NCA
				{
					c[i][k] = c[i][k] + a[i][j] * b[j][k];
				}				
			}
		}
		mtype = FROM_WORKER;
		MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
		MPI_Send(&c, rows*N, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD); //was NCB
	}
	MPI_Finalize();

}

