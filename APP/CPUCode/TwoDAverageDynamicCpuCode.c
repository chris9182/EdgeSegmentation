/**
 * Document: MaxCompiler Tutorial (maxcompiler-tutorial.pdf)
 * Chapter: 8      Example: 5      Name: Two-dimensional average dynamic
 * MaxFile name: TwoDAverageDynamic
 * Summary:
 *       Averages within an 8-point window.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <MaxSLiCInterface.h>
#include "Maxfiles.h"

void loadImage(
	char *filename,
	int **dest,
	int *width,
	int *height,
	int grayscale)
{
	// hack to make the buffer big enough
	char buffer[200];
	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		printf("Error opening file %s.", filename);
		exit(1);
	}
	fgets(buffer, 200, file); // Type
	// assume no comment for now
	fscanf(file, "%d %d\n", width, height);
	fgets(buffer, 200, file); // Max intensity

	*dest = malloc((*width) * (*height) * sizeof(int));

	int pixelsRead = 0;
	int pixel = 0;
	int currComponent = 0;

	for (int i = 0; i < ((*width) * (*height) * 3); i++) {
		int v;
		int got = fscanf(file, "%d", &v);
		if (got == 0) {
			printf("Unexpected end of file after reading %d color values", i);
			exit(1);
		}
		switch (currComponent)
		{
			case 0:
				pixel = v;
				currComponent++;
				break;
			case 1:
				pixel = (pixel << 8) | (v & 0xFF);
				currComponent++;
				break;
			case 2:
				pixel = (pixel << 8) | (v & 0xFF);
				currComponent = 0;
				if (grayscale)
					pixel = ((pixel >> 16) & 0xff) * 3 / 10 + ((pixel >> 8)
						& 0xff) * 59 / 100 + ((pixel) & 0xff) * 11 / 100;
				(*dest)[pixelsRead] = pixel;
				pixelsRead++;
				break;
		}
	}
	fclose(file);
}

/**
 * \brief Writes an image as a color .ppm file
 *
 * \param [in] filename Name of the .ppm file to save
 * \param [in] data Buffer containing the image data
 * \param [in] width Width of the image in pixels in the buffer
 * \param [in] height Height of the image in pixels in the buffer
 * \param [in] grayscale Whether the image in the buffer is grayscale (1) or
 *             color (!=1)
 */
void writeImage(char *filename, int *data, int width, int height, int grayscale)
{
	FILE *file = fopen(filename, "w");

	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", width, height);
	fprintf(file, "255\n");

	for (int i = 0; i < width * height; i++) {
		if (grayscale) {
			//	if (data[i] > 255)
			//		printf("Warning: Out of range data. %d at pixel %d\n.", data[i], i);
			data[i] = data[i] > 255 ? 255 : data[i];
			//	if (data[i] < 0)
			//		printf("Warning: Data < 0. %d at pixel %d\n.", data[i], i);
			data[i] = data[i] < 0 ? 0 : data[i];

			for (int j = 0; j < 3; j++)
				fprintf(file, "%d\n", (int) data[i]);
		} else {
			fprintf(file, "%d\n", ((int) data[i]) >> 16);
			fprintf(file, "%d\n", (((int) data[i]) >> 8) & 0xff);
			fprintf(file, "%d\n", (((int) data[i])) & 0xff);
		}
	}
	fclose(file);
}

void generateInputData(int *dataIn, int *expectedOut, int nx, int ny)
{
	for (int i = 0; i < nx * ny; i++) {
		dataIn[i] = 0;
		expectedOut[i] = 0;
	}
	dataIn[nx * (ny / 2) + nx / 2] = 9;
	dataIn[0] = 1;
	dataIn[47] = 47;
	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
			expectedOut[nx * (ny / 2 + y) + (nx / 2 + x)] = 1.0;
}

void print2dData(int *data, char* name, int nx, int ny)
{
	printf("\n%s\n", name);
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			printf("%i ", data[y * nx + x]);
		}
		printf("\n");
	}
}

void getedges(int *data, int *edges, int nx, int ny)
{
    for (int i=0;i<nx;i++){edges[i]=0;edges[(ny-1)*nx+i]=0;}
    for (int i=0;i<ny;i++){edges[i*nx]=0;edges[i*nx+nx-1]=0;}
	for (int y = 1; y < ny-1; y++) {
		for (int x = 1; x < nx-1; x++) {
		    edges[y * nx + x]=255;
			for(int i=-1;i<2;i++){
		    	for(int j=-1;j<2;j++){
    		    	if(data[y * nx + x]!=data[(y+i) * nx + (x+j)]){
    		    	   edges[y * nx + x]=0;
    		    	   goto next;
    		    	}
		    	}
			}
		   next:
		   edges;
		}
	}
}

int main()
{
	printf("Loading image.\n");
	int32_t *inImage;
	int width = 0, height = 0;
	loadImage("lena.ppm", &inImage, &width, &height, 1);
	writeImage("lena_grayscale.ppm", inImage, width, height, 1);
	size_t sizeBytes = width * height * sizeof(int);
	uint32_t *counterOut = malloc(sizeBytes);

	int dataSize = width * height * sizeof(int32_t);
	// Allocate a buffer for the output image
	int32_t *outImage = malloc(dataSize);
	int32_t *edges = malloc(dataSize);

	printf("Running Kernel.\n");
	TwoDAverageDynamic(width * height, width, inImage, counterOut, outImage);
	writeImage("lena_edit.ppm", outImage, width, height, 1);
    getedges(outImage, edges, width, height);
	printf("Saving image.\n");
	writeImage("lena_edges.ppm", edges, width, height, 1);
	

	printf("Exiting\n");
	
	
	return 0;
}
