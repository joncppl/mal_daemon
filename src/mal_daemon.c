/**
 * mal_daemon.cpp
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 *
 * This daemon retreives data from the MyAnimeList API and stores it in a local database
 *
 */

#include "config.h"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "Log.h"

#include "mal_daemon.h"

typedef enum {
	running,
	stopped
} RunState;

RunState run_state;

const char *pidfile = "/var/run/" PACKAGE_NAME;
pid_t pid;

int loop() 
{
	//superloop
	while (run_state == running) 
	{	
		usleep(SUPER_LOOP_TIMER);
	}
	
	Log(LOG_INFO, "Shutting down.");
	delete_pid_file();
	return 0;
}

int main(int argc, char *argv[]) 
{
	process_args(argc, argv);
	daemonize();
	return 0;
}

int process_args(int argc, char *argv[]) 
{
	if (argc > 1) {
		int i;
		for (i = 1; i < argc; i++) 
		{
			if (strcmp(argv[i], "stop") == 0) 
			{
				kill_daemon();
				exit(0);
			}
		}
	}

	return 0;
}

int kill_daemon()
{
	pid = read_pid_file();
	if (!pid) 
	{
		puts("Not running.");
		return -1;
	}
	else 
	{
		printf("Stopping process with pid %d.\n", pid);
		if (-1 == kill(pid, SIGTERM))
		{
			puts("Failed to stop process. Try as root.");
		}
		else 
		{
			puts("Stopped successfully.");
		}
		return 0;
	}
}

int daemonize() 
{
	pid = read_pid_file();

	if (pid) {
		//check if actually running
		char procfile[56];
		sprintf(procfile, "/proc/%d", pid);
		struct stat sts;
		if (!(stat(procfile, &sts) == -1 || errno == ENOENT))
		{
			printf("Already running with pid %d.\n", pid);
			exit(0);	
		}
		//Try to delete pidfile
		delete_pid_file();
	}

	//check that pid can be written to
	if (-1 == write_pid_file(0)) {
 		puts("Failed to write pid file. Try running as root.");
 		Log(LOG_ERROR, "Failed to write pid file. Try running as root.");
 		exit(1);
	}

	//fork
 	pid = fork();

 	if (pid != 0) {
 		printf("Starting with pid %d.\n", pid);
	 	if (-1 == write_pid_file(pid)) {
		 		puts("Failed to write pid file.");
		 		Log(LOG_ERROR, "Failed to write pid file.");
		 		exit(1);
			}
 		exit(0);
 	}

 	log_init();
 	Log(LOG_INFO, PACKAGE_NAME " is starting");

 	//handle signals
 	struct sigaction act;
 	memset(&act, 0, sizeof(act));
 	act.sa_handler = sig_handler;

 	if (sigaction(SIGINT, &act, 0) == -1) 
 	{
 		puts("Failed to handle SIGINT.");
 		Log(LOG_ERROR, "Failed to handle SIGINT.");
 	}
 	if (sigaction(SIGTERM, &act, 0) == -1) 
 	{
 		puts("Failed to handle SIGTERM.");
 		Log(LOG_ERROR, "Failed to handle SIGTERM.");
 	}
 
  	run_state = running;

 	loop();

 	return 0;
}

void sig_handler(int signo) 
{	
	if (signo == SIGINT) 
	{
		Log(LOG_INFO, "received SIGINT");
	}
	else if (signo == SIGTERM) 
	{
		Log(LOG_INFO, "received SIGTERM");
		run_state = stopped;
	} 
	else if (signo == SIGSTOP) 
	{
		Log(LOG_INFO, "received SIGSTOP");
		run_state = stopped;
	}
	else if (signo == SIGKILL) 
	{
		Log(LOG_INFO, "received SIGKILL");
		run_state = stopped;
	}

}

int write_pid_file(pid_t id) 
{
	FILE *fp = fopen(pidfile, "w");
	if (!fp)
	{
		return -1;
	}
	else 
	{
		fprintf(fp, "%d", id);
	}

	fclose(fp);

	return 0;
}

pid_t read_pid_file()
{
	pid_t ret;
  	size_t lSize;
  	char *buffer;
  	size_t result;

  	
	FILE *fp = fopen(pidfile, "r");
	if (!fp)
	{

		ret = 0;
	} 
	else 
	{
		fseek (fp , 0 , SEEK_END);
	  	lSize = ftell (fp);
	  	rewind (fp);

	  	buffer = (char*) malloc (sizeof(char) * lSize);
  		if (buffer == NULL) 
  		{
  			fputs ("Memory error.", stderr); 
  			exit (2);
  		}

  		result = fread(buffer, 1, lSize, fp);
 		if (result != lSize) 
 		{
 			fputs("Reading error.", stderr);
 			exit(3);
 		}

 		ret = atoi(buffer);

		fclose(fp);
	  	free(buffer);
	}

  	return ret;
}

int delete_pid_file()
{
	unlink(pidfile);
	return 0;
}