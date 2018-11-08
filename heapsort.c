/* heap sort */
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#define N 1000000

void showElapsed(int id, char *m);
void showVector(int *v, int n, int id);
int * merge(int *v1, int n1, int *v2, int n2);
void Sort( int *T, int root, int bottom, int *operation, int *test, int *swap);
void heapsort( int *T, int n);
double startTime, stopTime;

void showElapsed(int id, char *m)
{
	printf("%d: %s %f secs\n",id,m,(clock()-startTime)/CLOCKS_PER_SEC);
}

void showVector(int *v, int n, int id)
{
	int i;
	printf("%d: ",id);
	for(i=0;i<n;i++)
		printf("%d ",v[i]);
	putchar('\n');
}

int * merge(int *v1, int n1, int *v2, int n2)
{
	int i,j,k;
	int * result;

	result = (int *)malloc((n1+n2)*sizeof(int));

	i=0; j=0; k=0;
	while(i<n1 && j<n2)
		if(v1[i]<v2[j])
		{
			result[k] = v1[i];
			i++; k++;
		}
		else
		{
			result[k] = v2[j];
			j++; k++;
		}
	if(i==n1)
		while(j<n2)
		{
			result[k] = v2[j];
			j++; k++;
		}
	else
		while(i<n1)
		{
			result[k] = v1[i];
			i++; k++;
		}
	return result;
}

void Sort( int *T, int root, int bottom, int *operation, int *test, int *swap){  

	/* this function sort a Heap  */ 

	int maxchild, temp, child1, child2 ;

	/* T[] is the table to sort, bottom is the size of the array, root // is the root of the heap, maxchild represente the max of
	// (child1,child2),temp is use for exchange.
	*/
	child1=2*root+1;
	child2=2*(root+1);
	/* initialization of child1 an child2 */
	operation=operation+6; 

	if (child1<=bottom){ 

		if (child1==bottom){
			/* if there is no child2 maxchild is child1 */
			maxchild=child1;
			operation++;
			test++; /* child1==bottom */
		}
  
		else {
			maxchild=(T[child1]>=T[child2])?child1:child2;
			/* maxchild take the number of the max value between
			child 1 and 2 */
			test++;  /* T[child1]>=T[child2] */
			operation++; /* affectation of maxchild */
			test++; /* child1==bottom */
		}

		if ( T[root]< T[maxchild] ){
			temp=T[root];
			T[root]=T[maxchild];
			T[maxchild]=temp;
			/* if the maxvalue  is bigger than the value of the root,
			// the 2 values change.
			*/
			swap++;
			operation=operation+3;
			Sort(T, maxchild, bottom, operation, test, swap);
			/* recursive calling of the function Sort */
		}
		test++; 
	}
	test++;   /* child1<=bottom */
}

void heapsort( int *T, int n){    
	/* this function build a heap and sort it*/
	int i, temp;
	int operation=0;
	int test=0;
	int swap=0;
	/*
	// T is the array to sort, n the number of element in T, i is a
	// loop indice, temp is needed for exchanging the 2 elements of 
	// the table.
	*/

	for (i=(n/2)-1; i>=0; i--){
		Sort (T,i,n-1, &operation, &test, &swap);
		test++;			/* i>=0 */
		operation++;	/* initialisation + incrementation of i */
	} 

	for (i=n-1; i>=1; i--){
		test++;			/* i>=1 */
		operation++;	/* affectation + incrementation of i */
		temp=T[0];
		T[0]=T[i];
		T[i]=temp;
		/* puts the bigger element at the end */
		swap++;
		operation=operation+3;
		Sort (T, 0,i-1, &operation, &test, &swap);	/* sort the rest of the table */
		operation++;		/* i-1 */ 
	}
}

main(int argc, char **argv)
{
	int * data;
	int * chunk;
	int * other;
	int m,n=N;
	int id,p;
	int s;
	int i;
	int step;
	MPI_Status status;

	startTime = clock();

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&p);

	//showElapsed(id,"MPI setup complete");

	if(id==0)
	{
		int r;
		srandom(clock());
		
		s = n/p;
		r = n%p;
		data = (int *)malloc((n+s-r)*sizeof(int));
		for(i=0;i<n;i++)
			data[i] = random();
		if(r!=0)
		{
			for(i=n;i<n+s-r;i++)
				data[i]=0;
			s=s+1;
		}
		//showElapsed(id,"generated the random numbers");

		MPI_Bcast(&s,1,MPI_INT,0,MPI_COMM_WORLD);
		chunk = (int *)malloc(s*sizeof(int));
		MPI_Scatter(data,s,MPI_INT,chunk,s,MPI_INT,0,MPI_COMM_WORLD);

		//showElapsed(id,"scattered data");

		heapsort(chunk,s);
		/*showVector(chunk, s, id); */

		//showElapsed(id,"sorted");
	}
	else
	{
		MPI_Bcast(&s,1,MPI_INT,0,MPI_COMM_WORLD);
		chunk = (int *)malloc(s*sizeof(int));
		MPI_Scatter(data,s,MPI_INT,chunk,s,MPI_INT,0,MPI_COMM_WORLD);

		//showElapsed(id,"got data");

		heapsort(chunk,s);
		/*showVector(chunk, s, id); */

		//showElapsed(id,"sorted");
	}

	step = 1;
	while(step<p)
	{
		if(id%(2*step)==0)
		{
			if(id+step<p)
			{
				MPI_Recv(&m,1,MPI_INT,id+step,0,MPI_COMM_WORLD,&status);
				other = (int *)malloc(m*sizeof(int));
				MPI_Recv(other,m,MPI_INT,id+step,0,MPI_COMM_WORLD,&status);
				//showElapsed(id,"got merge data");
				chunk = merge(chunk,s,other,m);
				/*showVector(chunk, s, id); */
				//showElapsed(id,"merged data");
				s = s+m;
			} 
		}
		else
		{
			int near = id-step;
			MPI_Send(&s,1,MPI_INT,near,0,MPI_COMM_WORLD);
			MPI_Send(chunk,s,MPI_INT,near,0,MPI_COMM_WORLD);
			//showElapsed(id,"sent merge data");
			break;
		}
		step = step*2;
	}
	if(id==0)
	{
		FILE * fout;

		stopTime = clock();
		printf("%f\n",(stopTime-startTime)/CLOCKS_PER_SEC);
		
		//showElapsed(id,"opening out file");
		fout = fopen("result","w");
		for(i=0;i<s;i++)
			fprintf(fout,"%d\n",chunk[i]);
		fclose(fout);
		//showElapsed(id,"closed out file");
	}
	MPI_Finalize();
}

