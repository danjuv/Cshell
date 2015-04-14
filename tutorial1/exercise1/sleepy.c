#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
unsigned int sleep(unsigned int);
pid_t getpid(void);

void sleepy(int n)
{
	int i = 0;
	for(i = 0; i < n; i++)
	{
		sleep(1);
		printf("loop count: %i \n", i+1);
		printf("PID: %i \n", getpid()); 
	}
}

int main(int argc, char *argv[])
{
	if( argc!=2 )
	{
		//bad input
	}
	else
	{
		sleepy(atoi(argv[1]));
	}
	return 0;
}
