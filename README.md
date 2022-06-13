# imgProcessingWebserver
Webserver that supports image processing functions in C

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Configuration


INTRODUCTION
------------
This project includes following features:
 * Image resize, grayscale and rotation on the fly.
 * Error handling for unsupported input, invalid links, invalid filetypes.

The usage for the webserver is as follows:
http://<hostname>:<port>/v1/<operation>:<parameters>/.../url:<image url without the protocol>

An example for resize would be like this:
http://localhost:8080/v1/resize:640x360/url:upload.wikimedia.org/wikipedia/en/a/a9/Example.jpg

For grayscale, change resize:640x360 with grayscale

For rotate, change resize:640x360 with rotate:degrees, i.e, rotate:90 for 90 degrees rotation.

REQUIREMENTS
------------
This project requires the following libraries and their dependencies:

 * [libcurl](https://curl.se/libcurl/)
 * [imageMagick](https://imagemagick.org/index.php)


CONFIGURATION
-------------

Program does not take any arguments during runtime. It can be compiled
with the following line on the terminal.

$ gcc Server.c -o server.o get_image.c -lcurl image_proc.c `pkg-config --cflags --libs MagickWand`
  
Compiled version is also included in the repository as server.o 
