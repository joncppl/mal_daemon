/**
 * sql.h
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 *
 * Provide some mysql interfacing ability
 *
 */
#ifndef SQL_H
#define SQL_H

int sql_connect(const char* server, const char* username, const char* password);

int sql_is_connected();

int sql_query();

int sql_close();

#endif