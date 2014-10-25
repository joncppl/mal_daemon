/**
 * download.h
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 *
 * Uses curl to download stuff over http
 *
 */

#define MAL_USER_AGENT "api-indiv-A2E8711EE16CA9A96FC8A6AFB65A15DF"

//-1 fail, 0 success, malloc's out
int httpGetBasicAuthToMemory(char **out, const char* url, const char* username, const char* password);