/**
 * download.c
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: October 25, 2014
 *
 * Uses curl to download stuff over http
 *
 */
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/easy.h>

#include "Log.h"

#include "download.h"

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t writeToMemoryFunction(void *ptr, size_t size, size_t nmemb, char *userp)
{
	  size_t realsize = size * nmemb;
	  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	  mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	  if(mem->memory == NULL) {
	    /* out of memory! */
	    Log(LOG_ERROR, "not enough memory (realloc returned NULL)\n");
	    return 0;
	  }

	  memcpy(&(mem->memory[mem->size]), ptr, realsize);
	  mem->size += realsize;
	  mem->memory[mem->size] = 0;

	  return realsize;
}

//-1 fail, 0 success, malloc's out
int httpGetBasicAuthToMemory(char **out, const char* url, const char* username, const char* password)
{
	Log(LOG_DEBUG, "Attempting to download %s", url);
	CURL *curl;
	struct MemoryStruct msChunk;
	int iError;
	//Initiate cURL
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (!curl)
	{
		Log(LOG_ERROR, "Failed to initiate curl. The request will not be completed.\n");
		return -1;
	}
	msChunk.memory = (char *)malloc(1);
	msChunk.size = 0;

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToMemoryFunction);
	if (strlen(username) && strlen(password))
	{
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
	}
	curl_easy_setopt(curl, CURLOPT_USERAGENT, MAL_USER_AGENT);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&msChunk);

	iError = curl_easy_perform(curl);
	if (iError != CURLE_OK)
	{
		Log(LOG_ERROR, "Curl failed with error %d.\n", iError);
		free(msChunk.memory);
		return -1;
	}
	curl_easy_cleanup(curl);

	*out = msChunk.memory;
	return 0;
}