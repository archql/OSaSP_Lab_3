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

pid_t *arr_pids;
int arr_pids_len;
int arr_pids_next;

char name_file_A_full[PATH_MAX];
char *name_file_A;
char name_file_B_full[PATH_MAX];
char *name_file_B;

// variant >A< -- always check & variant B -- "trash collector"

void launch_next( )
{
	int ecnt = 10; // max tries
	while ( ecnt-- )
	{
		int pid_next = 0; pid_t catched_pid;
		// find free (HANDLE WAITPID ERROR!)
		while ( (pid_next < arr_pids_len) && arr_pids[pid_next] && !( catched_pid = waitpid( arr_pids[pid_next], NULL, WNOHANG )) ) // 
			pid_next++;
		// if founded
		if (pid_next < arr_pids_len)
		{
			pid_t pid = fork();
			if ( pid > 0 )
			{
				// were in parent -- queue waits and encount launched forks
				//printf( "Parent:\n");
				arr_pids[pid_next] = pid;
			}
			else if ( !pid )
			{
				// were in child -- do cmp here
				if ( execlp("./Task_cmp.exe", "Task_cmp.exe", name_file_A_full, name_file_B_full, NULL) == -1);
				{
					myerror("Error occured while doing a comparation!");
				}
			}	
			else 
			{
				// ERROR cant create fork
				printf( "ERROR cant create fork\n");
			}
			// exit find
			break;
		}
		puts("wait");
	}
	
	// print error??
}

void wait_for_all_pids()
{
	for (int i = 0; i < arr_pids_len; i++)
	if (arr_pids[i]) 
		waitpid( arr_pids[i], NULL, 0 );
}


void cmp_files( struct dirent *dirent_A, struct dirent *dirent_B )
{
//printf("%d\n", arr_pids_len);
	// get file names
	strcpy(name_file_A, dirent_A->d_name);
	strcpy(name_file_B, dirent_B->d_name);
	
	//printf("Launch %s %s\n", name_file_A, name_file_B);
	//printf("Launch %s %s\n", name_file_A_full, name_file_B_full);
	//return;
		
	//printf("Launch %s %s\n", name_file_A, name_file_B);
	// This can be replaced with more eff?? check in while loop
	if ( !(dirent_A->d_type & DT_REG) || !(dirent_B->d_type & DT_REG) ) // if not files
		return;
	
	launch_next( name_file_A, name_file_B );
}

int main( int argc, char *argv[] )
{
	//printf( "%d\n", getpid() );
	// check if it is enough params passed
	if (argc != 4)
		myerror("Wrong args! Format: Task_i_v3.exe [dir A] [dir B] [num of processes runned simultaniosly]");
	// get numb of procs
	//int num_of_procs = 0;
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
		myerror("Wrong args! Format: 2nd param is [dir B]");	
		
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
	arr_pids = (pid_t *) calloc( arr_pids_len, sizeof(pid_t) );
	if ( !arr_pids )
		myerror("Mem allocation error!");	
	
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
	// free allocated array	
	free( arr_pids );
	
	// close dir A
	if (closedir(dir_A))
		perror("Error while dir A close occured!");
	// close dir B
	if (closedir(dir_B))
		perror("Error while dir B close occured!");
	
	/*
	const char *paramCL = "cmp"; 
	
	printf("exec %s %s %s\n", paramCL, argv[1], argv[2]);

	//int errid = execlp("ls", "ls", "-l", NULL);
	int errid = execlp( argv[0], argv[0], argv[1], argv[2], paramCL, NULL );
	if (errid == -1)
	{
		perror("Error occured while creation a process!");
		return -1;
	}
	
	*/
	// wait for process to end

	return 0;
}
