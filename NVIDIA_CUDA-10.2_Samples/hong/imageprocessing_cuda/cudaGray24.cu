#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> /* USHRT_MAX 상수를 위해서 사용한다. */
#include <math.h>
#include <iostream>
#include "bmpHeader.h"
/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)
#define widthbytes(bits) (((bits)+31)/32*4)
typedef unsigned char ubyte;

//Cuda kernel for converting RGB image into a GreyScale image

__global__ void convertToGray(ubyte *rgb, ubyte *gray, int height, int width, int elemSize) {
		int x = threadIdx.x + blockIdx.x * blockDim.x;
		int y = threadIdx.y + blockIdx.y * blockDim.y;
		int size = width*elemSize;
		int offset = x*elemSize+(y*size);

		if( x < width && y < height){
				int r = rgb[offset + 2];
				int g = rgb[offset + 1];
				int b = rgb[offset + 0];
				gray[offset] = gray[offset+1] = gray[offset+2] = r * 0.299f + g * 0.587f + b * 0.114f;
		}

}
int main(int argc, char** argv)
{
		FILE* fp;
		BITMAPFILEHEADER bmpHeader; /* BMP FILE INFO */
		BITMAPINFOHEADER bmpInfoHeader; /* BMP IMAGE INFO */
		ubyte *inimg, *outimg;
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
		int elemSize = bmpInfoHeader.biBitCount/8.;
		int stride = bmpInfoHeader.biWidth * elemSize;
		int imageSize = stride * bmpInfoHeader.biHeight;
		/* 이미지의 해상도(넓이 × 깊이) */

		printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
		printf("Bit Count : %d(%d:%d)\n", bmpInfoHeader.biBitCount, elemSize, stride);
		printf("Image Size : %d\n", imageSize);
		inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
		outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
		fread(inimg, sizeof(ubyte), imageSize, fp);
		fclose(fp);

		ubyte *d_inimg = NULL, *d_outimg = NULL;

		cudaMalloc(&d_inimg, sizeof(ubyte) * imageSize);
		cudaMalloc(&d_outimg, sizeof(ubyte) * imageSize);

		cudaMemset(d_outimg, 0, sizeof(ubyte) * imageSize);
		cudaMemcpy(d_inimg, inimg, sizeof(ubyte) * imageSize, cudaMemcpyHostToDevice);

		const dim3 dimGrid((int)ceil((bmpInfoHeader.biWidth/32)), (int)ceil((bmpInfoHeader.biHeight)/16));
		const dim3 dimBlock(32, 16);

		convertToGray<<<dimGrid, dimBlock>>>(d_inimg, d_outimg, bmpInfoHeader.biHeight, bmpInfoHeader.biWidth, elemSize);
		cudaMemcpy(outimg, d_outimg, sizeof(ubyte) * imageSize, cudaMemcpyDeviceToHost);

		cudaFree(d_outimg);
		cudaFree(d_inimg);

		/***** write bmp *****/
		if((fp=fopen(argv[2], "wb"))==NULL) {
				fprintf(stderr, "Error : Failed to open file...₩n");
				return -1;
		}

		bmpInfoHeader.biBitCount = 24;
		/* BITMAPFILEHEADER 구조체의 데이터 */
		fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
		/* BITMAPINFOHEADER 구조체의 데이터 */
		fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
		fwrite(outimg, sizeof(ubyte), imageSize, fp);

		fclose(fp);

		free(inimg);
		free(outimg);
		return 0;
}
