#include <stdio.h>
#include <stdlib.h>
#include <wand/magick_wand.h>
int resize(const char *filename, int width_des, int height_des);
int grayscale(const char *filename);
int rotate(const char *filename, double degrees);
