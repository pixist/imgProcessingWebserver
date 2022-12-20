#include "get_image.h"
// compile with $ gcc get_image.c -o d.o -lcurl
char *getImage(char *URL, char *filename)
{
    CURL *curl;
    FILE *fp;
    int result;
    char *ret;
    int des_size = strlen(URL) + 8;
    char updatedURL[des_size * 2];
    memset(updatedURL, 0, strlen(updatedURL));
    strcat(strcat(updatedURL, "https://"), URL);

    struct stat st = {0};

    if (stat("cache", &st) == -1)
    {
        mkdir("cache", 0700);
    }

    fp = fopen(filename, "wb");
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, updatedURL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); // 404 handle

    result = curl_easy_perform(curl);

    if (result == CURLE_OK)
    {
        printf("Download Successful!\n");
        ret = "200 OK";
    }
    else
    {
        ret = strdup(curl_easy_strerror(result));
        printf("ERROR: %s\n", ret);
    }
    fclose(fp);

    curl_easy_cleanup(curl);
    return ret;
}
/*
int main(int argc, char const* argv[])
{ // "https://imagemagick.org/MagickWand/logo_resize.jpg"
    getImage("upload.wikimedia.org/wikipedia/commons/thumb/b/b4/Flag_of_Turkey.svg/90px-Flag_of_Turkey.svg.png", "cache/download2.png");
}
*/
