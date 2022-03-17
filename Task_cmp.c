#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>

void myerror(const char *errmsg)
{
	perror(errmsg);
	exit(-1);
}

int main( int argc, char *argv[] )
{
	// check if it is enough params passed
	if (argc != 3)
		myerror("Wrong args! Format: Task_4.exe [filename A] [filename B]");
	// get ptr to filename
	char *filename_A  = argv[1];
	char *filename_B  = argv[2];

	// try to open files
	FILE* file_A  = fopen (filename_A , "r");
	if ( !file_A )
		myerror("Cant open file A");
	FILE* file_B  = fopen (filename_B, "r");
	if ( !file_B )
		myerror("Cant open file B");
		
	// cmp em symbol per symbol
	char char_a, char_b; int cnt = 0; 
	while ( (char_a = fgetc( file_A )) == (char_b = fgetc( file_B )) )
	{
		cnt++; // inc num of looked bytes
		if (( char_a == EOF ) && (char_b == EOF)) // both files equals
		{
			printf("Child %d: file %s == %s (bytes readed: %d) \n", getpid(), filename_A, filename_B, cnt );
			return 0;
		}
	}
	// both files not equals
	printf("Child %d: file %s != %s (diff at byte no %d) \n", getpid(), filename_A, filename_B, cnt );
	

	return 0;
}
