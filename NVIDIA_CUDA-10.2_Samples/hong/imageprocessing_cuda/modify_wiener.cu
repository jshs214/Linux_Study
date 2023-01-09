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

//Create a noise standard deviation
__global__ void convertToBlur(ubyte *inimg, ubyte *out, int width, int height, int elemSize) {
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int z = threadIdx.z + blockIdx.z * blockDim.z;

	int rowSize = width*elemSize;
	int offset = (x*elemSize+(y*rowSize));


	float offset_Average, offset_SD;

	float sum = 0.0, sum_SD = 0.0, mask_SD = 0.0;
	int wiener = 0.0;

	// inSide
	if ( (x > 0 && x < width-1) && (y >0 && y < height-1) ) {
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				sum += inimg[(x+i)*elemSize +((y+j)*rowSize) +z];
			}
		}
		offset_Average = sum/9;		//In mask average
		sum = 0.0;
		//find a standard deviation value
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				offset_SD =  pow( ( inimg[(x+i)*elemSize + ((y+j)*rowSize)+z] - offset_Average), 2)  / 9 ;
				sum_SD += offset_SD;
			}
		}
		mask_SD = sqrt(sum_SD);
		float o = pow(mask_SD, 2);
		//Input noise_deviation
		float v = pow(noise_deviation, 2);
		//USE wiener filter
		wiener = offset_Average + (1 + v/o) * inimg(rgb[offset+z] - offset_Average);
		out[offset +z] = LIMIT_UBYTE(wiener);
	}
	// OutSide
	else{
		int arr[9] = {0, };
		//LeftSide
		else if(x ==0){
			//LeftTopVertex
			if(y==0){
				arr[0] = arr[1] = arr[3] = arr[4] = inimg[(x*elemSize)+(y*rowSize)+z];
				arr[2] = arr[5] = inimg[(x*elemSize)+elemSize +(y*rowSize)+z];
				arr[6] = arr[7] = inimg[(x*elemSize)+((y+1)*rowSize)+z];
				arr[8] = inimg[(x*elemSize)+elemSize+((y+1)*rowSize)+z];	
			}
			//LeftDownVertex
			else if(y==height-1){
				arr[0] = arr[1] = inimg[(x*elemSize)+((y-1)*rowSize)+z];
				arr[2] = inimg[(x*elemSize)+elemSize+((y-1)*rowSize)+z];
				arr[3] = arr[6] = arr[7] = arr[4] = inimg[(x*elemSize)+(y*rowSize)+z];
				arr[8] = arr[5] = inimg[(x*elemSize)+elemSize+(y*rowSize)+z];
			}

			//LeftSide
			else{
				arr[0] = arr[1] = inimg[(x*elemSize)+((y-1)*rowSize)+z];
				arr[2] = inimg[(x*elemSize)+elemSize+((y-1)*rowSize)+z];
				arr[3] = arr[4] = inimg[(x*elemSize)+(y*rowSize)+z];
				arr[5] = inimg[(x*elemSize)+elemSize+(y*rowSize)+z];
				arr[6] = arr[7] = inimg[(x*elemSize)+((y+1)*rowSize)+z];
				arr[8] = inimg[(x*elemSize)+elemSize+((y+1)*rowSize)+z];
			}

		}
		//RightSide
		else if(x==width-1){
			//RightTopVertex
			if(y==0){
				arr[0] = arr[3] = inimg[(x*elemSize)-elemSize+(y*rowSize)+z];
				arr[1] = arr[2] = arr[5] = arr[4] = inimg[offset+z];
				arr[6] = inimg[(x*elemSize)-elemSize+((y-1)*rowSize)+z];
				arr[7] = arr[8] = inimg[(x*elemSize)+((y+1)*rowSize)+z];
			}
			//RightDownVertex
			else if(y==height-1){
				arr[0] = inimg[(x*elemSize)-elemSize+((y-1)*rowSize)+z];
				arr[1] = arr[2] = inimg[(x*elemSize)-elemSize+((y-1)*rowSize)+z];
				arr[3] = arr[6] = inimg[(x*elemSize)-elemSize+(y*rowSize)+z];
				arr[4] = arr[5] = arr[7] = arr[8] = inimg[offset+z];
			}
			//RightSide
			else{
				arr[0] = inimg[(x*elemSize)-elemSize+((y-1)*rowSize)+z];
				arr[1] = arr[2] = inimg[(x*elemSize)+((y-1)*rowSize)+z];
				arr[3] = inimg[(x*elemSize)-elemSize+(y*rowSize)+z];
				arr[4] = arr[5] = inimg[(x*elemSize)+(y*rowSize)+z];
				arr[6] = inimg[(x*elemSize)-elemSize+((y+1)*rowSize)+z];
				arr[7] = arr[8] = inimg[(x*elemSize)+((y+1)*rowSize)+z];
			}
		}
		//TopSide
		else if( y==0){
			if(x!=0 && x!=width-1){
				arr[0] = arr[3] = inimg[(x*elemSize)-elemSize+(y*rowSize)+z];
				arr[1] = arr[4] = inimg[offset+z];
				arr[2] = arr[5] = inimg[(x*elemSize)+elemSize+(y*rowSize)+z];
				arr[6] = inimg[(x*elemSize)-elemSize+((y+1)*rowSize)+z];
				arr[7] = inimg[(x*elemSize)+((y+1)*rowSize)+z];
				arr[8] = inimg[(x*elemSize)+elemSize+((y+1)*rowSize)+z];

			}
		}
		//BottomSide
		else if( y==height-1){
			if(x!=0 && x!=width-1){
				arr[0] = inimg[(x*elemSize)-elemSize+((y-1)*rowSize)+z];
				arr[1] = inimg[(x*elemSize)+((y-1)*rowSize)+z];
				arr[2] = inimg[(x*elemSize)+elemSize+((y-1)*rowSize)+z];
				arr[3] = arr[6] = inimg[(x*elemSize)-elemSize+(y*rowSize)+z];
				arr[4] = arr[7] = inimg[offset+z];
				arr[5] = arr[8] = inimg[(x*elemSize)-elemSize+(y*rowSize)+z];
			}
		}	

		int cnt=0;
		//find sum of AdjacentValue
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				sum += arr[cnt++];
			}
		}

		offset_Average = sum/9;		//In mask average
		cnt = 0;

		//find a standard deviation value
		for(int i = -1; i < 2; i++) {
			for(int j = -1; j< 2; j ++){
				offset_SD =  pow( ( arr[cnt++] - offset_Average), 2)  / 9 ;
				sum_SD += offset_SD;
			}
		}
		mask_SD = sqrt(sum_SD);
		float o = pow(mask_SD, 2);
		//Input noise_deviation
		float v = pow(noise_deviation, 2);
		//USE wiener filter
		wiener = offset_Average + (1 + v/o) * abs(inimg[offset+z] - offset_Average);
		out[offset +z] = LIMIT_UBYTE(wiener);
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

	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.SizeImage = imageSize;

	bmpHeader.bfSize = bmpInfoHeader.SizeImage;
	/* BITMAPFILEHEADER 구조체의 데이터 */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	/* BITMAPINFOHEADER 구조체의 데이터 */
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	fwrite(outimg, sizeof(ubyte), imageSize, fp);
	fclose(fp);
	free(inimg);
	free(outimg);

	printf("Success blur\n");
	return 0;
}
