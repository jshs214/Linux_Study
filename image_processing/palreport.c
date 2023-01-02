#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpHeader.h"

int main(int argc, char** argv)
{
	FILE *fp;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	RGBQUAD palrgb[256];

	char input[100];

	unsigned char *inimg;
	int row, height, imagesize,elemSize;

	strcpy(input, argv[1]);

	if((fp = fopen(input, "rb")) == NULL) {
		perror(argv[1]);
		return -1;
	}


	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	elemSize= bmpInfoHeader.biBitCount /8;
	row = bmpInfoHeader.biWidth * elemSize;
	height = bmpInfoHeader.biHeight;
	imagesize = row * height;

	/* Use palete 256 */
	for(int i = 0; i< 256; i++){
		palrgb[i].rgbBlue = i;	
		palrgb[i].rgbGreen = i;	
		palrgb[i].rgbRed = i;	
	}

	inimg = malloc(sizeof(unsigned char)*imagesize);

	fread(inimg, sizeof(unsigned char) *imagesize, 1, fp);
	printf("fread : %ld\n", sizeof(unsigned char)*imagesize );

	fclose(fp);


	for(int j = 0; j < height; j ++){
		for(int i = 0; i < row; i +=elemSize){
			printf("(%d %d %d)\n", inimg[i + (j * row)], inimg[i + (j * row+1)], inimg[i + (j * row +2)]);
		}
	}

	free(inimg);

	return 0;
}
