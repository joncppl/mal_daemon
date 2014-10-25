/**
 * mal_daemon.hpp
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 * 
 */

#define SUPER_LOOP_TIMER 1000000 //microsect

int process_args(int argc, char *argv[]);

int kill_daemon();

int daemonize();

int loop();

void sig_handler(int);

int write_pid_file(pid_t);

pid_t read_pid_file();

int delete_pid_file();