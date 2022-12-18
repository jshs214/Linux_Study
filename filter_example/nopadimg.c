#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX �곸닔瑜� �꾪빐�� �ъ슜�쒕떎. */
#include <unistd.h>
#include <math.h>

#include "bmpHeader.h"

/* �대�吏� �곗씠�곗쓽 寃쎄퀎 寃��щ� �꾪븳 留ㅽ겕濡� */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
	FILE* fp; 
	BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
	BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
	RGBQUAD *palrgb;
	ubyte *inimg, *outimg, *midimg;
	int x, y, z, imageSize, index;
	float r, g, b, gray;
	double hedge, vedge;

	if(argc != 3) {
		fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
		return -1;
	}

	/***** read bmp *****/ 
	if((fp=fopen(argv[1], "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...�쯰"); 
		return -1;
	}

	/* BITMAPFILEHEADER 援ъ“泥댁쓽 �곗씠�� */
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	/* BITMAPINFOHEADER 援ъ“泥댁쓽 �곗씠�� */
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	/* �몃（ 而щ윭瑜� 吏��먰븯硫� 蹂��섑븷 �� �녿떎. */
	if(bmpInfoHeader.biBitCount != 24) {
		perror("This image file doesn't supports 24bit color\n");
		fclose(fp);
		return -1;
	}

	int elemSize = bmpInfoHeader.biBitCount/8;
	int size = bmpInfoHeader.biWidth*elemSize;
	imageSize = size * bmpInfoHeader.biHeight; 

	/* �대�吏��� �댁긽��(�볦씠 횞 源딆씠) */
	printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
	printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* �쎌��� 鍮꾪듃 ��(�됱긽) */
	printf("Image Size : %d\n", imageSize);

	inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
	outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
	midimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
		
	fread(inimg, sizeof(ubyte), imageSize, fp); 
	fclose(fp);



	//midimg = inimg to grayScale
	for(int i = 0; i < bmpInfoHeader.biHeight; i++) {
		index = (bmpInfoHeader.biHeight-i-1) * size; 
		for(int j = 0; j < bmpInfoHeader.biWidth; j++) { 
			r = (float)inimg[index+3*j+2];
			g = (float)inimg[index+3*j+1];
			b = (float)inimg[index+3*j+0];
			gray=(r*0.3F)+(g*0.59F)+(b*0.11F);

			midimg[index+3*j] = midimg[index+3*j+1] = midimg[index+3*j+2] = gray;
		};
	};


	// define arr
	float xfilter[3][3] = {{-1, 0, 1 },
		{-2, 0, 2 },
		{-1, 0, 1 } };
	float yfilter[3][3] = {{1, 2, 1 },
		{0, 0, 0 },
		{-1, -2, -1 } };
		
	int arr[9]= {0,};

	memset(outimg, 0, sizeof(ubyte)*imageSize);
	for(y = 0; y < bmpInfoHeader.biHeight ; y++) { 
		for(x = 0; x < size; x+=elemSize) {
			for(z = 0; z < elemSize; z++) {
				float sum = 0.0;
				hedge = 0.0;
				vedge = 0.0;
				if(y==0){
					outimg[x+(size*y+z)] = inimg[x+(size*y+z)]; 
				}
				else if(y==bmpInfoHeader.biHeight -1)
				{
					outimg[x+(size*y+z)] = inimg[x+(size*y+z)]; 
				
				}
				else if(x==0){
					outimg[x+(size*y+z)] = inimg[x+(size*y+z)]; 
				}	
				else if(x==size - elemSize){
					outimg[x+(size*y+z)] = inimg[x+(size*y+z)]; 
				}	
				else if(x !=0 && y != 0 && y != bmpInfoHeader.biHeight-1 && x != size - elemSize){
					for(int i = -1; i < 2; i++) {
						for(int j = -1; j < 2; j++) {
							vedge += xfilter[i+1][j+1]*midimg[(x+i*elemSize)+(y+j)*size+z];
							hedge += yfilter[i+1][j+1]*midimg[(x+i*elemSize)+(y+j)*size+z];
						}
					}
					sum=sqrt(hedge*hedge+vedge*vedge);
					outimg[(x-elemSize)+(y-1)*size+z] = LIMIT_UBYTE(sum);
				
				}
				//for(int i = 0; i < 9 ; i ++){
				//	sum += arr[i];
				//}
				//outimg[(x-elemSize)+(y-1)*size+z] = LIMIT_UBYTE(sum);
			}
		}
	}  
	printf("\n2\n");

	/***** write bmp *****/ 
	if((fp=fopen(argv[2], "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...�쯰"); 
		return -1;
	}

	/* BITMAPFILEHEADER 援ъ“泥댁쓽 �곗씠�� */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	/* BITMAPINFOHEADER 援ъ“泥댁쓽 �곗씠�� */
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	fwrite(outimg, sizeof(ubyte), imageSize, fp);

	fclose(fp); 

	free(inimg); 
	free(outimg);
	free(midimg);

	return 0;
}
