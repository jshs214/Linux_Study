#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void calc_histo(const Mat& image, Mat& hist, int bins, int range_max = 256)
{
	int      histSize[] = { bins };                  // 히스토그램 계급 개수
	float   range[] = { 0, (float)range_max };         // 0번 채널 화소값 범위
	int      channels[] = { 0 };                     // 채널 목록 - 단일 채널
	const float* ranges[] = { range };               // 모든 채널 화소 범위

	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges);
}

void draw_histo(Mat hist, Mat &hist_img, Size size = Size(256, 200))
{
	hist_img = Mat(size, CV_8U, Scalar(255));            // 그래프 행렬
	float bin = (float)hist_img.cols / hist.rows;         // 한 계급 너비
	normalize(hist, hist, 0, hist_img.rows, NORM_MINMAX);

	for (int i = 0; i < hist.rows; i++)
	{
		float start_x = i * bin;                  // 막대 사각형 시작 x 좌표
		float end_x = (i + 1) * bin;               // 막대 사각형 종료 x 좌표
		Point2f pt1(start_x, 0);
		Point2f pt2(end_x, hist.at <float>(i));

		if (pt2.y > 0)
			rectangle(hist_img, pt1, pt2, Scalar(0), -1);   // 막대 사각형 그리기
	}
	flip(hist_img, hist_img, 0);
}

int search_valueIdx(Mat hist, int bias = 0)
{
	for (int i = 0; i < hist.rows; i++) {
		int idx = abs(bias - i);               // 검색 위치 (처음 or 마지막)
		if (hist.at<float>(idx) > 0)   return idx;   // 위치 반환
	}
	return -1;                              // 대상 없음 반환
}

int main()
{
	Mat image = imread("./image/lena.bmp", 0);   // 명암도 영상 로드
	CV_Assert(!image.empty());               // 영상 예외처리

	Mat hist, hist_dst, hist_img, hist_dst_img;
	int histsize = 64, ranges = 256;         // 계급 개수 및 화소범위
	calc_histo(image, hist, histsize, ranges);

	float bin_width = (float)ranges / histsize;   // 계급 너비
	int low_value = (int)(search_valueIdx(hist, 0) * bin_width);   // 최저 화소값
	int high_value = (int)(search_valueIdx(hist, hist.rows-1)* bin_width);   // 최고 화소값
	cout << "high_value = " << high_value << endl;            // 검색 화소값 출력
	cout << "low_value = " << low_value << endl;

	int d_value = high_value - low_value;         // delta value
	Mat dst = (image - low_value) * (255.0 / d_value);

	calc_histo(dst, hist_dst, histsize, ranges);   // 결과영상 히스토그램 재계산
	draw_histo(hist, hist_img);                  // 원본영상 히스토그램 그리기
	draw_histo(hist_dst, hist_dst_img);            // 결과영상 히스토그램 그리기

	imshow("image", image), imshow("hist_img", hist_img);
	imshow("dst", dst), imshow("hist_dst_img", hist_dst_img);
	waitKey();
	return 0;
}
