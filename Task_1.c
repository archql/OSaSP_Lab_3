#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

void printCurTime ()
{
	// time vars
	struct timeval tv_now;
	// get msecs
	gettimeofday( &tv_now, NULL );
	// get specs
	int msecs = tv_now.tv_usec / 1000;
	int secs  = tv_now.tv_sec % 60;
	int mins  = (tv_now.tv_sec / 60) % 60;
	int hrs   = (tv_now.tv_sec / 3600 + 3) % 24; // +3 is gmt
	// print time
	printf("time now %02d:%02d:%02d:%03d\n", hrs, mins, secs, msecs );
	
}

void printPIDs ()
{
	printf("My PID is %d, parents PID is %d\n", getpid(), getppid()); 
}

int waitForExit(pid_t pid, int pnum)
{
	if (pid > 0)
	{
		// wait to exit
		printf("wait for %dth exit\n", pnum);
		waitpid(pid, NULL, 0);
		printf("%dth exited\n", pnum);
	}
}

int main()
{
	pid_t pid_self, pid_child_1 = 0, pid_child_2 = 0;
	
	if (( pid_child_1 = fork() ) && ( pid_child_2 = fork() ))
	{
		// check if succeed			
		if (pid_child_1 >= 0 && pid_child_2 >= 0)
		{
			// at parent
			printf("> at parent ");
			printCurTime();
			printPIDs ();
			
			// call ps -x
			system( "ps -x" );
			
			// wait for process to end
			waitForExit( pid_child_1, 1 );
			waitForExit( pid_child_2, 2 );
		}
	}
	else if (pid_child_1 == 0)
	{
		// at 1st child
		printf("> at 1st child ");
		printCurTime();
		printPIDs ();
	}
	else
	{
		// at 2nd child
		printf("> at 2nd child ");
		printCurTime();
		printPIDs ();
	}

	return 0;
}
