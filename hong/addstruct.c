#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "bmpHeader.h"

#define BYTE unsigned char

#define widthbytes(bits) (((bits)+31)/32*4)


inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
	return(value > max ? max : value < min ? min : value);
}

int main(int argc, char** argv) {
	FILE* fp;
	BGBQUAD palrgb[256];

	/* BMP FILE INFO */
	BITMAPFILEHEADER bmpHeader;

	/* BMP IMAGE INFO */
	BITMAPINFOHEADER bmpInfoHeader;

	char input[128], output[128];

	int i, j, size;
	float srcX, srcY;
	int index;
	float r, g, b, gray;
	int graysize;
	int index2;

	unsigned char* grayimg, * inimg, * outimg;

	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]);
	strcpy(output, argv[2]);


	if ((fp = fopen(input, "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...₩n");
		return -1;
	}

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	size = widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);
	graysize = widthbytes(8 * bmpInfoHeader.biWidth);

	if (!bmpInfoHeader.SizeImage)
		bmpInfoHeader.SizeImage = bmpInfoHeader.biHeight * size;

	inimg = (BYTE*)malloc(sizeof(BYTE) * bmpInfoHeader.SizeImage);
	outimg = (BYTE*)malloc(sizeof(BYTE) * bmpInfoHeader.SizeImage);
	fread(inimg, sizeof(BYTE), bmpInfoHeader.SizeImage, fp);

	fclose(fp);

	for (i = 0; i < bmpInfoHeader.biHeight * 3; i += 3) {
		for (j = 1; j < bmpInfoHeader.biWidth * 3; j += 3) {
			int b = inimg[j + (i * bmpInfoHeader.biWidth + 0)];
			int g = inimg[j + (i * bmpInfoHeader.biWidth + 1)];
			int r = inimg[j + (i * bmpInfoHeader.biWidth + 2)];

			b += 50;
			g += 50;
			r += 50;

			outimg[j + bmpInfoHeader.biWidth * i + 0] = clip(b, 0, 255);
			outimg[j + bmpInfoHeader.biWidth * i + 1] = clip(g, 0, 255);
			outimg[j + bmpInfoHeader.biWidth * i + 2] = clip(r, 0, 255);
		};
	};

	size = widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);
	bmpInfoHeader.SizeImage = bmpInfoHeader.biHeight * size;
	bmpHeader.bfOffBits += 256 * sizeof(BGBQUAD);

	if ((fp = fopen(output, "wb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...₩n");
		return -1;
	}

	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(palrgb, sizeof(unsigned int), 256, fp);

	fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);

	free(inimg);
	free(outimg);

	fclose(fp);

	return 0;
}
