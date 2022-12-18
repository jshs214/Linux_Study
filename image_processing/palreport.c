#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpHeader.h"
#include <time.h>


int main(int argc, char** argv)
{
	FILE *fp, *after;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	RGBQUAD palrgb[256];
	clock_t start, end;
    double result;


	char input[100], output[100];

	unsigned char *inimg, *outimg;
	int row, height, imagesize,elemSize;

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

	elemSize= bmpInfoHeader.biBitCount /8;
	row = bmpInfoHeader.biWidth * elemSize;
	height = bmpInfoHeader.biHeight;
	imagesize = row * height;
	
	 start = clock(); // 수행 시간 측정 시작

	/* Use palete 256 */
	for(int i = 0; i< 256; i++){
		palrgb[i].rgbBlue = i;	
		palrgb[i].rgbGreen = i;	
		palrgb[i].rgbRed = i;	
	}

	inimg = malloc(sizeof(unsigned char)*imagesize);
	outimg = malloc(sizeof(unsigned char)*imagesize);
	
	fread(inimg, sizeof(unsigned char) *imagesize, 1, fp);
	printf("fread : %ld\n", sizeof(unsigned char)*imagesize );

	fclose(fp);

	printf("bmp Image : %d X %d\n",bmpInfoHeader.biWidth, height);
	printf("image bit : %d\n",bmpInfoHeader.biBitCount);
	printf("image size : %d\n", imagesize);

	//2 Dimensional Array
	for(int j = 0; j<height; j++){
		for(int i = 0; i<row; i++){
			outimg[i*elemSize+(j*row)] = inimg[i*elemSize+(j*row)];
			outimg[i*elemSize+(j*row+1)] = inimg[i*elemSize+(j*row+1)];
			outimg[i*elemSize+(j*row+2)] = inimg[i*elemSize+(j*row+2)];
		}
	}

	// 1 Dimensional Array
//	for(int i = 0; i < imagesize; i+= elemSize){
//		outimg[i] = inimg[i];
//		outimg[i+1] = inimg[i+1];
//		outimg[i+2] = inimg[i+2];
//	}
	
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
					sizeof(RGBQUAD) * 256;
	
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, after );
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER) , 1 , after );
	fwrite(&palrgb, sizeof(RGBQUAD), 256, after);
	
	fwrite(outimg, sizeof(unsigned char) * imagesize ,1, after);

	fclose(after);
	
	free(inimg);
	free(outimg);

	end = clock(); //시간 측정 끝
    result = (double)(end - start);

    // 결과 출력
    printf("%f", result / CLOCKS_PER_SEC);


	return 0;
}
