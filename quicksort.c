#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>





void sawp(int * first, int* second);
void printArray(int myArray[], int size);
void quickSort(int myArray[], int low, int high);
int quickPartition(int myArray[], int low, int high);
int main(int argc, char *argv[])
{
	long usTime;
	struct timeval t1,t2;
	gettimeofday(&t1,NULL);
	if(argc!=2)
	{
		fprintf(stderr,"No argument inserted.\n");
		return -1;
	}

	srand(time(NULL));	
	int i;
	int size = atoi(argv[1]);
	int myArray[size];
	for(i =0; i<size;i++)
	{
		myArray[i] = rand() % 1000 + 1;
	}
	printf("Original Array:\n");
	printArray(myArray,size);
	
	#pragma omp parallel
	{
	quickSort(myArray,0,size-1);

	}	
	printf("Sorted Array: \n");
	printArray(myArray,size);
	gettimeofday(&t2,NULL);
	usTime = t2.tv_usec - t1.tv_usec;
	printf("%d ns   %d ms    %d s \n",usTime*1000,usTime,usTime/1000000);	
	
	return 0;

}
void swap(int * first, int* second)
{
	int temp = *first;
	*first = *second;
	*second = temp;
}
int quickPartition(int myArray[],int  low,int  high)
{
	int pivot = myArray[high];
	int indexSmaller = low -1;
	int j = 0;	
	for( j = low;j <=high-1;j++)
	{
		if(myArray[j] <= pivot)
		{
			indexSmaller++;
			swap(&myArray[indexSmaller],&myArray[j]);
		
		}
	
	}
	swap(&myArray[indexSmaller+1],&myArray[high]);
	return(indexSmaller+1);
}
void quickSort(int myArray[],int low, int high)
{
	if(low<high)
	{
		int mid = quickPartition(myArray,low,high);
		quickSort(myArray, low, mid-1);
		quickSort(myArray, mid+1,high);
	}
}

void printArray(int myArray[], int size)
{
	int i=0;
	for(i=0;i<size;i++)
	{
		printf("%d ", myArray[i]);
	}
	printf("\n");	
}

	

