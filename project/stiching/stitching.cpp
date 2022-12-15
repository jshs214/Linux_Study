#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include <iostream>
#include <string.h>	//use strcat
using namespace std;
using namespace cv;

bool divide_images = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;


int main(int argc, char* argv[])
{
	char output[100], savefile[100]="./output/";
	strcpy(output, argv[1]);
	vector<cv::String> fn;
	String path("../image/*.jpg");	// file's path
	glob(path, fn, false);

	vector<cv::String>::iterator iter;
	iter = fn.begin();
	for(iter = fn.begin(); iter != fn.end(); iter++){
		cout <<"---" << *iter<< "---"<<endl;
	}

	vector<Mat> imgs;
	size_t count = fn.size(); 
	for (size_t i=0; i<count; i++)
		imgs.push_back(imread(fn[i]));

	Mat pano;			//panorama image Mattrix data
	Ptr<Stitcher> stitcher = Stitcher::create(mode);
	Stitcher::Status status = stitcher->stitch(imgs, pano);
	
	int imagesize = pano.rows * pano.cols;
	printf("width x height : %dX%d\n",pano.rows , pano.cols);
	printf("imagesize : %d\n", imagesize);

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return EXIT_FAILURE;
	}
	strcat(savefile,output);	//path+save filename
	
	imshow("result panorama image", pano);
	imwrite(savefile, pano);	
	waitKey(0);

	cout << "stitching completed successfully\n" << endl;
	

	return 0;
}
