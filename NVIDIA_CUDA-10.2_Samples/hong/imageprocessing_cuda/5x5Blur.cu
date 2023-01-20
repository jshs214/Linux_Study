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

__global__ void convertToBlur(ubyte *inimg, ubyte *out, int width, int height, int elemSize) {
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int z = threadIdx.z + blockIdx.z * blockDim.z;

	int size = width*elemSize;
	int offset = (x*elemSize+(y*size));

	unsigned char arr[25]={0,};
	float blur[5][5] = { {1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
		{1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
		{1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
		{1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0},
		{1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0} };

	// inSide
	float sum = 0.0;
	if ( (x > 1 && x < width-2) && (y >1 && y < height-2) ) {
		for(int i = -2; i < 3; i++) {
			for(int j = -2; j < 3; j++) {
				sum += blur[i+2][j+2]*inimg[(x+i)*elemSize+((y+j)*size)+z];
			}
		}
		out[offset +z] = LIMIT_UBYTE(sum);
	}

	//LeftVertex
	else if(x ==0){
		//LeftTopVertex
		if(y==0){
			arr[0] = arr[1]= arr[2] = arr[5] = arr[6] = arr[7] = arr[10] = arr[11] = arr[12] = inimg[offset+z];
			arr[3] = arr[8] = arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[4] = arr[9] = arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[20] = arr[21] = arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+2)*elemSize+((y+2)*size)+z];
		}
		else if(y == 1){
			arr[0] = arr[1] = arr[2] = arr[5] = arr[6] = arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[10] = arr[11] = arr[12] = inimg[offset+z];
			arr[3] = arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[4] = arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[20] = arr[21] = arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+2)*elemSize+((y+2)*size)+z];
		}
		else if(y==height-2){
			arr[0] = arr[1] = arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = arr[6] = arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[11] = arr[12] = inimg[offset+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = arr[17] = arr[20] = arr[21] = arr[22] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = arr[23] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = arr[24] = inimg[(x+2)*elemSize+((y+1)*size)+z];
		}
		//LeftBottomVertex
		else if(y == height-1){
			arr[0] = arr[1] = arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = arr[6] = arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[11]= arr[15] = arr[16] = arr[17] = arr[20] = arr[21] = arr[22] = arr[12] = inimg[offset+z];
			arr[13] = arr[18] = arr[23] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[14] = arr[19] = arr[24] = inimg[(x+2)*elemSize+((y+1)*size)+z];
		}
		//LeftSide
		else{
			arr[0] = arr[1] = arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = arr[6] = arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[11] = arr[12] = inimg[offset+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[20] = arr[21] = arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+2)*elemSize+((y+2)*size)+z];
		}

	}
	//LeftSide
	else if(x==1){
		//LeftTopVertex
		if(y==0){
			arr[0] = arr[1] = arr[5] = arr[6] = arr[10] = arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[2] = arr[7] = arr[12] = inimg[offset+z];
			arr[3] = arr[8] = arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[4] = arr[9] = arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[20] = arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+2)*elemSize+((y+2)*size)+z];
		}
		else if(y==1){
			arr[0] = arr[1] = arr[5] = arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[2] = arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[3] = arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[4] = arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[20] = arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+1)*elemSize+((y+2)*size)+z];
		}
		//LeftBottomVertex
		else if(y == height -1){
			arr[0] = arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[11] = arr[15] = arr[16] = arr[20] = arr[21] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = arr[17] = arr[22] = inimg[offset+z];
			arr[13] = arr[18] = arr[23] = inimg[(x+1)*elemSize+(y*size)  +z];
			arr[14] = arr[19] = arr[24] = inimg[(x+2)*elemSize+(y*size)+z];
		}
		else if(y == height -2){
			arr[0] = arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = arr[20] = arr[21] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = arr[22] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = arr[23] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = arr[24] = inimg[(x+2)*elemSize+((y+1)*size)+z];
		}
		//LeftSide
		else{
			arr[0] = arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[20] = arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+2)*elemSize+((y+2)*size)+z];
		}

	}
	//RightSide
	else if(x==width-2){
		//RightTopVertex
		if(y==0){
			arr[0] = arr[5] = arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[1] = arr[6] = arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[2] = arr[7] = arr[12] = inimg[offset+z];
			arr[3] = arr[8] = arr[4] = arr[9] = arr[14] = arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[15] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = arr[19] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = arr[24] = inimg[(x+1)*elemSize+((y+2)*size)+z];
		}
		else if(y==1){
			arr[0] = arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[1] = arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[2] = arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[3] = arr[4] = arr[8] = arr[9] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = arr[14] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[15] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = arr[19] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = arr[24] = inimg[(x+1)*elemSize+((y+2)*size)+z];
		}
		//RightBottomVertex
		else if(y==height-2){
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = arr[4] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = arr[9] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = arr[14] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[15] = arr[20] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = arr[21] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = arr[22] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = arr[23] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = arr[24] = inimg[(x+2)*elemSize+((y+1)*size)+z];
		}
		else if(y == height-1){
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = arr[4] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = arr[9] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[10] = arr[15] = arr[20] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = arr[16] = arr[21] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = arr[17] = arr[22] = inimg[offset+z];
			arr[13] = arr[14] = arr[18] = arr[19] = arr[23] = arr[24] = inimg[(x+1)*elemSize+(y*size)+z];
		}
		//RightSide
		else{
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = arr[4] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = arr[9] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = arr[19] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = arr[24] = inimg[(x+1)*elemSize+((y+2)*size)+z];
		}
	}
	//RightSide
	else if(x==width-1){
		//RightTopVertex
		if(y==0){
			arr[0] = arr[5] = arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[1] = arr[6] = arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[2] = arr[7] = arr[3] = arr[8] = arr[13] = arr[4] = arr[9] = arr[14] = arr[12] = inimg[offset+z];
			arr[15] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = arr[18] = arr[19] = inimg[x*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = arr[23] = arr[24] = inimg[x*elemSize+((y+2)*size)+z];
		}
		else if(y==1){
			arr[0] = arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[1] = arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[2] = arr[7] = arr[3] = arr[4] = arr[8] = arr[9] = inimg[x*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = arr[13] = arr[14] = inimg[offset+z];
			arr[15] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = arr[18] = arr[19] = inimg[x*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = arr[23] = arr[24] = inimg[x*elemSize+((y+2)*size)+z];
		}
		//RightBottomVertex
		else if(y==height-1){
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = arr[3] = arr[4] = inimg[x*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = arr[8] = arr[9] = inimg[x*elemSize+((y-1)*size)+z];
			arr[10] = arr[15] = arr[20] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = arr[16] = arr[21] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = arr[13] = arr[14] = arr[17] = arr[18] = arr[19] = arr[22] = arr[23] = arr[24] = inimg[offset+z];
		}
		else if(y==height-2){
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = arr[3] = arr[4] = inimg[x*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = arr[8] = arr[9] = inimg[x*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = arr[13] = arr[14] = inimg[offset+z];
			arr[15] = arr[20] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = arr[21] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = arr[18] = arr[19] = arr[22] = arr[23] = arr[24] = inimg[x*elemSize+((y+1)*size)+z];
		}
		//RightSide
		else{
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = arr[3] = arr[4] = inimg[x*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = arr[8] = arr[9] = inimg[x*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = arr[13] = arr[14] = inimg[offset+z];
			arr[15] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = arr[18] = arr[19] = inimg[x*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = arr[23] = arr[24] = inimg[x*elemSize+((y+2)*size)+z];

		}
	}

	//TopSide
	else if( y==0){
		if(x>1 && x <width-2){
			arr[0] = arr[5] = arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[1] = arr[6] = arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[2] = arr[7] = arr[12] = inimg[offset+z];
			arr[3] = arr[8] = arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[4] = arr[9] = arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+2)*elemSize+((y+2)*size)+z];
		}
	}
	else if(y==1){
		if(x>1 && x <width-2){
			arr[0] = arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[1] = arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[2] = arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[3] = arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[4] = arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = inimg[(x+2)*elemSize+((y+1)*size)+z];
			arr[20] = inimg[(x-2)*elemSize+((y+2)*size)+z];
			arr[21] = inimg[(x-1)*elemSize+((y+2)*size)+z];
			arr[22] = inimg[x*elemSize+((y+2)*size)+z];
			arr[23] = inimg[(x+1)*elemSize+((y+2)*size)+z];
			arr[24] = inimg[(x+2)*elemSize+((y+2)*size)+z];
		}
	}
	//BottomSide
	else if(y==height-2){
		if(x>1 && x <width-2){
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = inimg[offset+z];
			arr[13] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = inimg[(x+2)*elemSize+(y*size)+z];
			arr[15] = arr[20] = inimg[(x-2)*elemSize+((y+1)*size)+z];
			arr[16] = arr[21] = inimg[(x-1)*elemSize+((y+1)*size)+z];
			arr[17] = arr[22] = inimg[x*elemSize+((y+1)*size)+z];
			arr[18] = arr[23] = inimg[(x+1)*elemSize+((y+1)*size)+z];
			arr[19] = arr[24] = inimg[(x+2)*elemSize+((y+1)*size)+z];
		}	
	}
	else if( y==height-1){
		if(x>1 && x <width-2){
			arr[0] = inimg[(x-2)*elemSize+((y-2)*size)+z];
			arr[1] = inimg[(x-1)*elemSize+((y-2)*size)+z];
			arr[2] = inimg[x*elemSize+((y-2)*size)+z];
			arr[3] = inimg[(x+1)*elemSize+((y-2)*size)+z];
			arr[4] = inimg[(x+2)*elemSize+((y-2)*size)+z];
			arr[5] = inimg[(x-2)*elemSize+((y-1)*size)+z];
			arr[6] = inimg[(x-1)*elemSize+((y-1)*size)+z];
			arr[7] = inimg[x*elemSize+((y-1)*size)+z];
			arr[8] = inimg[(x+1)*elemSize+((y-1)*size)+z];
			arr[9] = inimg[(x+2)*elemSize+((y-1)*size)+z];
			arr[10] = arr[15] = arr[20] = inimg[(x-2)*elemSize+(y*size)+z];
			arr[11] = arr[16] = arr[21] = inimg[(x-1)*elemSize+(y*size)+z];
			arr[12] = arr[17] = arr[22] = inimg[offset+z];
			arr[13] = arr[18] = arr[23] = inimg[(x+1)*elemSize+(y*size)+z];
			arr[14] = arr[19] = arr[24] = inimg[(x+2)*elemSize+(y*size)+z];
		}
	}

	int cnt=0;
	for(int i = -2; i < 3; i++) {
		for(int j = -2; j < 3; j++) {
			sum += blur[i+2][j+2]*arr[cnt++];
		}
	}
	out[offset+z] = LIMIT_UBYTE(sum);
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
