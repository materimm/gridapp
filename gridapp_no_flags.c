#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define MAXGRIDSIZE 	10
#define MAXTHREADS	1000
#define NO_SWAPS	20

extern int errno;

typedef enum {GRID, ROW, CELL, NONE} grain_type;
int gridsize = 0;
int grid[MAXGRIDSIZE][MAXGRIDSIZE];
int threads_left = 0;

time_t start_t, end_t;

pthread_mutex_t gridLock;
pthread_mutex_t threadLock;
pthread_mutex_t rowLockArray[MAXGRIDSIZE];
pthread_mutex_t cellLockMatrix[MAXGRIDSIZE][MAXGRIDSIZE];

int PrintGrid(int grid[MAXGRIDSIZE][MAXGRIDSIZE], int gridsize)
{
	int i;
	int j;

	for (i = 0; i < gridsize; i++)
	{
		for (j = 0; j < gridsize; j++)
			fprintf(stdout, "%d\t", grid[i][j]);
		fprintf(stdout, "\n");
	}
	return 0;
}

long InitGrid(int grid[MAXGRIDSIZE][MAXGRIDSIZE], int gridsize)
{
	int i;
	int j;
	long sum = 0;
	int temp = 0;

	srand( (unsigned int)time( NULL ) );

	for (i = 0; i < gridsize; i++){
		for (j = 0; j < gridsize; j++) {
			temp = rand() % 100;
			grid[i][j] = temp;
			sum = sum + temp;
		}
	}
	return sum;
}

long SumGrid(int grid[MAXGRIDSIZE][MAXGRIDSIZE], int gridsize)
{
	int i;
	int j;
	long sum = 0;


	for (i = 0; i < gridsize; i++){
		for (j = 0; j < gridsize; j++) {
			sum = sum + grid[i][j];
		}
	}
	return sum;

}

void* do_swaps(void* args)
{
	int i, row1, column1, row2, column2;
	int temp;
	grain_type* gran_type = (grain_type*)args;

	pthread_mutex_lock(&threadLock);
		threads_left++;
		printf("total threads: %d\n",threads_left);
	pthread_mutex_unlock(&threadLock);

	for(i=0; i<NO_SWAPS; i++)
	{
		row1 = rand() % gridsize;
		column1 = rand() % gridsize;
		row2 = rand() % gridsize;
		column2 = rand() % gridsize;

/*		printf("row1: %i\nrow2: %i\n", row1, row2);*/
		if (*gran_type == ROW)
		{
		  /* obtain row level locks*/
		  /* *** FILL IN CODE HERE*/
		  if(row1<row2)
		  {
			pthread_mutex_lock(&rowLockArray[row1]);
			pthread_mutex_lock(&rowLockArray[row2]);
  		  }
		  else if(row1>row2)
		  {
			pthread_mutex_lock(&rowLockArray[row2]);
			pthread_mutex_lock(&rowLockArray[row1]);
		  }
		  else /* rows are equal */
		  {
 			pthread_mutex_lock(&rowLockArray[row1]);
		  }
		}
		else if (*gran_type == CELL)
		{
		  /* obtain cell level locks */
		  /* *** FILL IN CODE HERE  */
		  if(row1<row2)
		  {
			pthread_mutex_lock(&cellLockMatrix[row1][column1]);
			pthread_mutex_lock(&cellLockMatrix[row2][column2]);
		  }
		  else if(row1>row2)
		  {
			pthread_mutex_lock(&cellLockMatrix[row2][column2]);
			pthread_mutex_lock(&cellLockMatrix[row1][column1]);
		  }
		  else /* rows are equal */
		  {
			  if(column1<column2)
			  {
				pthread_mutex_lock(&cellLockMatrix[row1][column1]);
				pthread_mutex_lock(&cellLockMatrix[row2][column2]);
		  	  }
			  else if(column1>column2)
			  {
				pthread_mutex_lock(&cellLockMatrix[row2][column2]);
				pthread_mutex_lock(&cellLockMatrix[row1][column1]);
			  }
			  else /* columns are equal too */
			  {
 				pthread_mutex_lock(&cellLockMatrix[row1][column1]);
		  	  }
		  }

		}
		else if (*gran_type == GRID)
		{
		  /* obtain grid level lock*/
		  /* *** FILL IN CODE HERE */
	  	  pthread_mutex_lock(&gridLock);
		}

		/* Critical Section */
		printf("running %d\n",i);
		temp = grid[row1][column1];
		sleep(1);
		grid[row1][column1]=grid[row2][column2];
		grid[row2][column2]=temp;

		if (*gran_type == ROW)
		{
		  /* release row level locks */
		  /* *** FILL IN CODE HERE */
		  pthread_mutex_unlock(&rowLockArray[row1]);

		  if(row1!=row2)
		  {
			pthread_mutex_unlock(&rowLockArray[row2]);
		  }
		}
		else if (*gran_type == CELL)
		{
		  /* release cell level locks */
		  /* *** FILL IN CODE HERE */
		  pthread_mutex_unlock(&cellLockMatrix[row1][column1]);

		  if((row1!=row2) || (column1!=column2))
		  {
		  	pthread_mutex_unlock(&cellLockMatrix[row2][column2]);
		  }
		}
		else if (*gran_type == GRID)
		{
		  /* release grid level lock */
		  /* *** FILL IN CODE HERE */
		  pthread_mutex_unlock(&gridLock);
		}
	}

	/* does this need protection? */
	pthread_mutex_lock(&threadLock);
	printf("in thread lock\n");
	threads_left--;
	if (threads_left == 0){  /* if this is last thread to finish*/
	  time(&end_t);         /* record the end time*/
	  printf("no more threads\n");
	}
	printf("out of thread lock\n");
	pthread_mutex_unlock(&threadLock);

	return NULL;
}

int main(int argc, char **argv)
{
	int nthreads = 0;
	pthread_t threads[MAXTHREADS];
	grain_type rowGranularity = NONE;
	long initSum = 0, finalSum = 0;
	int i;
	int j;

	if (argc > 3)
	{
		gridsize = atoi(argv[1]);
		if (gridsize > MAXGRIDSIZE || gridsize < 1)
		{
			printf("Grid size must be between 1 and 10.\n");
			return(1);
		}
		nthreads = atoi(argv[2]);
		if (nthreads < 1 || nthreads > MAXTHREADS)
		{
			printf("Number of threads must be between 1 and 1000.");
			return(1);
		}

		if (argv[3][1] == 'r' || argv[3][1] == 'R')
		{
			rowGranularity = ROW;
			for(i=0; i<gridsize; i=i+1)
			{
				pthread_mutex_init(&rowLockArray[i],NULL);
			}
		}
		if (argv[3][1] == 'c' || argv[3][1] == 'C')
		{
			rowGranularity = CELL;
			for(i=0; i<gridsize; i=i+1)
			{
				for(j=0; j<gridsize; j=j+1)
				{
					pthread_mutex_init(&cellLockMatrix[i][j],NULL);
				}
			}
		}
		if (argv[3][1] == 'g' || argv[3][1] == 'G')
		{
			rowGranularity = GRID;
			pthread_mutex_init(&gridLock,NULL);
		}
		pthread_mutex_init(&threadLock,NULL);

	}
	else
	{
		printf("Format:  gridapp gridSize numThreads -cell\n");
		printf("         gridapp gridSize numThreads -row\n");
		printf("         gridapp gridSize numThreads -grid\n");
		printf("         gridapp gridSize numThreads -none\n");
		return(1);
	}


	printf("Initial Grid:\n\n");
	initSum =  InitGrid(grid, gridsize);
	PrintGrid(grid, gridsize);
	printf("\nInitial Sum:  %d\n", initSum);
	printf("Executing threads...\n");

	/* better to seed the random number generator outside
	   of do swaps or all threads will start with same
	   choice
	*/
	srand((unsigned int)time( NULL ) );

	time(&start_t);
	for (i = 0; i < nthreads; i++)
	{
		if (pthread_create(&(threads[i]), NULL, do_swaps, (void *)(&rowGranularity)) != 0)
		{
			perror("thread creation failed:");
			exit(-1);
		}
	}


	for (i = 0; i < nthreads; i++)
		pthread_detach(threads[i]);


	while (1)
	{
		sleep(2);
		if (threads_left == 0)
		  {
		    fprintf(stdout, "\nFinal Grid:\n\n");
		    PrintGrid(grid, gridsize);
		    finalSum = SumGrid(grid, gridsize);
		    fprintf(stdout, "\n\nFinal Sum:  %d\n", finalSum);
		    if (initSum != finalSum){
		      fprintf(stdout,"DATA INTEGRITY VIOLATION!!!!!\n");
		    } else {
		      fprintf(stdout,"DATA INTEGRITY MAINTAINED!!!!!\n");
		    }
		    fprintf(stdout, "Secs elapsed:  %g\n", difftime(end_t, start_t));

		    exit(0);
		  }
	}

	pthread_mutex_destroy(&threadLock);
	if(rowGranularity == GRID)
	{
		pthread_mutex_destroy(&gridLock);
	}
	else if(rowGranularity == ROW)
	{
		for(i=0; i<gridsize; i=i+1)
		{
			pthread_mutex_destroy(&rowLockArray[i]);
		}
	}
	else if(rowGranularity == CELL)
	{
		for(i=0; i<gridsize; i=i+1)
		{
			for(j=0; j<gridsize; j=j+1)
			{
				pthread_mutex_destroy(&cellLockMatrix[i][j]);
			}
		}
	}

	return(0);
}






