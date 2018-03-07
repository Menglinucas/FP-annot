// extractol.cpp : ��ȡ��������extractol��

#include "stdafx.h"

// ������
Mat extractol(Mat roomchart)
{
	if (!roomchart.data) { printf("there's no picture!"); throw; }

	// ��1�� ������ȡ��Ч����
	Mat fea;
	fea = roomchart < 255 * 0.9316; //threshold(roomchart, fea, 256 * 0.9316, 255, CV_THRESH_BINARY_INV);
									
	Mat element; // ����յ�Ԫ
	element = getStructuringElement(MORPH_RECT, Size(10, 10));
	// �ղ���
	morphologyEx(fea, fea, MORPH_CLOSE, element);
	// �����������ȼ�����
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	// ����fea�е�����contours
	findContours(fea, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE);
	// cout << contours.size() << "   " << roomchart.cols << "   " << roomchart.rows ;
	// �������ж�����������ں�ɫͼ�����ԡ���ɫ�������������
	Mat outline16 = Mat(roomchart.rows, roomchart.cols, CV_16UC1);
	Mat tempim = Mat(roomchart.rows, roomchart.cols, CV_16UC1);
	Mat_<unsigned short>::iterator it = outline16.begin<unsigned short>();
	Mat_<unsigned short>::iterator itend = outline16.end<unsigned short>();
	long long temp, maxpixels = 0;
	int myindex;

	for (int index = 0; index >= 0; index = hierarchy[index][0])
	{
		drawContours(outline16, contours, index, Scalar(index), FILLED, 8, hierarchy);
		temp = count(it, itend, index);
		if (temp > maxpixels)
		{
			maxpixels = temp;
			myindex = index;
		}
		// cout << index << "   " << contourArea(contours[index]) << endl;	
	}
	tempim = 0;
	drawContours(tempim, contours, myindex, Scalar(255), FILLED, 8, hierarchy);

	Mat outline(tempim.rows, tempim.cols, CV_8UC1);
	tempim.convertTo(outline, CV_8UC1);
	// �ղ���
	dilate(outline, outline, getStructuringElement(MORPH_RECT, Size(3, 3)));
	// ������ȡ��Ч��
	outline = roomchart.mul(outline / 255) + (255 - outline);
	
	// ��4�� ������Ч������ȡ���̣��ظ���3������
	for (int i = 5; i > 0; i = i--)
	{
		fea = outline < 255 * 0.9316; //threshold(roomchart, fea, 256 * 0.9316, 255, CV_THRESH_BINARY_INV);
									  // �ı�յ�Ԫ
		element = getStructuringElement(MORPH_RECT, Size(i, i));
		// �ղ���
		morphologyEx(fea, fea, MORPH_CLOSE, element);
		// �����������ȼ�����(֮ǰ�Ѷ���)
		//vector<vector<Point>> contours;
		//vector<Vec4i> hierarchy;
		// ����fea�е�����contours
		findContours(fea, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE);
		// �������ж�����������ں�ɫͼ�����ԡ���ɫ�������������
		outline16 = 52685;
		tempim = 52685;
		//Mat_<unsigned short>::iterator it = outline16.begin<unsigned short>();
		//Mat_<unsigned short>::iterator itend = outline16.end<unsigned short>();
		maxpixels = 0;
		for (int index = 0; index >= 0; index = hierarchy[index][0])
		{
			drawContours(outline16, contours, index, Scalar(index), FILLED, 8, hierarchy);
			temp = count(it, itend, index);
			if (temp > maxpixels)
			{
				maxpixels = temp;
				myindex = index;
			}
			//cout << index << "   " << contourArea(contours[index]) << endl;
		}
		tempim = 0;
		drawContours(tempim, contours, myindex, Scalar(255), FILLED, 8, hierarchy);

		//Mat outline(tempim.rows, tempim.cols, CV_8UC1);
		tempim.convertTo(outline, CV_8UC1);
		// �ղ���
		dilate(outline, outline, getStructuringElement(MORPH_RECT, Size(3, 3)));
		// ������ȡ��Ч��
		outline = roomchart.mul(outline / 255) + (255 - outline);
	}
	
	/*namedWindow("���뻧��ͼ");
	imshow("���뻧��ͼ", outline);
	waitKey(0);*/
	// ��5�� ������������
	Rect outlinebd = boundingRect(outline<255);
	Mat outlineROI = outline(outlinebd);
	int tab = static_cast<int>(min(outlineROI.rows, outlineROI.cols) / 5); // (int) convet double to int, otherwise throwing the error
	Mat newoutline = Mat::ones(outlineROI.rows + 2 * tab + 2, outlineROI.cols + 2 * tab + 2, CV_8UC1) * 255;
	Mat newoutlineROI = newoutline(Rect(tab + 2, tab + 2, outlineROI.cols, outlineROI.rows)); // ע��Rect�ĳ�����
	outlineROI.copyTo(newoutlineROI);
	/*element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(newoutline<255, outline, MORPH_CLOSE, element);*/
	
	//// ��6�� ����������
	//findContours(outline, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//outline = 0;
	//drawContours(outline, contours, 0, Scalar(255), 1, 8, hierarchy);

	return newoutline;
}