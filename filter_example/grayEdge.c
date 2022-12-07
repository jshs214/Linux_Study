#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpHeader.h"

#define BYTE	unsigned char
#define BASE    16


#define widthbytes(bits)   (((bits)+31)/32*4)

int main(int argc, char** argv)
{
	FILE *fp;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	RGBQUAD palrgb[256];

	char input[128], output[128];

	float r, g, b, gray, gray2;

	int i, j, size, index, value;
	unsigned char *inimg;
	unsigned char *outimg;

	strcpy(input, argv[1]);
	strcpy(output, argv[2]);

	if((fp = fopen(input, "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}
        fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	size = widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);

	if(!bmpInfoHeader.SizeImage)
	       	bmpInfoHeader.SizeImage = bmpInfoHeader.biHeight * size;

	inimg = (BYTE*)malloc(sizeof(BYTE)* bmpInfoHeader.SizeImage);
	outimg = (BYTE*)malloc(sizeof(BYTE)* bmpInfoHeader.SizeImage);
	fread(inimg, sizeof(BYTE), bmpInfoHeader.SizeImage, fp);
	fclose(fp);

	printf("Image width : %d, height : %d(%d)\n", 
			bmpInfoHeader.biWidth, bmpInfoHeader.biHeight,
		       bmpInfoHeader.biWidth*bmpInfoHeader.biHeight);


	for(i = 0; i < bmpInfoHeader.biHeight; i++) {
		index = (bmpInfoHeader.biHeight-i-1) * size; 
		for(j = 0 ; j < bmpInfoHeader.biWidth; j++) { 
			r = (float)inimg[index+3*j+2];
			g = (float)inimg[index+3*j+1];
			b = (float)inimg[index+3*j+0];
			gray = (r*0.3F)+(g*0.59F)+(b*0.11F);
			
			r = (float)inimg[index+3*j+2 -3];
			g = (float)inimg[index+3*j+1 -3];
			b = (float)inimg[index+3*j+0 -3];
			gray2 = (r*0.3F)+(g*0.59F)+(b*0.11F);
			
			value = (int)(gray2 - gray);
			outimg[index+3*j] =(value > BASE) ? 255: 0;
			outimg[index+3*j+1] =(value > BASE) ? 255: 0;
		       	outimg[index+3*j+2] = (value > BASE) ? 255: 0;
		};
	};


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
