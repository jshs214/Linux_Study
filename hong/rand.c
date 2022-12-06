#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bmpHeader.h"

#define BYTE	unsigned char
#define BASE    16


#define widthbytes(bits)   (((bits)+31)/32*4)

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
	return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv)
{
	FILE *fp;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	RGBQUAD palrgb[256];

	char input[128], output[128];
	int noise=0;

	float r, g, b, gray;

	srand((unsigned int)time(NULL));

	int i, j, size, index;
	unsigned long histogram[256];
	unsigned char *inimg;
	unsigned char *outimg;

	strcpy(input, argv[1]);
	strcpy(output, argv[2]);
	noise = atoi(argv[3]);

	if((fp = fopen(input, "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	int elemSize = bmpInfoHeader.biBitCount/8;
	size = bmpInfoHeader.biWidth*elemSize;
	int imageSize = size * bmpInfoHeader.biHeight; 

	if(!bmpInfoHeader.SizeImage)
		bmpInfoHeader.SizeImage = bmpInfoHeader.biHeight * size;

	inimg = (BYTE*)malloc(sizeof(BYTE)* bmpInfoHeader.SizeImage);
	outimg = (BYTE*)malloc(sizeof(BYTE)* bmpInfoHeader.SizeImage);
	fread(inimg, sizeof(BYTE), bmpInfoHeader.SizeImage, fp);
	fclose(fp);

	printf(" %d X %d\n ImageSize :(%d)\n", 
			bmpInfoHeader.biWidth, bmpInfoHeader.biHeight,
			bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*elemSize);


	for(i = 0; i < imageSize; i++) {
		for(int z = 0; z<elemSize; z++)
			outimg[i+z] = inimg[i+z];
	};

	//rand
	for(i = 0; i < noise; i++) {	
		int randValue = rand()%255;
		int x = rand()%(bmpInfoHeader.biWidth*
				bmpInfoHeader.biHeight);
		for(int z = 0; z<elemSize; z++)
			outimg[x*elemSize+z] = clip(inimg[x*elemSize+z]+randValue,0,255);
	}



	//offset += 256*sizeof(BGBQUAD); 

	if((fp = fopen(output, "wb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}


	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	//	fwrite(palrgb, sizeof(unsigned int), 256, fp);
	fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);

	free(inimg);
	free(outimg);

	fclose(fp);

	return 0;
}
