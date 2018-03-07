// interaction.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "userfuncs.h" // �����Զ��庯����ͷ�ļ�

bool select_flag = false; //�����Ƿ�ѡ���жϱ���
Mat srcimage,objimage; //����ԭʼͼ���Ŀ��ͼ��
Mat tmpimage; //����һ����ʱͼ�񣬿�һ����ʱͼ��洢����
Mat noise_element = getStructuringElement(MORPH_RECT, Size(3, 3)); // ����������Ԫ
Rect ROIrect; //����ROI���ο�
vector<Point> points;
int npts[1];

void ROI_select(int event, int x, int y, int type, void*);
void erase_background(int event, int x, int y, int type, void*);
void erase_any(int event, int x, int y, int type, void*);
void joint_any(int event, int x, int y, int type, void*);
void fill_poly(int event, int x, int y, int type, void*);

int main()
{
	//���빤��Ŀ¼����
	string wdname;
	while (1)
	{
		cout << "Input the work dir:" << endl;
		cin >> wdname;
		
		if (wdname == "exit")
		{
			return(0);
		}

		//�ж��ļ����Ƿ���ڣ��������ڣ�������������
		if(_access(wdname.c_str(),0) == -1)
		{
			cout << "can not enter this dir" << endl;
		}
		else
		{
			string newdir = wdname + "/annotations";
			if (_access(newdir.c_str(), 0) == -1)
			{
				_mkdir(newdir.c_str());
			}
			cout << "Start working" << endl;
			cout << "***********************************" << endl;
			break;
		}
	}

	//�����ļ�����
	string fname;
	while (1)
	{
		cout << "Input the correct filename:" << endl;
		cin >> fname;
		if (fname == "exit")
		{
			return(0);
		}
		cout << wdname + "/" + fname << endl;
		//�ж��ļ��Ƿ���ڣ��������ڣ�������������
		ifstream fin(wdname + "/" + fname);
		if (!fin)
		{
			cout << "can not open this file" << endl;
		}
		else break;
	}
	
	//�������������ڣ���ơ�������ڡ���װ�ء�Ŀ��ͼ��
	namedWindow("result window");
	//���������������ڣ���ơ��������ڡ���װ�ء�ԭʼͼ��
	namedWindow("interaction window");
	
	//����ԭʼͼ�񣬻Ҷ�ͼ
	srcimage = imread(wdname+"/"+fname, 0);
	//�ڽ��������г�����ʾԭʼͼ��
	imshow("interaction window", srcimage);
	
	//����ͼƬ���� (width*height < 1.0e6)
	if (srcimage.rows * srcimage.cols > 6.0e5)
	{
		float t = static_cast<float>(sqrt(6.0e5 / srcimage.cols / srcimage.rows));
		resize(srcimage, srcimage, Size(0, 0), t, t); //����t��
	}
	
	//��¡ԭʼͼƬΪĿ��ͼ��
	objimage = srcimage.clone();  // deep copy

	imshow("interaction window", srcimage); //�ڽ�����������ʾԭʼͼ��
	imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
	
	moveWindow("result window", 0, 200);
	moveWindow("interaction window", objimage.cols, 200);

	//��1��������ȡ
	cout << "Extract the outline:\n";
	while (1) //��������,����esc�����˳�
	{
		int key = waitKey(10);
		if (key == 49) //1.��ȡROI
		{
			cout << "Extracting ROI ... ..." << endl;
			while (1)
			{
				setMouseCallback("interaction window", ROI_select, 0);
					
				int key1 = waitKey(10);		
				if (key1 == 32)
				{
					if (ROIrect.width && ROIrect.height) //���κ���
					{
						srcimage = srcimage(ROIrect);
					}

					//��¡ԭʼͼƬΪĿ��ͼ��
					objimage = srcimage.clone();  // deep copy

					imshow("interaction window", srcimage); //�ڽ�����������ʾԭʼͼ��
					imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��

					key1 = -1;
					break;
				}
				else if (key1 == 50 || key1 == 51 || key1 == 52 || key1 == 53)
				{
					key1 = -1;
					break;
				}
				else if (key1 == 27)
				{
					key1 = -1;
					return(0);
				}	
			}
		}
		else if (key == 50) //2.��������
		{
			cout << "Erasing background ... ..." << endl;
			while (1)
			{
				//�����Ӧ�������������֣���Ӧ���������ݲ���
				setMouseCallback("interaction window", erase_background, 0);

				int key1 = waitKey(10);
				if (key1 == 13 || key1 == 49 || key1 == 51 || key1 == 52 || key1 == 53)
				{
					key1 = -1;
					break;
				}	 
				else if (key1 == 27)
				{
					key1 = -1;
					return(0);
				}
			}
		}
		else if (key == 51) //3.��������λ��
		{
			cout << "Erasing anywhere ... ..." << endl;
			while (1)
			{
				//�����Ӧ�������������֣���Ӧ���������ݲ���
				setMouseCallback("interaction window", erase_any, 0);

				int key1 = waitKey(10);
				if (key1 == 13 || key1 == 49 || key1 == 50 || key1 == 52 || key1 == 53)
				{
					key1 = -1;
					break;
				}
				else if (key1 == 27)
				{
					key1 = -1;
					return(0);
				}
			}
		}
		else if (key == 52) //4.��������λ��
		{
			cout << "Jointing anywhere ... ..." << endl;
			while (1)
			{
				//�����Ӧ�������������֣���Ӧ���������ݲ���
				setMouseCallback("interaction window", fill_poly, 0);

				int key1 = waitKey(10);
				if (key1 == 13 || key1 == 49 || key1 == 50 || key1 == 51 || key1 == 53)
				{
					key1 = -1;
					break;
				}
				else if (key1 == 27)
				{
					key1 = -1;
					return(0);
				}
			}
		}
		else if (key == 53) //5.�������
		{
			cout << "Filling poly ... ..." << endl;
			
			while (1)
			{
				//�����Ӧ�������������֣���Ӧ���������ݲ���
				setMouseCallback("interaction window", fill_poly, 0);
				
				int key1 = waitKey(10);
				if (key1 == 32)
				{
					Mat annot = objimage.clone();
					annot = 0;

					npts[0] = int(points.size());
					Point **npoints = new Point*[npts[0]];
					npoints[0] = new Point[npts[0]];
					for (int j = 0; j<npts[0]; j++)
					{
						npoints[0][j] = points[j];
					}
					const Point* ppt[1] = { npoints[0] };

					fillPoly(objimage, ppt, npts, 1, Scalar(0));
					fillPoly(annot, ppt, npts, 1, Scalar(255));
					//polylines(objimage, ppt, npts, 1, 1, Scalar(0), 1, 8, 0);
					imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
					imwrite(wdname+"/annotations/annot-"+fname, annot);
					delete[] npoints;
					points.clear();
					key1 = -1;
				}
				else if (key1 == 13 || key1 == 49 || key1 == 50 || key1 == 51 || key1 == 52)
				{
					key1 = -1;
					break;
				}
				else if (key1 == 27)
				{
					key1 = -1;
					return(0);
				}
			}
		}
		else if (key == 13) //�˳�while
		{
			// ��ȡ����  ////�ò���Ϊ�Ķ���
			/*cout << "Tidying ... ...\n";
			objimage = extractol(objimage);
			srcimage = objimage.clone();
			imshow("interaction window", srcimage);
			imshow("result window", objimage);
			moveWindow("result window", 0, 200);
			moveWindow("interaction window", objimage.cols, 200);*/
			while (1)
			{
				cout << "Input the correct filename:" << endl;
				cin >> fname;
				if (fname == "exit")
				{
					return(0);
				}
				//�ж��ļ��Ƿ���ڣ��������ڣ�������������
				ifstream fin(wdname + "/" + fname);
				if (!fin)
				{
					cout << "can not open this file" << endl;
				}
				else break;

			}
			//����ԭʼͼ�񣬻Ҷ�ͼ
			srcimage = imread(wdname+"/"+fname, 0);
			//�ڽ��������г�����ʾԭʼͼ��
			imshow("interaction window", srcimage);

			//����ͼƬ���� (width*height < 1.0e6)
			if (srcimage.rows * srcimage.cols > 6.0e5)
			{
				float t = static_cast<float>(sqrt(6.0e5 / srcimage.cols / srcimage.rows));
				resize(srcimage, srcimage, Size(0, 0), t, t); //����t��
			}

			//��¡ԭʼͼƬΪĿ��ͼ��
			objimage = srcimage.clone();  // deep copy

			imshow("interaction window", srcimage); //�ڽ�����������ʾԭʼͼ��
			imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��

			moveWindow("result window", 0, 200);
			moveWindow("interaction window", objimage.cols, 200);
			if (key == 53)
			{
				break;
			}
			else if (key == 27)
			{
				return(0);
			}
		}
		else if (key == 27) //�˳�����
		{
			key = -1;
			return(0);
		}
	}

	cout << "Succeed!\n\n";
}

//����ROIѡ��������������Ŀ���ǽ�����ͼ������ȡ������x��yΪͼ�����꣬�����Ǵ�������
void ROI_select(int event, int x, int y, int flags, void*)
{
	Point p1, p2; // ����ȷ�����ε�������

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //�������
		{
			select_flag = true;
			ROIrect.x = x;
			ROIrect.y = y;
		}
		break; //�����¡��¼��������˳�switch

		case(EVENT_MOUSEMOVE): //�ƶ����
		{
			if (select_flag)
			{
				srcimage.copyTo(tmpimage);
				p1 = Point(ROIrect.x, ROIrect.y);
				p2 = Point(x, y);
				rectangle(tmpimage, p1, p2, Scalar(255)); //���ƾ���
				imshow("interaction window", tmpimage); //�ڽ�����������ʾԭʼͼ��
			}
		}
		break; //���ƶ����¼��������˳�switch

		case(EVENT_LBUTTONUP): //̧�����
		{
			//ROI���ο�
			ROIrect = Rect(Point(ROIrect.x,ROIrect.y),Point(x,y));
			select_flag = false;
		}
		break; //��̧���¼��������˳�switch
	}
}

//�����������������������Ŀ����ȥ������ɫ��x��yΪͼ�����꣬�����Ǵ�������
void erase_background(int event, int x, int y, int flags, void*)
{
	uchar pt_value; //ѡ�������ֵ
	Point pt_select; //ѡ���

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //�������
		{
			select_flag = true;
			pt_select = Point(x, y);
			pt_value = srcimage.at<uchar>(y,x);

			// ����ѡ������ֵ��ȵ���������Ϊ��ɫ
			for (int i = 0; i < srcimage.rows; i++)
			{
				uchar *srcdata = srcimage.ptr<uchar>(i);
				uchar *objdata = objimage.ptr<uchar>(i);
				for (int j = 0; j < srcimage.cols; j++)
				{
					if (srcdata[j] >= pt_value - 5 && srcdata[j] <= pt_value + 5) { objdata[j] = 255; }
				}
			}

			// ��ԭʼͼ���ϱ��ѡ���
			circle(srcimage, pt_select, 5, Scalar(255), -1);

			// ��������ȥ����ԵĹ���������
			tmpimage = objimage.clone() < 255 * 0.9316;
			//morphologyEx(tmpimage,	tmpimage, MORPH_OPEN, noise_element);
			objimage = objimage.mul(tmpimage / 255) + (255 - tmpimage);

			imshow("interaction window", srcimage); //�ڽ�����������ʾԭʼͼ��
			imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
		}
		break; //�����¡��¼��������˳�switch

		case(EVENT_MOUSEMOVE): //�ƶ����
		{
			// ����������Ŀ�괰�ڵ�ͬλ��׷��
			if (!select_flag)
			{
				pt_select = Point(x, y);
				pt_value = objimage.at<uchar>(y, x);
				objimage.copyTo(tmpimage);
				// ��ԭʼͼ����ͻ����ʾ��ǰ��
				circle(tmpimage, pt_select, 5, Scalar(0), -1);
				//�ڽ����������ʾĿ��ͼ��
				imshow("result window", tmpimage); 
			}

			// ������������
			if (select_flag)
			{
				pt_select = Point(x, y);
				pt_value = srcimage.at<uchar>(y, x);

				// ����ѡ������ֵ��ȵ���������Ϊ��ɫ
				for (int i = 0; i < srcimage.rows; i++)
				{
					uchar *srcdata = srcimage.ptr<uchar>(i);
					uchar *objdata = objimage.ptr<uchar>(i);
					for (int j = 0; j < srcimage.cols; j++)
					{
						if (srcdata[j] == pt_value) { objdata[j] = 255; }
					}
				}

				// ��ԭʼͼ���ϱ��ѡ���
				circle(srcimage, pt_select, 5, Scalar(255), -1);

				// ��������ȥ����ԵĹ���������
				tmpimage = objimage.clone() < 255 * 0.9316;
				//morphologyEx(tmpimage, tmpimage, MORPH_OPEN, noise_element);
				objimage = objimage.mul(tmpimage / 255) + (255 - tmpimage);

				imshow("interaction window", srcimage); //�ڽ�����������ʾԭʼͼ��
				imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
			}
		}
		break; //���ƶ����¼��������˳�switch

		case(EVENT_LBUTTONUP): //̧�����
		{
			select_flag = false;
		}
		break; //��̧���¼��������˳�switch
	}
}

//��������λ�á�x��yΪͼ�����꣬�����Ǵ�������
void erase_any(int event, int x, int y, int flags, void*)
{
	uchar pt_value; //ѡ��������ֵ
	Point pt_select; //ѡ���

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //�������
		{
			select_flag = true;
			pt_select = Point(x, y);

			circle(objimage, pt_select, 5, Scalar(255), -1); // ��ɫ
			imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
		}
		break; //�����¡��¼��������˳�switch

		case(EVENT_MOUSEMOVE): //�ƶ����
		{
			// ����������Ŀ�괰�ڵ�ͬλ��׷��
			if (!select_flag)
			{
				pt_select = Point(x, y);
				pt_value = objimage.at<uchar>(y, x);
				objimage.copyTo(tmpimage);
				// ��ԭʼͼ����ͻ����ʾ��ǰ��
				circle(tmpimage, pt_select, 5, Scalar(0), -1);
				//�ڽ����������ʾĿ��ͼ��
				imshow("result window", tmpimage);
			}

			// ��������
			if (select_flag)
			{
				select_flag = true;
				pt_select = Point(x, y);

				circle(objimage, pt_select, 5, Scalar(255), -1); // ��ɫ
				imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
			}
		}
		break; //���ƶ����¼��������˳�switch

		case(EVENT_LBUTTONUP): //̧�����
		{
			select_flag = false;
		}
		break; //��̧���¼��������˳�switch
	}
}

//��������λ�á�x��yΪͼ�����꣬�����Ǵ�������
void joint_any(int event, int x, int y, int flags, void*)
{
	uchar pt_value; //ѡ��������ֵ
	Point pt_select; //ѡ���

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //�������
		{
			select_flag = true;
			pt_select = Point(x, y);

			circle(objimage, pt_select, 5, Scalar(0), -1); // ��ɫ
			imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
		}
		break; //�����¡��¼��������˳�switch

		case(EVENT_MOUSEMOVE): //�ƶ����
		{
			// ����������Ŀ�괰�ڵ�ͬλ��׷��
			if (!select_flag)
			{
				pt_select = Point(x, y);
				pt_value = objimage.at<uchar>(y, x);
				objimage.copyTo(tmpimage);
				// ��ԭʼͼ����ͻ����ʾ��ǰ��
				circle(tmpimage, pt_select, 5, Scalar(0), -1);
				//�ڽ����������ʾĿ��ͼ��
				imshow("result window", tmpimage);
			}

			// ���Ӳ���
			if (select_flag)
			{
				select_flag = true;
				pt_select = Point(x, y);

				circle(objimage, pt_select, 5, Scalar(0), -1); // ��ɫ
				imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
			}
		}
		break; //���ƶ����¼��������˳�switch

		case(EVENT_LBUTTONUP): //̧�����
		{
			select_flag = false;
		}
		break; //��̧���¼��������˳�switch
	}
}

void fill_poly(int event, int x, int y, int flags, void*)
{
	Point pt_select; //ѡ���
	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //�������
		{
			select_flag = true;
			pt_select = Point(x, y);
			points.push_back(pt_select);
			circle(objimage, pt_select, 2, Scalar(0), -1); // ��ɫ
			imshow("result window", objimage); //�ڽ����������ʾĿ��ͼ��
		}
		break; //�����¡��¼��������˳�switch

		case(EVENT_LBUTTONUP): //̧�����
		{
			select_flag = false;
		}
		break; //��̧���¼��������˳�switch
	}
}