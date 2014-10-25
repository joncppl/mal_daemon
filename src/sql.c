/**
 * sql.c
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 *
 * Provide some mysql interfacing ability
 *
 */
#include <stdlib.h>

#include <mysql.h>

#include "Log.h"

#include "sql.h"

MYSQL *mysql_connection;
int is_connected = 0;
 
int sql_connect(const char* server, const char* username, const char* password)
{
	if (is_connected)
	{
		Log(LOG_ALERT, "%s called when already connected.", __func__);
	}

	mysql_connection = mysql_init(NULL);

	if (mysql_connection == NULL) 
	{
		Log(LOG_ERROR, "Failed to initiate mysql. %s", mysql_error(mysql_connection));
		return -1;
	}
	Log(LOG_DEBUG, "mysql initiated.");

	if (mysql_real_connect(mysql_connection, server, username, password, NULL, 0, NULL, 0) == NULL) 
	{
		Log(LOG_ERROR, "Failed to connect to mysql db. %s", mysql_error(mysql_connection));
		mysql_close(mysql_connection);
		return -1;
	}
	Log(LOG_DEBUG, "connected to mysql db.");
	is_connected = 1;
	return 0;
}

int sql_is_connected()
{
	return is_connected;
}

int sql_query(const char* query)
{
	if (!is_connected) {
		Log(LOG_ALERT, "%s called when not connected.", __func__);
		return -1;
	}

	if (mysql_query(mysql_connection, query)) 
	{
		Log(LOG_ERROR, "query \"%s\" failed with error. %s", query, mysql_error(mysql_connection));
		return -1;
	}

	return 0;
}

int sql_close() 
{
	if (is_connected)
	{
		mysql_close(mysql_connection);
		Log(LOG_INFO, "Closing sql connection");
	}
	is_connected = 0;
	return 0;
}