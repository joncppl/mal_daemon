/**
 * mal_daemon.h
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 * 
 */
#ifndef MAL_DAEMON_H
 #define MAL_DAEMON_H

#define SUPER_LOOP_TIMER 1000000 //microsect

int process_args(int argc, char *argv[]);

int kill_daemon();

int daemonize();

void usage();

int loop();

void sig_handler(int);

int write_pid_file(pid_t);

pid_t read_pid_file();

int delete_pid_file();

#endif