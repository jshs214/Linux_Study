#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <io.h>
//#include <conio.h>

using namespace std;

typedef unsigned short ushort;

void dirDarkfile();		// 지정된 경로의 Dark 디렉토리 내 모든 raw파일을 darkfn 벡터에 pushback 하는 함수 
void dirGainfile();		// 지정된 경로의 Gain 디렉토리 내 모든 raw파일을 gainfn 벡터에 pushback 하는 함수 
void darkMap();			// darkfn의 평균값으로 darkMap.raw 생성 함수
void darkProcessing();	// 원본이미지 - darkMap 함수
void gainMap();			// gainfn의 평균값으로 darkMap.raw 생성 함수

void gainProcessing();	// 원본이미지 - GainMap 함수

vector<string> darkfn;	// Dark 디렉토리 내 raw파일들을 저장하고 있는 벡터
vector<string> gainfn;	// Gain 디렉토리 내 raw파일들을 저장하고 있는 벡터

void callibration();	// 연산 함수

int main()
{
	dirDarkfile();
	dirGainfile();
	darkMap();
	darkProcessing();
	gainMap();

	gainProcessing();

	callibration();

	printf("Success processing !!\n");
	return 0;
}

void darkMap() {
	FILE* infp, * outfp;
	char savefile[] = "./output(1628x1628)/DarkMap(1628).raw";

	ushort* inimg, * outimg;
	float* f_averageimg;

	int width = 1628;
	int height = 1628;
	int imageSize = width * height;

	inimg = (ushort*)malloc(sizeof(ushort) * imageSize);
	outimg = (ushort*)malloc(sizeof(ushort) * imageSize);
	f_averageimg = (float*)malloc(sizeof(float) * imageSize);

	memset(inimg, 0, sizeof(ushort) * imageSize);
	memset(outimg, 0, sizeof(ushort) * imageSize);
	memset(f_averageimg, 0, sizeof(float) * imageSize);

	vector<string>::iterator iter;
	iter = darkfn.begin();
	for (iter = darkfn.begin(); iter != darkfn.end(); iter++) {
		memset(inimg, 0, sizeof(ushort) * imageSize);
		char path[100] = "./S1_1628x1628/Dark/";
		string file = path + *iter;

		//cout << file << endl;	// 파일 fopen 확인

		if ((infp = fopen(file.c_str(), "rb")) == NULL) {
			printf("No such file or folder\n");
			return;
		}

		fread(inimg, sizeof(ushort) * imageSize, 1, infp);

		for (int i = 0; i < imageSize; i++) {
			*(f_averageimg + i) += inimg[i];
		}

		fclose(infp);
	}

	for (int i = 0; i < imageSize; i++) {
		* (outimg + i) = *(f_averageimg + i) / 101;
	}

	if ((outfp = fopen(savefile, "wb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
		return;
	}
	fwrite(outimg, sizeof(ushort) * imageSize, 1, outfp);

	free(inimg);
	free(outimg);
	free(f_averageimg);
	fclose(outfp);
}

void darkProcessing() {
	FILE* darkMapFp, * MTF_VFp, *outfp;
	int width = 1628;
	int height = 1628;
	int imageSize = width * height;
	char savefile[] = "./output(1628x1628)/DarkProcessing(1628).raw";

	ushort* darkMapImg, * MTF_VImg, *outimg;

	darkMapImg = (ushort*)malloc(sizeof(ushort) * imageSize);
	MTF_VImg = (ushort*)malloc(sizeof(ushort) * imageSize);
	outimg = (ushort*)malloc(sizeof(ushort) * imageSize);
	
	memset(darkMapImg, 0, sizeof(ushort) * imageSize);
	memset(MTF_VImg, 0, sizeof(ushort) * imageSize);
	memset(outimg, 0, sizeof(ushort) * imageSize);

	if ((darkMapFp = fopen("./output(1628x1628)/DarkMap(1628).raw", "rb")) == NULL) {
		printf("No such file or folder\n");
		return;
	}
	if ((MTF_VFp = fopen("./S1_1628x1628/MTF_V.raw", "rb")) == NULL) {
		printf("No such file or folder\n");
		return;
	}

	fread(darkMapImg, sizeof(ushort) * imageSize, 1, darkMapFp);
	fread(MTF_VImg, sizeof(ushort) * imageSize, 1, MTF_VFp);
	fclose(darkMapFp);
	fclose(MTF_VFp);

	for (int i = 0; i < imageSize; i++) {
		if(*(MTF_VImg + i) - *(darkMapImg + i) >=0 )
			*(outimg + i) = *(MTF_VImg + i) - *(darkMapImg + i);
		else
			*(outimg + i) = *(darkMapImg + i) - *(MTF_VImg + i);
	}

	if ((outfp = fopen(savefile, "wb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
		return;
	}
	fwrite(outimg, sizeof(ushort) * imageSize, 1, outfp);

	fclose(outfp);

	free(darkMapImg);
	free(MTF_VImg);
	free(outimg);
}

void gainMap() {
	FILE* infp, * outfp;
	char savefile[] = "./output(1628x1628)/GainMap(1628).raw";

	ushort* inimg, * outimg;
	float* f_averageimg;

	int width = 1628;
	int height = 1628;
	int imageSize = width * height;

	inimg = (ushort*)malloc(sizeof(ushort) * imageSize);
	outimg = (ushort*)malloc(sizeof(ushort) * imageSize);
	f_averageimg = (float*)malloc(sizeof(float) * imageSize);

	memset(inimg, 0, sizeof(ushort) * imageSize);
	memset(outimg, 0, sizeof(ushort) * imageSize);
	memset(f_averageimg, 0, sizeof(float) * imageSize);

	vector<string>::iterator iter;
	iter = gainfn.begin();
	for (iter = gainfn.begin(); iter != gainfn.end(); iter++) {
		memset(inimg, 0, sizeof(ushort) * imageSize);
		char path[100] = "./S1_1628x1628/Gain/";
		string file = path + *iter;

		//cout << file << endl;	// 파일 fopen 확인
		if ((infp = fopen(file.c_str(), "rb")) == NULL) {
			printf("No such file or folder\n");
			return;
		}

		fread(inimg, sizeof(ushort) * imageSize, 1, infp);

		for (int i = 0; i < imageSize; i++) {
			*(f_averageimg + i) += inimg[i];
		}

		fclose(infp);
	}

	for (int i = 0; i < imageSize; i++) {
		*(outimg + i) = *(f_averageimg + i) / 101;
	}

	if ((outfp = fopen(savefile, "wb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
		return;
	}
	fwrite(outimg, sizeof(ushort) * imageSize, 1, outfp);

	free(inimg);
	free(outimg);
	free(f_averageimg);
	fclose(outfp);
}

void gainProcessing() {
	FILE* gainMapFp, * MTF_VFp, * outfp;
	int width = 1628;
	int height = 1628;
	int imageSize = width * height;
	char savefile[] = "./output(1628x1628)/GainProcessing(1628).raw";

	ushort* gainMapImg, * MTF_VImg, * outimg;

	gainMapImg = (ushort*)malloc(sizeof(ushort) * imageSize);
	MTF_VImg = (ushort*)malloc(sizeof(ushort) * imageSize);
	outimg = (ushort*)malloc(sizeof(ushort) * imageSize);

	memset(gainMapImg, 0, sizeof(ushort) * imageSize);
	memset(MTF_VImg, 0, sizeof(ushort) * imageSize);
	memset(outimg, 0, sizeof(ushort) * imageSize);

	if ((gainMapFp = fopen("./output(1628x1628)/GainMap(1628).raw", "rb")) == NULL) {
		printf("No such file or folder\n");
		return;
	}
	if ((MTF_VFp = fopen("./S1_1628x1628/MTF_V.raw", "rb")) == NULL) {
		printf("No such file or folder\n");
		return;
	}

	fread(gainMapImg, sizeof(ushort) * imageSize, 1, gainMapFp);
	fread(MTF_VImg, sizeof(ushort) * imageSize, 1, MTF_VFp);
	fclose(gainMapFp);
	fclose(MTF_VFp);

	for (int i = 0; i < imageSize; i++) {
		if (*(MTF_VImg + i) - *(gainMapImg + i) >= 0)
			*(outimg + i) = *(MTF_VImg + i) - *(gainMapImg + i);
		else
			*(outimg + i) = *(gainMapImg + i) - *(MTF_VImg + i);
	}

	if ((outfp = fopen(savefile, "wb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
		return;
	}
	fwrite(outimg, sizeof(ushort) * imageSize, 1, outfp);

	fclose(outfp);

	free(gainMapImg);
	free(MTF_VImg);
	free(outimg);
}

void callibration() {
	FILE* GainMapFp, * darkMapFp, * MTF_VFp, *outfp;
	char savefile[] = "./output(1628x1628)/callibration(1628).raw";

	int width = 1628, height = 1628, widthcnt = 0;
	int imageSize = width * height;
	int subImageSize = (width - 200) * (height - 200);
	double subGainSum = 0, subGainAvg = 0;

	ushort * darkMapImg, * GainMapImg,* MTF_VImg, * subGainImg, *outimg;

	darkMapImg = (ushort*)malloc(sizeof(ushort) * imageSize);
	GainMapImg = (ushort*)malloc(sizeof(ushort) * imageSize);
	MTF_VImg = (ushort*)malloc(sizeof(ushort) * imageSize);
	outimg = (ushort*)malloc(sizeof(ushort) * imageSize);
	subGainImg = (ushort*)malloc(sizeof(ushort) * subImageSize);

	memset(darkMapImg, 0, sizeof(ushort) * imageSize);
	memset(GainMapImg, 0, sizeof(ushort) * imageSize);
	memset(MTF_VImg, 0, sizeof(ushort) * imageSize);
	memset(outimg, 0, sizeof(ushort) * imageSize);
	memset(subGainImg, 0, sizeof(ushort) * subImageSize);

	if ((darkMapFp = fopen("./output(1628x1628)/DarkMap(1628).raw", "rb")) == NULL) {
		printf("No such file or folder\n");
		return;
	}
	if ((GainMapFp = fopen("./output(1628x1628)/GainMap(1628).raw", "rb")) == NULL) {
		printf("No such file or folder\n");
		return;
	}
	if ((MTF_VFp = fopen("./S1_1628x1628/MTF_V.raw", "rb")) == NULL) {
		printf("No such file or folder\n");
		return;
	}

	fread(darkMapImg, sizeof(ushort) * imageSize, 1, darkMapFp);
	fread(GainMapImg, sizeof(ushort) * imageSize, 1, GainMapFp);
	fread(MTF_VImg, sizeof(ushort) * imageSize, 1, MTF_VFp);

	fclose(darkMapFp);
	fclose(GainMapFp);
	fclose(MTF_VFp);

	int j = 0;
	/* subGain image load*/
	for (int i = 0; i < imageSize; i++) {
		widthcnt++;
		if (widthcnt == width) widthcnt = 0;

		if (width * 100 > i || width * (height - 100) < i) continue;
		if (widthcnt <= 100 || widthcnt > 1528) continue;

		subGainImg[j] = GainMapImg[i];
		subGainSum += subGainImg[j];
		j++;
	}

	subGainAvg = subGainSum / subImageSize ;
	printf("Sum : %f \n", subGainSum);
	printf("mean : %f\n", subGainAvg);

	/* 연산 for 문*/
	for (int i = 0; i < imageSize; i++) {
		*(outimg + i) = 
		 (ushort) ( abs( *(MTF_VImg + i) - ( *(darkMapImg + i) ) ) / (float)( *(GainMapImg + i) )  * subGainAvg );
	}


	if ((outfp = fopen(savefile, "wb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
		return;
	}

	fwrite(outimg, sizeof(ushort) * imageSize, 1, outfp);

	fclose(outfp);

	free(GainMapImg);
	free(darkMapImg);
	free(MTF_VImg);
	free(subGainImg);
	free(outimg);
}

// dark 디렉토리 내 파일 찾기 함수
void dirDarkfile()
{
	_finddata_t fd;
	long handle;
	int out = 1;
	handle = _findfirst("./S1_1628x1628/Dark/*.raw", &fd);  //현재 폴더 내 모든 파일을 찾는다.

	if (handle == -1)
	{
		printf("There were no files.\n");
	}

	while (out != -1)
	{
		darkfn.push_back(fd.name);
		out = _findnext(handle, &fd);
	}
	_findclose(handle);
}

// Gain 디렉토리 내 파일 찾기 함수
void dirGainfile()
{
	_finddata_t fd;
	long handle;
	int out = 1;
	handle = _findfirst("./S1_1628x1628/Gain/*.raw", &fd);  //현재 폴더 내 모든 파일을 찾는다.

	if (handle == -1)
	{
		printf("There were no files.\n");
	}

	while (out != -1)
	{
		gainfn.push_back(fd.name);
		out = _findnext(handle, &fd);
	}
	_findclose(handle);
}