#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpHeader.h"

#define RGB 3

int main(int argc, char** argv)
{
	FILE *fp, *after;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	RGBQUAD palrgb[256];

	char input[128], output[128];

	unsigned char *inimg, *outimg;
	int row, height, imagesize;

	strcpy(input, argv[1]);
	strcpy(output, argv[2]);

	if((fp = fopen(input, "rb")) == NULL) {
		perror(argv[1]);
		return -1;
	}

	if((after = fopen(output, "w")) == NULL) {
		perror(argv[2]);
		return -1;
	}  

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	row = bmpInfoHeader.biWidth * RGB;
	height = bmpInfoHeader.biHeight;
	imagesize = row * height;

	inimg = malloc(sizeof(unsigned char)*imagesize);
	outimg = malloc(sizeof(unsigned char)*imagesize);

	fread(inimg, sizeof(unsigned char) *imagesize, 1, fp);

	printf("fread : %ld\n", sizeof(unsigned char)*imagesize );

	fclose(fp);

	printf("bmp Image : %d X %d\n",bmpInfoHeader.biWidth, height);
	printf("image bit : %d\n",bmpInfoHeader.biBitCount);
	printf("image size : %d\n", imagesize);

	//2 Dimensional Array
	//for(int j = 0; j<height; j++){
	//	for(int i = 0; i<row; i++){
	//		outimg[i*RGB+(j*row)] = inimg[i*RGB+(j*row)];
	//		outimg[i*RGB+(j*row+1)] = inimg[i*RGB+(j*row+1)];
	//		outimg[i*RGB+(j*row+2)] = inimg[i*RGB+(j*row+2)];
	//	}
	//}

	// 1 Dimensional Array
	for(int i = 0; i < imagesize; i+= RGB){
		outimg[i] = inimg[i];
		outimg[i+1] = inimg[i+1];
		outimg[i+2] = inimg[i+2];
		printf("%d %d %d\n", outimg[i], outimg[i+1], outimg[i+2]);
	}
	
	
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, after );
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1 , after );
	fwrite(outimg, sizeof(unsigned char) * imagesize ,1, after);

	fclose(after);


	return 0;
}
