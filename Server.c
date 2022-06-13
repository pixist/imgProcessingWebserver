// $ gcc Server.c -o s6.o get_image.c -lcurl image_proc.c `pkg-config --cflags --libs MagickWand`
#include "Server.h"

int main(int argc, char const* argv[])
{
	int server_fd, connfd, n;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
    uint8_t buffer[MAXLINE+1];
    uint8_t recvline[MAXLINE+1];
    int fdimg;
	char file_dir[86];
	char imageheader[] =
	"HTTP/1.1 200 Ok\r\n"
	"Content-Type: image/jpeg\r\n\r\n";
	char* downloadStatus;
	int imgsize, resizeStatus;
	struct stat st;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
		== 0) {
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}

    bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	// restart port
	int flag = 1;
	if (-1 == setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {
		perror("setsockopt fail");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (SA*)&address, sizeof(address))< 0) {
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 10) < 0) {
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}

    for(;;)
    {
        struct sockaddr_in addr;
        socklen_t addr_len;

        printf("Waiting for connection on port %d\n",PORT);
        fflush(stdout);
        connfd = accept(server_fd, (SA *) NULL, NULL);

        memset(recvline, 0, MAXLINE);

        while ((n = read(connfd, recvline, MAXLINE-1))>0)
        {
            fprintf(stdout, "\n\n===== HTTP REQUEST BELOW =====\n%s",recvline);

            if (recvline[n-1] == '\n') {
                break;
            }
            memset(recvline, 0, MAXLINE);
        }
        if (n<0)
        {
        	perror("negative byte");
        	exit(EXIT_FAILURE);
        }
        char* httpline, *request_type, *requestUrl, *token;
	    const char *request[3], *arg2[3], *arg3[3];
	    char *errorCode = "200 OK";
		printf("HTTP request received. Request Length: %ld\n", strlen(recvline));
		if (strlen(recvline) == 0) continue;
		token = strtok(strdup(recvline), "\n");
	    httpline = strdup(token);
		token = strtok(strdup(httpline), " ");
	    request_type = strdup(token);
	    if ((strcmp(request_type, "GET") == 0) && (token != NULL) && (strncmp(httpline, "GET / HTTP/1.1", 13) != 0))
	    {
	        token = strtok(NULL, " ");
	        requestUrl = strdup(token);
			token = strtok(strdup(requestUrl), "/");
	        request[0] = strdup(token);
	        token = strtok(NULL, "/");

			if ((strcmp(request[0], "v1") == 0) && (token != NULL))
	        {
				printf("v1 correct\n");
				request[1] = strdup(token);
	            token = strtok(NULL, " ");
	            request[2] = strdup(token);

	            token = strtok(strdup(request[1]), ":");
	            arg2[0] = strdup(token);
	            if ((strcmp(arg2[0], "resize") == 0) && (token != NULL))
				{
					printf("resize correct\n");
	                token = strtok(NULL, "x");
	                arg2[1] = strdup(token);
	                token = strtok(NULL, "x");
	                arg2[2] = strdup(token);
	            }
				else if ((strcmp(arg2[0], "grayscale") == 0) && (token != NULL))
				{printf("grayscale correct\n");}
				else if ((strcmp(arg2[0], "rotate") == 0) && (token != NULL))
				{
					printf("rotate correct\n");
	                token = strtok(NULL, ":");
	                arg2[1] = strdup(token);
				}

				else errorCode = "404 Not Found";

	            token = strtok(strdup(request[2]), ":");
	            arg3[0] = strdup(token);
	            if ((strcmp(arg3[0], "url") == 0) && (token != NULL))
				{
					printf("url correct\n");
	                token = strtok(NULL, ":");
	                arg3[1] = strdup(token);
	                token = strtok(strdup(arg3[1]), "/");
	                while( token != NULL )
	                {
	                    arg3[2] = strdup(token);
	                    token = strtok(NULL, "/");
	                }
	            } else errorCode = "404 Not Found";
	        } else errorCode = "404 Not Found";
	    } else errorCode = "400 Bad Request";
		// 501 not implemented for other http methods

		if(strcmp(errorCode, "200 OK") == 0)
		{

			memset(file_dir,0,strlen(file_dir));
    		strcat(strcat(file_dir, "cache/"), arg3[2]);
			printf("Accessed URL: %s\n", arg3[1]);
			downloadStatus = getImage(strdup(arg3[1]), file_dir);
			if ((strcmp(downloadStatus, "200 OK") == 0) && (strcmp(arg2[0], "resize") == 0))
			{
				resizeStatus = resize(file_dir, atoi(arg2[1]), atoi(arg2[2]));
				if (resizeStatus == 0) errorCode = "415 Unsupported Media Type";
			}
			else if ((strcmp(downloadStatus, "200 OK") == 0) && (strcmp(arg2[0], "grayscale") == 0))
			{
				resizeStatus = grayscale(file_dir);
				if (resizeStatus == 0) errorCode = "415 Unsupported Media Type";
			}
			else if ((strcmp(downloadStatus, "200 OK") == 0) && (strcmp(arg2[0], "rotate") == 0))
			{
				resizeStatus = rotate(file_dir, atoi(arg2[1]));
				if (resizeStatus == 0) errorCode = "415 Unsupported Media Type";
			}
			else errorCode = "204 No Content";
			printf("%s\n",errorCode);
			if (strcmp(errorCode, "200 OK") == 0)
			{
				printf("Sending Image!\n");
	    		// Send header first: (Note, should loop in case of partial write or EINTR)
	    		write(connfd, imageheader, sizeof(imageheader) - 1);
	    		// Send image content
		        fdimg = open(file_dir, O_RDONLY);
				// Get img Size
				stat(file_dir, &st);
				imgsize = st.st_size;
				printf("Image size: %d\n",imgsize);
				// Send image
		        int sent = sendfile(connfd, fdimg, NULL, imgsize);
		        printf("Sent content size: %d\n", sent);
		        close(fdimg);
			}

	    }
	    if(strcmp(errorCode, "200 OK") != 0){
			snprintf((char*)buffer, sizeof(buffer), "HTTP/1.0 %s\r\n\r\n%s",errorCode,errorCode);
			printf("Error message sent\n");
	    }
		write(connfd, (char*)buffer, strlen((char*)buffer));
        close(connfd);
    }
}
