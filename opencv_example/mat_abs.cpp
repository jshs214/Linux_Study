#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
	Mat image1 = imread("./image/lena.bmp", 0);
	Mat image2 = imread("./image/mandrill.bmp", 0);
	CV_Assert(image1.data && image2.data);

	Mat dif_img, abs_dif1, abs_dif2;
	image1.convertTo(image1, CV_16S);
	image2.convertTo(image2, CV_16S);
	subtract(image1, image2, dif_img);

	Rect roi(10, 10, 7, 3);
	cout <<"[diff_img] = " << endl <<dif_img(roi) << endl;
	abs_dif1 = abs(dif_img);

	image1.convertTo(image1, CV_8U);
	image2.convertTo(image2, CV_8U);

	absdiff(image1, image2, abs_dif2);
	
	cout <<"[dif_img] = " << endl << dif_img(roi) <<endl << endl;
	cout <<"[dif_img1] = " << endl << abs_dif1(roi)  << endl;
	cout <<"[dif_img2] = " << endl << abs_dif2(roi)  << endl;

	imshow("image1", image1),	imshow("image2", image2);
	imshow("dif_img", dif_img);
	imshow("abs_dif1", abs_dif1),	imshow("abs_dif2", abs_dif2);

	waitKey();

	return 0;
}


