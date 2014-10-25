/**
 * configuration.h
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 *
 * Provide some configuration ability
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define CONFIGURE_STRING_LENGTH 64

#define CONFIGURATION_FILENAME "mald_config"

void configure_init();

int configure_load();

int configure_write();

const char * configure_get_mal_username();

const char * configure_get_mal_password();

const char * configure_get_db_server();

const char * configure_get_db_username();

const char * configure_get_db_password();

void configure_set_mal_username(const char *);

void configure_set_mal_password(const char *);

void configure_set_db_server(const char *);

void configure_set_db_username(const char *);

void configure_set_db_password(const char *);

#endif
