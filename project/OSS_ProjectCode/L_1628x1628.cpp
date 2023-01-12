#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>

/* Linux File in Dir find include */
#include <sys/types.h>
#include <dirent.h>
#include <error.h>

using namespace std;

typedef unsigned short ushort;

void dirDarkfile();		// ������ ����� Dark ���丮 �� ��� raw������ darkfn ���Ϳ� pushback �ϴ� �Լ� 
void dirGainfile();		// ������ ����� Gain ���丮 �� ��� raw������ gainfn ���Ϳ� pushback �ϴ� �Լ� 
void darkMap();			// darkfn�� ��հ����� darkMap.raw ���� �Լ�
void gainMap();			// gainfn�� ��հ����� darkMap.raw ���� �Լ�


vector<string> darkfn;	// Dark ���丮 �� raw���ϵ��� �����ϰ� �ִ� ����
vector<string> gainfn;	// Gain ���丮 �� raw���ϵ��� �����ϰ� �ִ� ����

void callibration();	// ���� �Լ�

int main()
{
	dirDarkfile();
	dirGainfile();
	darkMap();
	gainMap();

	callibration();

	printf("Success processing !!\n");
	return 0;
}

void darkMap() {
	FILE* infp, * outfp;
	char savefile[] = "./output1628x1628/DarkMap(1628).raw";

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

		//cout << file << endl;	// ���� fopen Ȯ��

		if ((infp = fopen(file.c_str(), "rb")) == NULL) {
			printf("%d No such file or folder\n", __LINE__);
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


void gainMap() {
	FILE* infp, * outfp;
	char savefile[] = "./output1628x1628/GainMap(1628).raw";

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

		//cout << file << endl;	// ���� fopen Ȯ��

		if ((infp = fopen(file.c_str(), "rb")) == NULL) {
			printf("%d No such file or folder\n", __LINE__);
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


void callibration() {
	FILE* GainMapFp, * darkMapFp, * MTF_VFp, *outfp;
	char savefile[] = "./output1628x1628/callibration(1628).raw";

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

	if ((darkMapFp = fopen("./output1628x1628/DarkMap(1628).raw", "rb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
		return;
	}
	if ((GainMapFp = fopen("./output1628x1628/GainMap(1628).raw", "rb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
		return;
	}
	if ((MTF_VFp = fopen("./S1_1628x1628/MTF_V.raw", "rb")) == NULL) {
		printf("%d No such file or folder\n", __LINE__);
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
	DIR *dir;
	struct dirent *ent;
	dir = opendir ("./S1_1628x1628/Dark/");
	if (dir != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			string file = ent->d_name;
			if( file.find(".raw") ==string::npos ) continue;
			else{
				darkfn.push_back(ent->d_name);
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return;
	}
}

// Gain 디렉토리 내 파일 찾기 함수
void dirGainfile()
{
	DIR *dir;
	struct dirent *ent;
	dir = opendir ("./S1_1628x1628/Gain/");
	if (dir != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			string file = ent->d_name;
			if( file.find(".raw") ==string::npos ) continue;
			else{
				gainfn.push_back(ent->d_name);
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return;
	}
}
