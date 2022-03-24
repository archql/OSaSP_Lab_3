#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <string.h>
#include <dirent.h>

#include <limits.h> // here is PATH_MAX

void myerror(const char *errmsg)
{
	perror(errmsg);
	exit(-1);
}

int arr_pids_len;
int arr_pids_next;

char name_file_A_full[PATH_MAX];
char *name_file_A;
char name_file_B_full[PATH_MAX];
char *name_file_B;

void launch_next()
{
	if ( arr_pids_next >= arr_pids_len ) // not enough place to create new process
	{
		// wait for somebody
		wait(NULL);
		// dec ctr
		arr_pids_next--;
	}
	arr_pids_next++;
	// create new process
	pid_t pid = fork();
	/*if ( pid > 0 )
	{
		// were in parent -- queue waits and encount launched forks
	}
	else*/
	if ( !pid )
	{
		// were in child -- do cmp here
		if ( execlp("./Task_cmp.exe", "Task_cmp.exe", name_file_A_full, name_file_B_full, NULL) == -1);
		{
			myerror("Error occured while doing a comparation!");
		}
	}	
	else if ( pid < 0 )
	{
		// ERROR cant create fork
		puts( "ERROR cant create fork");
	}
	

}
void wait_for_all_pids()
{
	for ( int i = arr_pids_next; i > 0; i-- ) // end all processes
	{
		// wait for somebody
		wait(NULL);
	}
}


void cmp_files( struct dirent *dirent_A, struct dirent *dirent_B )
{
	// get file names
	strcpy(name_file_A, dirent_A->d_name);
	strcpy(name_file_B, dirent_B->d_name);
	
	// This can be replaced with more eff?? check in while loop
	if ( !(dirent_A->d_type & DT_REG) || !(dirent_B->d_type & DT_REG) ) // if not files
		return;
	
	launch_next( name_file_A, name_file_B );
}

int main( int argc, char *argv[] )
{
	// check if it is enough params passed
	if (argc != 4)
		myerror("Wrong args! Format: Task_i_v3.exe [dir A] [dir B] [num of processes runned simultaniosly]");
	// get numb of procs
	arr_pids_len = 0;
	if ( !sscanf( argv[3], "%u", &arr_pids_len ) || (arr_pids_len < 1) )
		myerror("Wrong args! Format: 3rd param is [num of processes runned simultaniosly]");
	// open dirs
	// get dir A
	DIR * dir_A = opendir( argv[1] );  
	// try dir
	if (!dir_A)
		myerror("Wrong args! Format: 1st param is [dir A]");
	// get dir B
	DIR * dir_B = opendir( argv[2] );  
	// try dir
	if (!dir_B) 
	{
		closedir(dir_A);
		myerror("Wrong args! Format: 2nd param is [dir B]");	
	}
		
	// setup file base pathes
	strcpy(name_file_A_full, argv[1] );
	strcpy(name_file_B_full, argv[2] );
	
	name_file_A = name_file_A_full + strlen(name_file_A_full);
	name_file_B = name_file_B_full + strlen(name_file_B_full);
	
	*(name_file_A++) = '/';
	*(name_file_B++) = '/';
		
	// === find loop ===
	struct dirent *dirent_A, *dirent_B;
	// alloc array for pids
	arr_pids_next = 0;
	
	// run for all files in dir
	while ( dirent_A = readdir(dir_A) )
	{
		while ( dirent_B = readdir(dir_B) )
			cmp_files( dirent_A, dirent_B );		
		// restart list
		rewinddir(dir_B);
	}
			
	// wait for all started processess	
	wait_for_all_pids();
	
	// close dir A
	if (closedir(dir_A))
		perror("Error while dir A close occured!");
	// close dir B
	if (closedir(dir_B))
		perror("Error while dir B close occured!");

	return 0;
}
