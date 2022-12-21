#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	  , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::on_pushButton_clicked()
{
	bool pdf = false, txt = false, jpg = false;
//	char savefile[100]="./output/";
	vector<QString> qfn; //하위 디렉토리의 모든 파일 저장할 벡터
	vector<cv::String> fn; //하위 디렉토리의 모든 파일 저장할 벡터

	QString dir = QFileDialog::getExistingDirectory(this,   //dir은 QFileDialog를 이용해 내가 선택한 폴더의 경로
			"Select Dir", QDir::currentPath(), QFileDialog::DontUseNativeDialog);

	QDir filename(dir);

	QFileInfoList fileList = filename.entryInfoList();

	for (int i = 0; i < fileList.size(); ++i) {
		QFileInfo fileInfo = fileList.at(i);
		pdf = fileInfo.fileName().contains(".pdf", Qt::CaseInsensitive); // 문자열 포함여부 검사
		txt = fileInfo.fileName().contains(".txt", Qt::CaseInsensitive); // 문자열 포함여부 검사
		jpg = fileInfo.fileName().contains(".jpg", Qt::CaseInsensitive); // 문자열 포함여부 검사

		if(pdf == true || txt == true || jpg == true){
			qfn.push_back( dir +"/" + fileInfo.fileName());
		}
		else continue;
	}

	vector<QString>::iterator qiter;
	qiter = qfn.begin();
	for(qiter = qfn.begin(); qiter != qfn.end(); qiter++){
		//fn.push_back(*qiter) ;
		fn.push_back((*qiter).toStdString()) ;
	}

	/* 파일 목록 확인용 */
	vector<cv::String>::iterator iter;
	iter = fn.begin();
	for(iter = fn.begin(); iter != fn.end(); iter++){
		cout << *iter << endl ;
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
		//return ;
	}
	//strcat(savefile,output);	//path+save filename

	imshow("result panorama image", pano);

	imwrite("Stitching_Image.jpg", pano);	
	waitKey(0);

}

