#include "image_proc.h"
// $ gcc -o p.o  image_proc.c `pkg-config --cflags --libs MagickWand`
int resize(const char *filename, int width_des, int height_des)
{
    MagickWand *m_wand = NULL;
    int width,height;
    unsigned char output;

	MagickWandGenesis();

	m_wand = NewMagickWand();
	// Read the image - all you need to do is change "logo:" to some other
	// filename to have this resize and, if necessary, convert a different file
	MagickReadImage(m_wand, filename);
    //FILE* fp = fopen(URL, "r");
    //MagickReadImageFile(m_wand, fp);

	// Resize the image using the Lanczos filter
	// The blur factor is a "double", where > 1 is blurry, < 1 is sharp
	// I haven't figured out how you would change the blur parameter of MagickResizeImage
	// on the command line so I have set it to its default of one.
	MagickResizeImage(m_wand,width_des,height_des,LanczosFilter,1);

	// Set the compression quality to 95 (high quality = low compression)
	MagickSetImageCompressionQuality(m_wand,95);

	/* Write the new image */
	output = MagickWriteImage(m_wand,filename);
    printf("Writing the resized image: %u \n", output);
	/* Clean up */
	if(m_wand)m_wand = DestroyMagickWand(m_wand);

	MagickWandTerminus();
    return output;
}

int grayscale(const char *filename)
{
    MagickWand *m_wand = NULL;
    unsigned char output;

	MagickWandGenesis();

	m_wand = NewMagickWand();
	// Read the image - all you need to do is change "logo:" to some other
	// filename to have this resize and, if necessary, convert a different file
	MagickReadImage(m_wand, filename);

    MagickQuantizeImage(m_wand,256,GRAYColorspace,0,0,0);

	// Set the compression quality to 95 (high quality = low compression)
	MagickSetImageCompressionQuality(m_wand,95);

	/* Write the new image */
	output = MagickWriteImage(m_wand,filename);
    printf("Writing the grayscaled image: %u \n", output);
	/* Clean up */
	if(m_wand)m_wand = DestroyMagickWand(m_wand);

	MagickWandTerminus();
    return output;
}
int rotate(const char *filename, double degrees)
{
    MagickWand *m_wand = NULL;
	PixelWand *c_wand = NULL;
    unsigned char output;

	MagickWandGenesis();

	m_wand = NewMagickWand();
	c_wand = NewPixelWand();
	PixelSetColor(c_wand,"white");
	// Read the image - all you need to do is change "logo:" to some other
	// filename to have this resize and, if necessary, convert a different file
	MagickReadImage(m_wand, filename);

    MagickRotateImage(m_wand,c_wand,degrees);
	// Set the compression quality to 95 (high quality = low compression)
	MagickSetImageCompressionQuality(m_wand,95);

	/* Write the new image */
	output = MagickWriteImage(m_wand,filename);
    printf("Writing the rotated image: %u \n", output);
	/* Clean up */
	if(m_wand)m_wand = DestroyMagickWand(m_wand);

	MagickWandTerminus();
    return output;
}
/*
int main(int argc, char const* argv[])
{ // "https://imagemagick.org/MagickWand/logo_resize.jpg"
    resize("cache/download.png", 480, 540);
}
*/
