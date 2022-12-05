#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) 
{
	FILE* fp; 
	BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
	BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
	RGBQUAD *palrgb;
	ubyte *inimg, *outimg, *padimg;
	int x, y, z, imageSize;

	if(argc != 3) {
		fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
		return -1;
	}

	/***** read bmp *****/ 
	if((fp=fopen(argv[1], "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	/* BITMAPFILEHEADER 구조체의 데이터 */
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	/* BITMAPINFOHEADER 구조체의 데이터 */
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	/* 트루 컬러를 지원하면 변환할 수 없다. */
	if(bmpInfoHeader.biBitCount != 24) {
		perror("This image file doesn't supports 24bit color\n");
		fclose(fp);
		return -1;
	}

	int elemSize = bmpInfoHeader.biBitCount/8;
	int size = bmpInfoHeader.biWidth*elemSize;
	int padsize = (bmpInfoHeader.biWidth+2)*elemSize;
	imageSize = size * bmpInfoHeader.biHeight; 

	/* 이미지의 해상도(넓이 × 깊이) */
	printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
	printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
	printf("Image Size : %d\n", imageSize);

	inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
	outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);

	padimg = (ubyte*)malloc(sizeof(ubyte)* (imageSize + 
				((bmpInfoHeader.biWidth + bmpInfoHeader.biHeight + 2) *2*elemSize)) );

	printf("padimg : %ld \n",  sizeof(ubyte)* (imageSize +
		    ((bmpInfoHeader.biWidth + bmpInfoHeader.biHeight + 2) *2)));
	printf("inimg : %ld \n",  sizeof(ubyte)* (imageSize));
	
	fread(inimg, sizeof(ubyte), imageSize, fp); 
	fclose(fp);

	memset(padimg, 0, sizeof(ubyte) *
			(imageSize + ((bmpInfoHeader.biWidth + bmpInfoHeader.biHeight+2)*2)) );
	//padimg -> image
	for(y = 0; y< bmpInfoHeader.biHeight +2; y++){
		for(x = 0; x<bmpInfoHeader.biWidth +2; x++){
			if(y == 0 || y == bmpInfoHeader.biHeight +1
			   || x == 0 || x == bmpInfoHeader.biWidth +1){ }
			else {
				padimg[y*padsize + 3*x+0] = inimg[size*(y-1) + 3*(x-1) +0];
				padimg[y*padsize + 3*x+1] = inimg[size*(y-1) + 3*(x-1) +1];
				padimg[y*padsize + 3*x+2] = inimg[size*(y-1) + 3*(x-1) +2];			
			}
		}
	}
	
	float kernel[3][3] = { {-1, -1, -1},
		{-1 , 9, -1},
		{ -1, -1, -1} };
	//padimg * kernel
	for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) { 
		for(x = 1; x < (bmpInfoHeader.biWidth + 1) * elemSize; x+=elemSize) {
			for(z = 0; z < elemSize; z++) {
				float sum = 0.0;
				for(int i = -1; i < 2; i++) {
					for(int j = -1; j < 2; j++) {
						sum += kernel[i+1][j+1]*inimg[(x-i*elemSize)+(y-j)*size+z];
					}
				}
				padimg[x+y*padsize+z] = clip(sum,0,255);
			}
		}
	}         

	memset(outimg, 0, sizeof(ubyte)*imageSize);
	for(y = 0; y< bmpInfoHeader.biHeight; y++){
		for(x = 0; x<bmpInfoHeader.biWidth; x++){
				outimg[y*size + 3*x+0] = padimg[padsize*(y+1) + 3*(x+1) +0];
				outimg[y*size + 3*x+1] = padimg[padsize*(y+1) + 3*(x+1) +1];
				outimg[y*size + 3*x+2] = padimg[padsize*(y+1) + 3*(x+1) +2];			
		}
	}

	
	/***** write bmp *****/ 
	if((fp=fopen(argv[2], "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}

	/* BITMAPFILEHEADER 구조체의 데이터 */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	/* BITMAPINFOHEADER 구조체의 데이터 */
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	//fwrite(inimg, sizeof(ubyte), imageSize, fp); 
	fwrite(outimg, sizeof(ubyte), imageSize, fp);

	fclose(fp); 

	free(inimg); 
	free(padimg);
	free(outimg);
	return 0;
}
