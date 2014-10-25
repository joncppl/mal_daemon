/**
 * mal_daemon.c
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

#include <curl/curl.h>
#include <mysql.h>
#include <libconfig.h>

#include "Log.h"
#include "sql.h"
#include "configuration.h"

#include "mal_daemon.h"

RunState run_state;

RunMode run_mode;

const char *pidfile = "/var/run/" PACKAGE_NAME;
pid_t pid;

//from command line options
unsigned short run_config = 0; //bool

int loop() 
{
	//startup
	printf("%s version %s.\n", PACKAGE_NAME, PACKAGE_VERSION);
	printf("Report all bugs to %s.\n", PACKAGE_BUGREPORT);
	printf("Using libcurl version: %s\n", curl_version());
	printf("Using MySQL client version: %s\n", mysql_get_client_info());

	//superloop
	while (run_state == running) 
	{	
		//attempt to reconnect if we aren't connected.
		if(!sql_is_connected()) {
			sql_connect(configure_get_db_server(), configure_get_db_username(), configure_get_db_password());
		}		

		//start dumping date from queue
		if (sql_is_connected()) {

		}

		usleep(SUPER_LOOP_TIMER);
	}
	
	sql_close();
	Log(LOG_INFO, "Shutting down.");
	delete_pid_file();
	return 0;
}

int main(int argc, char *argv[]) 
{
	run_mode = mode_daemon;
	process_args(argc, argv);	
	return 0;
}

void check_config() 
{
	if (!run_config) 
	{
		if (-1 == configure_load()) 
		{
			configure_init();
		}
		if (-1 == configure_write())
		{
			exit(1);
		}
	}
	else {
		configure_init();
		if (-1 == configure_write())
		{
			exit(1);
		}
	}
}

int process_args(int argc, char *argv[]) 
{
	int i;
	unsigned short start = 0;
	unsigned short stop = 0;
	unsigned short instance = 0;
	unsigned short help = 0;
	char options[64];
	memset(options, 0, 64);

	if (argc > 1) {
		for (i = 1; i < argc; i++) 
		{
			if (argv[i][0] == '-' && strlen(argv[i]) > 1 && argv[i][1] != '-')
			{
				memcpy(&options[0] + strlen(options), &argv[i][1], strlen(argv[i]) - 1);
			}
			else if (strcmp(argv[i], "stop") == 0) 
			{
				stop = 1;
			}
			else if (strcmp(argv[i], "start") == 0)
			{
				start = 1;
			}
			else if (strcmp(argv[i], "--help") == 0)
			{
				options[strlen(options)] = 'h';
			}
			else if (strcmp(argv[i], "--instance") == 0)
			{
				options[strlen(options)] = 'i';
			}
			else if (strcmp(argv[i], "--reconfig") == 0)
			{
				options[strlen(options)] = 'r';
			}
			else 
			{
				printf("Unkown option: %s\n", argv[i]);
			}
		}
	}

	for (i = 0; i < strlen(options); i++)
	{
		switch (options[i])
		{
			case 'i':
				instance = 1;
				break;
			case 'r':
				run_config = 1;
				break;
			case 'h':
				help = 1;
			default:
				printf("Unkown option: %c\n", options[i]);
				break;
		}
	}

	if (help)
	{
		usage();
		exit(0);
	}

	if (start + stop + instance > 1) 
	{
		puts("Conflicting options. (if using -i don't use start or stop)");
		exit(0);
	}

	if (instance)
	{
		run_mode = mode_instance;

		log_init();
 		Log(LOG_INFO, PACKAGE_NAME " is starting.");
				check_config();

		//handle signals
	 	struct sigaction act;
	 	memset(&act, 0, sizeof(act));
	 	act.sa_handler = sig_handler;
		if (sigaction(SIGINT, &act, 0) == -1) 
	 	{
	 		Log(LOG_ERROR, "Failed to handle SIGINT.");
	 	}
		loop();
		exit(0);
	}

	if ((!stop && !instance) || start) 
	{
		daemonize();
		exit(0);
	}

	else if (stop)
	{
		kill_daemon();
		exit(0);
	}

	if (run_config && !(start || stop || instance)) 
	{
		configure_init();
		if (-1 == configure_write())
		{
			exit(1);
		}
		exit(0);
	}
	usage();
	return 0;
}

void usage() {
	printf("When %s runs, it continually updates a local database to effectively mirror the MyAnimeList database.\n", PACKAGE_NAME);
	printf("Version: %s\nReport bugs to %s\n\n", PACKAGE_VERSION, PACKAGE_BUGREPORT);
	printf("Using libcurl version: %s\n", curl_version());
	printf("Using MySQL client version: %s\n", mysql_get_client_info());
	printf("\n");
	printf("Usage: %s [OPTIONS] [start|stop]\n\n", PACKAGE_NAME);
	puts("Options:");
	puts("-i --instance    don't run as a daemon");
	puts("-r --reconfig    generate configuration before running");
	puts("-h --help        show help");
}

int kill_daemon()
{
	pid = read_pid_file();

	if (!pid) 
	{
		puts("Not running.");
		return -1;
	}
	
	char procfile[56];
	sprintf(procfile, "/proc/%d", pid);
	struct stat sts;
	if ((stat(procfile, &sts) == -1 && errno == ENOENT))
	{
		puts("Not running.");
		delete_pid_file();
		return -1;
	}


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
 		exit(1);
	}

	check_config();

	//fork
 	pid = fork();

 	if (pid != 0) {
 		printf("Starting with pid %d.\n", pid);
	 	if (-1 == write_pid_file(pid)) {
	 		puts("Failed to write pid file.");
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
 		Log(LOG_ERROR, "Failed to handle SIGINT.");
 	}
 	if (sigaction(SIGTERM, &act, 0) == -1) 
 	{
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
		if (run_mode == mode_instance)
		{
			run_state = stopped;
		}
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