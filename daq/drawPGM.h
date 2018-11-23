#ifndef _DRAWPGM_H_
#define _DRAWPGM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
            size_t width;
            size_t height;
            unsigned char *data;
} Image;

Image * image_new (size_t width, size_t height)
{
            Image *image;

            image = new Image;
            image->width = width;
            image->height = height;
            image->data = new unsigned char[width * height];

            return image;
}

void image_free (Image *image)
{
            free (image->data);
            free (image);
}

void image_fill (Image *image,
                    unsigned char value)
{
            memset (image->data, value, image->width * image->height);
}

void image_set_pixel (Image *image,
                            ssize_t x,
                            ssize_t y,
                            unsigned char value)
{
            size_t tx, ty;
            unsigned char *p;

            tx = x;
            ty = image->height - y -1;

            p = image->data + (ty * image->width) + tx;

            *p = value;
}

void image_save (const Image *image,
                    const char *filename)
{
            FILE *out;

            out = fopen (filename, "wb");
            if (!out)
                            return;

            fprintf (out, "P5\n");
            fprintf (out, "%zu %zu\n", image->width, image->height);
            fprintf (out, "255\n");

            fwrite (image->data, 1, image->width * image->height, out);

            fclose (out);
}
/* ************* Example **************
int main (int argc, char *argv[])
{
	int i=0;
            Image *image;
            image = image_new (80, 80);
            image_fill (image, 0xff);
	while(i<80){
                      image_set_pixel(image,i,i,0x00);
			i++;
	}
            image_save (image, "jinkai.pgm");
            image_free (image);

            return 0;
}
************************************* */

#endif
