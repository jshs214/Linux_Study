#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX �곸닔瑜� �꾪빐�� �ъ슜�쒕떎. */
#include <unistd.h>

#include "bmpHeader.h"

/* �대�吏� �곗씠�곗쓽 寃쎄퀎 寃��щ� �꾪븳 留ㅽ겕濡� */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

void insertion(int a[], int n)
{
	int i, j;
	for(i = 1; i< n; i++){
		int tmp = a[i];
		for(j = i; j>0 && a[j-1] > tmp; j--)
			a[j] = a[j-1];
		a[j] = tmp;
	}
}

int main(int argc, char** argv) 
{
	FILE* fp; 
	BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
	BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
	RGBQUAD *palrgb;
	ubyte *inimg, *padimg, *outimg;
	int x, y, z, imageSize;

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
	fread(inimg, sizeof(ubyte), imageSize, fp); 
	fclose(fp);

	int padSize = (bmpInfoHeader.biWidth + 2) * elemSize;
	int addSize = (padSize + bmpInfoHeader.biHeight)*2;
	padimg = (ubyte*)malloc(sizeof(ubyte)*(imageSize + addSize));

	/* make padding image */
	memset(padimg, 0, (sizeof(ubyte)*imageSize + addSize));
	//memset(outimg, 0, sizeof(ubyte)*imageSize);
	for(y = 0; y < bmpInfoHeader.biHeight; y++) {
		for(x = 0; x < bmpInfoHeader.biWidth * elemSize; x+=elemSize) {
			for(z = 0; z < elemSize; z++) {
				//outimg[(x+elemSize)+(y+1)*size+z]=inimg[x+y*size+z];
				padimg[(x+elemSize)+(y+1)*padSize+z]=inimg[x+y*size+z];
			}
		}
	}

	for(y = 0; y < bmpInfoHeader.biHeight; y++) { 
		for(z = 0; z < elemSize; z++) {
			padimg[0+(y+1)*padSize+z]=inimg[0+y*size+z];
			padimg[padSize-elemSize+(y+1)*padSize+z]=inimg[size-elemSize+y*size+z];
		}
	}

	for(x = 0; x < bmpInfoHeader.biWidth*elemSize; x++) { 
		padimg[elemSize+x]=inimg[x];
		padimg[elemSize+x+(bmpInfoHeader.biHeight+1)*padSize]=inimg[x+(bmpInfoHeader.biHeight-1)*size];
	}

	for(z = 0; z < elemSize; z++) {
		padimg[z]=inimg[z];
		padimg[padSize-elemSize+z]=inimg[size-elemSize+z];
		padimg[(bmpInfoHeader.biHeight+1)*padSize+z]=inimg[(bmpInfoHeader.biHeight-1)*size+z];
		padimg[(bmpInfoHeader.biHeight+1)*padSize+padSize-elemSize+z]=inimg[(bmpInfoHeader.biHeight-1)*size+size-elemSize+z];
	}

	// define the kernel
	float kernel[3][3] = { {0, -1, 0},
		{-1,  5, -1},
		{0, -1, 0} };
	int arr[9] = {0,};
	int count = 0;

	memset(outimg, 0, sizeof(ubyte)*imageSize);
	for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) { 
		for(x = elemSize; x < padSize; x+=elemSize) {
			for(z = 0; z < elemSize; z++) {
				float sum = 0.0;	

				count =0;

				for(int i = -1; i < 2; i++) {
					for(int j = -1; j < 2; j++) {
						arr[count++] = padimg[(x+i*elemSize)+(y+j)*padSize+z];
					}
				}
				insertion(arr, 9);

				outimg[(x-elemSize)+(y-1)*size+z] = arr[4];
			}
		}
	}

	/***** write bmp *****/ 
	if((fp=fopen(argv[2], "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...�쯰"); 
		return -1;
	}

	/* BITMAPFILEHEADER 援ъ“泥댁쓽 �곗씠�� */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	/* BITMAPINFOHEADER 援ъ“泥댁쓽 �곗씠�� */
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	//fwrite(inimg, sizeof(ubyte), imageSize, fp); 
	//fwrite(padimg, sizeof(ubyte), imageSize, fp);
	fwrite(outimg, sizeof(ubyte), imageSize, fp);

	fclose(fp); 

	free(inimg); 
	free(outimg);
	free(padimg);

	return 0;
}
