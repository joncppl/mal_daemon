/**
 * configuration.c
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 *
 * Provide some configuration ability
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <libconfig.h>

#include "configuration.h"

#include "Log.h"

// char mal_username[CONFIGURE_STRING_LENGTH];
// char mal_password[CONFIGURE_STRING_LENGTH];
// char db_server[CONFIGURE_STRING_LENGTH];
// char db_username[CONFIGURE_STRING_LENGTH];
// char db_password[CONFIGURE_STRING_LENGTH];

config_t config;
config_setting_t *root, *mal_username, *mal_password, *db_server, *db_username, *db_password;

void configure_init() {
	struct termios oflags, nflags;
	char t_mal_username[CONFIGURE_STRING_LENGTH];
	char t_mal_password[CONFIGURE_STRING_LENGTH];
	char t_db_server[CONFIGURE_STRING_LENGTH];
	char t_db_username[CONFIGURE_STRING_LENGTH];
	char t_db_password[CONFIGURE_STRING_LENGTH];
	config_init(&config);
	root = config_root_setting(&config);
	mal_username = config_setting_add(root, "mal_username", CONFIG_TYPE_STRING);
	mal_password = config_setting_add(root, "mal_password", CONFIG_TYPE_STRING);
	db_server = config_setting_add(root, "db_server", CONFIG_TYPE_STRING);
	db_username = config_setting_add(root, "db_username", CONFIG_TYPE_STRING);
	db_password = config_setting_add(root, "db_password", CONFIG_TYPE_STRING);
	
	printf("No Configuration detected. Enter configuration here.\nMyAnimeList Credentials.\nUsername: ");
	fgets(t_mal_username, CONFIGURE_STRING_LENGTH, stdin);
	printf("Password: ");

	/* disabling echo */
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
    }

    fgets(t_mal_password, CONFIGURE_STRING_LENGTH, stdin);
 	/* restore terminal */
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
    }

    printf("MySQL DB.\nAddress: ");
    fgets(t_db_server, CONFIGURE_STRING_LENGTH, stdin);
    printf("Username: ");
    fgets(t_db_username, CONFIGURE_STRING_LENGTH, stdin);
    printf("Password:");

    /* disabling echo */
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
    }

    fgets(t_db_password, CONFIGURE_STRING_LENGTH, stdin);

 	/* restore terminal */
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
    }
	
	t_mal_username[strlen(t_mal_username) - 1] = 0;
	t_mal_password[strlen(t_mal_password) - 1] = 0;
	t_db_server[strlen(t_db_server) - 1] = 0;
	t_db_username[strlen(t_db_username) - 1] = 0;
	t_db_password[strlen(t_db_password) - 1] = 0;

    configure_set_mal_username(t_mal_username);
	configure_set_mal_password(t_mal_password);
	configure_set_db_server(t_db_server);
	configure_set_db_username(t_db_username);	
	configure_set_db_password(t_db_password);
}

int configure_load()
{
	if (CONFIG_FALSE == config_read_file(&config, CONFIGURATION_FILENAME))
	{
		Log(LOG_ALERT, "Failed to open config file. %s.", config_error_text(&config));
		return -1;
	}

	root = config_root_setting(&config);

	mal_username = config_setting_get_member(root, "mal_username");
	mal_password = config_setting_get_member(root, "mal_password");
	db_server = config_setting_get_member(root, "db_server");
	db_username = config_setting_get_member(root, "db_username");	
	db_password = config_setting_get_member(root, "db_password");

	return 0;
}

int configure_write() 
{
	if (CONFIG_FALSE == config_write_file(&config, CONFIGURATION_FILENAME))
	{
		Log(LOG_ERROR, "Failed to write config file. %s", config_error_text(&config));
		return -1;
	}
	return 0;
}

const char * configure_get_mal_username()
{
	return config_setting_get_string(mal_username);
}

const char * configure_get_mal_password()
{
	return config_setting_get_string(mal_password);
}

const char * configure_get_db_server()
{
	return config_setting_get_string(db_server);
}

const char * configure_get_db_username()
{
	return config_setting_get_string(db_username);
}

const char * configure_get_db_password()
{
	return config_setting_get_string(db_password);
}

void configure_set_mal_username(const char *in)
{
	config_setting_set_string(mal_username, in);
}

void configure_set_mal_password(const char *in)
{
	config_setting_set_string(mal_password, in);	
}

void configure_set_db_server(const char *in)
{
	config_setting_set_string(db_server, in);
}

void configure_set_db_username(const char *in)
{
	config_setting_set_string(db_username, in);
}

void configure_set_db_password(const char *in)
{
	config_setting_set_string(db_password, in);
}

