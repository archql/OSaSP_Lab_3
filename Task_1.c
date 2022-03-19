#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

void printCurTime ( const char *string_at )
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
	printf("%s. My PID is %d, parents PID is %d. time now %02d:%02d:%02d:%03d\n", string_at, getpid(), getppid(),
		hrs, mins, secs, msecs );
	
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
	
	if ((( pid_child_1 = fork() ) > 0) && (( pid_child_2 = fork() ) > 0)) // always true for parent
	{
		// at parent
		printCurTime("> at parent");
			
		// call ps -x
		system( "ps -x" );
			
		// wait for process to end
		waitForExit( pid_child_1, 1 );
		waitForExit( pid_child_2, 2 );
	}
	else if (pid_child_1 == 0)
	{
		// at 1st child
		printCurTime("> at 1st child");
	}
	else if (pid_child_2 == 0)
	{
		// at 2nd child
		printCurTime("> at 2nd child");
	} 
	else if (pid_child_2 < 0)
	{
		// error create 2nd child
		perror("error create 2nd child");
		waitForExit( pid_child_1, 1 );
	} 
	else 
	{
		// error create 1st child
		perror("error create 2nd child");
	
	}
	

	return 0;
}
