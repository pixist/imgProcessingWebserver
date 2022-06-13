#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char* getImage(char *URL, char *filename);
