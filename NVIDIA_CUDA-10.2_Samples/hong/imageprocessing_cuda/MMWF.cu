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

// change quick sort
__device__ void insertion(int a[], int n)
{
	int i, j;
	for(i = 1; i< n; i++){
		int tmp = a[i];
		for(j = i; j>0 && a[j-1] > tmp; j--)
			a[j] = a[j-1];
		a[j] = tmp;
	}
}

//Create a noise standard deviation
__global__ void convertToBlur(ubyte *rgb, ubyte *out, int width, int height, int elemSize) {
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int z = threadIdx.z + blockIdx.z * blockDim.z;

	int size = width*elemSize;
	int rgb_offset = (x*elemSize+(y*size));

	float offset_Average, offset_SD;
	int arr[9] = {0,}, sort_arr[9] ={0,};
	float sum = 0.0, sum_SD = 0.0, mask_SD = 0.0, medianValue = 0.0;
	int MMWF = 0.0;
	float sigma = 0.3*((3-1) * 0.5 - 1) + 0.8;	//Temporary noise standard deviation

	// inSide
	if ( (x > 0 && x < width-1) && (y >0 && y < height-1) ) {
		int cnt =0;
		// find sum of AdjacentValue
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				sum += rgb[(x+i)*elemSize +((y+j)*size) +z];
				sort_arr[cnt++] = rgb[(x+i)*elemSize + ((y+j)*size)+z];
			}
		}
		// find median_value after sorting
		insertion(sort_arr, 9);
		medianValue = sort_arr[4];

		offset_Average = sum/9;		//In mask average

		//find a standard deviation value
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				offset_SD =  pow( ( rgb[(x+i)*elemSize + ((y+j)*size)+z] - offset_Average), 2)  / 9 ;
				sum_SD += offset_SD;
			}
		}

		mask_SD = sqrt(sum_SD);
		float o = pow(mask_SD, 2);
		// sigma = Input noise_deviation
		float v = pow(sigma, 2);

		MMWF = medianValue + (1 - v/o) * abs(rgb[rgb_offset+z] - medianValue);

		out[rgb_offset +z] = LIMIT_UBYTE(MMWF);
	}
	//OutSide
	else{
		//LeftSide
		if(x ==0){
			//LeftTopVertex
			if(y==0){
				arr[0] = arr[1] = arr[3] = arr[4] = rgb[(x*elemSize)+(y*size)+z];
				arr[2] = arr[5] = rgb[(x*elemSize)+elemSize +(y*size)+z];
				arr[6] = arr[7] = rgb[(x*elemSize)+((y+1)*size)+z];
				arr[8] = rgb[(x*elemSize)+elemSize+((y+1)*size)+z];	
			}
			//LeftDownVertex
			else if(y==height-1){
				arr[0] = arr[1] = rgb[(x*elemSize)+((y-1)*size)+z];
				arr[2] = rgb[(x*elemSize)+elemSize+((y-1)*size)+z];
				arr[3] = arr[6] = arr[7] = arr[4] = rgb[(x*elemSize)+(y*size)+z];
				arr[8] = arr[5] = rgb[(x*elemSize)+elemSize+(y*size)+z];
			}

			//LeftSide
			else{
				arr[0] = arr[1] = rgb[(x*elemSize)+((y-1)*size)+z];
				arr[2] = rgb[(x*elemSize)+elemSize+((y-1)*size)+z];
				arr[3] = arr[4] = rgb[(x*elemSize)+(y*size)+z];
				arr[5] = rgb[(x*elemSize)+elemSize+(y*size)+z];
				arr[6] = arr[7] = rgb[(x*elemSize)+((y+1)*size)+z];
				arr[8] = rgb[(x*elemSize)+elemSize+((y+1)*size)+z];
			}

		}
		//RightSide
		else if(x==width-1){
			//RightTopVertex
			if(y==0){
				arr[0] = arr[3] = rgb[(x*elemSize)-elemSize+(y*size)+z];
				arr[1] = arr[2] = arr[5] = arr[4] = rgb[rgb_offset+z];
				arr[6] = rgb[(x*elemSize)-elemSize+((y-1)*size)+z];
				arr[7] = arr[8] = rgb[(x*elemSize)+((y+1)*size)+z];
			}
			//RightDownVertex
			else if(y==height-1){
				arr[0] = rgb[(x*elemSize)-elemSize+((y-1)*size)+z];
				arr[1] = arr[2] = rgb[(x*elemSize)-elemSize+((y-1)*size)+z];
				arr[3] = arr[6] = rgb[(x*elemSize)-elemSize+(y*size)+z];
				arr[4] = arr[5] = arr[7] = arr[8] = rgb[rgb_offset+z];
			}
			//RightSide
			else{
				arr[0] = rgb[(x*elemSize)-elemSize+((y-1)*size)+z];
				arr[1] = arr[2] = rgb[(x*elemSize)+((y-1)*size)+z];
				arr[3] = rgb[(x*elemSize)-elemSize+(y*size)+z];
				arr[4] = arr[5] = rgb[(x*elemSize)+(y*size)+z];
				arr[6] = rgb[(x*elemSize)-elemSize+((y+1)*size)+z];
				arr[7] = arr[8] = rgb[(x*elemSize)+((y+1)*size)+z];
			}
		}
		//TopSide
		else if( y==0){
			if(x!=0 && x!=width-1){
				arr[0] = arr[3] = rgb[(x*elemSize)-elemSize+(y*size)+z];
				arr[1] = arr[4] = rgb[rgb_offset+z];
				arr[2] = arr[5] = rgb[(x*elemSize)+elemSize+(y*size)+z];
				arr[6] = rgb[(x*elemSize)-elemSize+((y+1)*size)+z];
				arr[7] = rgb[(x*elemSize)+((y+1)*size)+z];
				arr[8] = rgb[(x*elemSize)+elemSize+((y+1)*size)+z];

			}
		}
		//BottomSide
		else if( y==height-1){
			if(x!=0 && x!=width-1){
				arr[0] = rgb[(x*elemSize)-elemSize+((y-1)*size)+z];
				arr[1] = rgb[(x*elemSize)+((y-1)*size)+z];
				arr[2] = rgb[(x*elemSize)+elemSize+((y-1)*size)+z];
				arr[3] = arr[6] = rgb[(x*elemSize)-elemSize+(y*size)+z];
				arr[4] = arr[7] = rgb[rgb_offset+z];
				arr[5] = arr[8] = rgb[(x*elemSize)-elemSize+(y*size)+z];
			}
		}	

		out[rgb_offset+z] = LIMIT_UBYTE(sum);
		int cnt =0;
		// find sum of AdjacentValue
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				sum += arr[cnt];
				sort_arr[cnt] = arr[cnt];
				cnt++;
			}
		}

		// find median_value after sorting
		insertion(sort_arr, 9);
		medianValue = sort_arr[4];
		offset_Average = sum/9;		//In mask average
		
		cnt =0;
		//find a standard deviation value
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				offset_SD =  pow( ( arr[cnt++] - offset_Average), 2)  / 9 ;
				sum_SD += offset_SD;
			}
		}

		mask_SD = sqrt(sum_SD);
		float o = pow(mask_SD, 2);
		// sigma = Input noise_deviation
		float v = pow(sigma, 2);

		MMWF = medianValue + (1 - v/o) * abs(rgb[rgb_offset+z] - medianValue);

		out[rgb_offset +z] = LIMIT_UBYTE(MMWF);
	}
}

int main(int argc, char** argv)
{
	FILE* fp;
	BITMAPFILEHEADER bmpHeader; /* BMP FILE INFO */
	BITMAPINFOHEADER bmpInfoHeader; /* BMP IMAGE INFO */
	//RGBQUAD *palrgb;
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
	//widthbytes(bits) (((bits)+31)/32*4)
	int imageSize = stride * bmpInfoHeader.biHeight;

	inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
	outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);

	fread(inimg, sizeof(ubyte), imageSize, fp);
	fclose(fp);

	ubyte *d_inimg = NULL, *d_outimg = NULL;
	//allocate and initialize memory on device
	cudaMalloc(&d_inimg, sizeof(ubyte) * imageSize);
	cudaMalloc(&d_outimg, sizeof(ubyte) * imageSize);
	cudaMemset(d_outimg, 0, sizeof(ubyte) * imageSize);
	//copy host rgb data array to device rgb data array
	cudaMemcpy(d_inimg, inimg, sizeof(ubyte) * imageSize, cudaMemcpyHostToDevice);

	//define block and grid dimensions
	const dim3 dimGrid((int)ceil((bmpInfoHeader.biWidth/32)), (int)ceil((bmpInfoHeader.biHeight)/4),1);
	const dim3 dimBlock(32, 4, elemSize);

	//execute cuda kernel
	convertToBlur<<<dimGrid, dimBlock>>>(d_inimg, d_outimg, bmpInfoHeader.biHeight, bmpInfoHeader.biWidth, elemSize);
	//copy computed gray data array from device to host
	cudaMemcpy(outimg, d_outimg, sizeof(ubyte) * imageSize, cudaMemcpyDeviceToHost);

	cudaFree(d_outimg);
	cudaFree(d_inimg);

	/***** write bmp *****/
	if((fp=fopen(argv[2], "wb"))==NULL) {
		fprintf(stderr, "Error : Failed to open file...₩n");
		return -1;
	}
	/*
	   palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
	   for(int x = 0; x < 256; x++) {
	   palrgb[x].rgbBlue = palrgb[x].rgbGreen = palrgb[x].rgbRed = x;
	   palrgb[x].rgbReserved = 0;
	   }
	 */
	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.SizeImage = imageSize;
	//bmpInfoHeader.biCompression = 0;
	//bmpInfoHeader.biClrUsed = 0;
	//bmpInfoHeader.biClrImportant = 0;
	//bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
	bmpHeader.bfSize = bmpInfoHeader.SizeImage;
	/* BITMAPFILEHEADER 구조체의 데이터 */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	/* BITMAPINFOHEADER 구조체의 데이터 */
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//fwrite(palrgb, sizeof(RGBQUAD), 256, fp);
	//fwrite(inimg, sizeof(ubyte), imageSize, fp);
	fwrite(outimg, sizeof(ubyte), imageSize, fp);
	fclose(fp);
	free(inimg);
	free(outimg);

	printf("Success blur\n");
	return 0;
}
