#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat image(200,300, CV_8U, Scalar(255));
	namedWindow("keyboard event", WINDOW_AUTOSIZE);
	imshow("keyboard event", image);
	
	while(1)
	{
		int key = waitKeyEx(100);
		if(key == 27) break;
		
		switch(key)
		{
			
			case 'a': cout << "a key input" << endl; break;
			case 'b': cout << "b key input" << endl; break;
			case 0x41: cout << "A key input" << endl; break;
			case 66: cout << "B key input" << endl; break;

			case 0xff51: cout << "Left key input" << endl; break;
			case 0xff52: cout << "Up key input" << endl; break;
			case 0xff53: cout << "Right key input" << endl; break;
			case 0xff54: cout << "Down key input" << endl; break;
		}
	}

	return 0;
}
