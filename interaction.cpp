// interaction.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "userfuncs.h" // 包含自定义函数的头文件

bool select_flag = false; //声明是否选中判断变量
Mat srcimage,objimage; //声明原始图像和目标图像
Mat tmpimage; //声明一个临时图像，开一块临时图像存储区域
Mat noise_element = getStructuringElement(MORPH_RECT, Size(3, 3)); // 声明噪声单元
Rect ROIrect; //声明ROI矩形框
vector<Point> points;
int npts[1];

void ROI_select(int event, int x, int y, int type, void*);
void erase_background(int event, int x, int y, int type, void*);
void erase_any(int event, int x, int y, int type, void*);
void joint_any(int event, int x, int y, int type, void*);
void fill_poly(int event, int x, int y, int type, void*);

int main()
{
	//输入工作目录名称
	string wdname;
	while (1)
	{
		cout << "Input the work dir:" << endl;
		cin >> wdname;
		
		if (wdname == "exit")
		{
			return(0);
		}

		//判断文件夹是否存在，若不存在，报错并重新输入
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

	//输入文件名称
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
		//判断文件是否存在，若不存在，报错并重新输入
		ifstream fin(wdname + "/" + fname);
		if (!fin)
		{
			cout << "can not open this file" << endl;
		}
		else break;
	}
	
	//建立处理结果窗口，后称“结果窗口”。装载“目标图像”
	namedWindow("result window");
	//建立交互操作窗口，后称“交互窗口”。装载“原始图像”
	namedWindow("interaction window");
	
	//读入原始图像，灰度图
	srcimage = imread(wdname+"/"+fname, 0);
	//在交互窗口中初步显示原始图像
	imshow("interaction window", srcimage);
	
	//降低图片像素 (width*height < 1.0e6)
	if (srcimage.rows * srcimage.cols > 6.0e5)
	{
		float t = static_cast<float>(sqrt(6.0e5 / srcimage.cols / srcimage.rows));
		resize(srcimage, srcimage, Size(0, 0), t, t); //缩放t倍
	}
	
	//克隆原始图片为目标图像
	objimage = srcimage.clone();  // deep copy

	imshow("interaction window", srcimage); //在交互窗口上显示原始图像
	imshow("result window", objimage); //在结果窗口上显示目标图像
	
	moveWindow("result window", 0, 200);
	moveWindow("interaction window", objimage.cols, 200);

	//【1】轮廓提取
	cout << "Extract the outline:\n";
	while (1) //持续处理,按“esc”键退出
	{
		int key = waitKey(10);
		if (key == 49) //1.提取ROI
		{
			cout << "Extracting ROI ... ..." << endl;
			while (1)
			{
				setMouseCallback("interaction window", ROI_select, 0);
					
				int key1 = waitKey(10);		
				if (key1 == 32)
				{
					if (ROIrect.width && ROIrect.height) //矩形合理？
					{
						srcimage = srcimage(ROIrect);
					}

					//克隆原始图片为目标图像
					objimage = srcimage.clone();  // deep copy

					imshow("interaction window", srcimage); //在交互窗口上显示原始图像
					imshow("result window", objimage); //在结果窗口上显示目标图像

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
		else if (key == 50) //2.擦除背景
		{
			cout << "Erasing background ... ..." << endl;
			while (1)
			{
				//鼠标响应函数。窗口名字，响应函数，传递参数
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
		else if (key == 51) //3.擦除任意位置
		{
			cout << "Erasing anywhere ... ..." << endl;
			while (1)
			{
				//鼠标响应函数。窗口名字，响应函数，传递参数
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
		else if (key == 52) //4.连接任意位置
		{
			cout << "Jointing anywhere ... ..." << endl;
			while (1)
			{
				//鼠标响应函数。窗口名字，响应函数，传递参数
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
		else if (key == 53) //5.填充多边形
		{
			cout << "Filling poly ... ..." << endl;
			
			while (1)
			{
				//鼠标响应函数。窗口名字，响应函数，传递参数
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
					imshow("result window", objimage); //在结果窗口上显示目标图像
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
		else if (key == 13) //退出while
		{
			// 提取轮廓  ////该部分为改动处
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
				//判断文件是否存在，若不存在，报错并重新输入
				ifstream fin(wdname + "/" + fname);
				if (!fin)
				{
					cout << "can not open this file" << endl;
				}
				else break;

			}
			//读入原始图像，灰度图
			srcimage = imread(wdname+"/"+fname, 0);
			//在交互窗口中初步显示原始图像
			imshow("interaction window", srcimage);

			//降低图片像素 (width*height < 1.0e6)
			if (srcimage.rows * srcimage.cols > 6.0e5)
			{
				float t = static_cast<float>(sqrt(6.0e5 / srcimage.cols / srcimage.rows));
				resize(srcimage, srcimage, Size(0, 0), t, t); //缩放t倍
			}

			//克隆原始图片为目标图像
			objimage = srcimage.clone();  // deep copy

			imshow("interaction window", srcimage); //在交互窗口上显示原始图像
			imshow("result window", objimage); //在结果窗口上显示目标图像

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
		else if (key == 27) //退出程序
		{
			key = -1;
			return(0);
		}
	}

	cout << "Succeed!\n\n";
}

//定义ROI选择鼠标操作函数，目的是将户型图大致提取出来。x、y为图像坐标，而不是窗口坐标
void ROI_select(int event, int x, int y, int flags, void*)
{
	Point p1, p2; // 声明确定矩形的两个点

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //按下左键
		{
			select_flag = true;
			ROIrect.x = x;
			ROIrect.y = y;
		}
		break; //“按下”事件结束，退出switch

		case(EVENT_MOUSEMOVE): //移动鼠标
		{
			if (select_flag)
			{
				srcimage.copyTo(tmpimage);
				p1 = Point(ROIrect.x, ROIrect.y);
				p2 = Point(x, y);
				rectangle(tmpimage, p1, p2, Scalar(255)); //绘制矩形
				imshow("interaction window", tmpimage); //在交互窗口上显示原始图像
			}
		}
		break; //“移动”事件结束，退出switch

		case(EVENT_LBUTTONUP): //抬起左键
		{
			//ROI矩形框
			ROIrect = Rect(Point(ROIrect.x,ROIrect.y),Point(x,y));
			select_flag = false;
		}
		break; //“抬起”事件结束，退出switch
	}
}

//定义擦除背景鼠标操作函数，目的是去掉背景色。x、y为图像坐标，而不是窗口坐标
void erase_background(int event, int x, int y, int flags, void*)
{
	uchar pt_value; //选择点像素值
	Point pt_select; //选择点

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //按下左键
		{
			select_flag = true;
			pt_select = Point(x, y);
			pt_value = srcimage.at<uchar>(y,x);

			// 将与选择像素值相等的区域设置为白色
			for (int i = 0; i < srcimage.rows; i++)
			{
				uchar *srcdata = srcimage.ptr<uchar>(i);
				uchar *objdata = objimage.ptr<uchar>(i);
				for (int j = 0; j < srcimage.cols; j++)
				{
					if (srcdata[j] >= pt_value - 5 && srcdata[j] <= pt_value + 5) { objdata[j] = 255; }
				}
			}

			// 在原始图像上标出选择点
			circle(srcimage, pt_select, 5, Scalar(255), -1);

			// 开操作，去除相对的孤立点噪声
			tmpimage = objimage.clone() < 255 * 0.9316;
			//morphologyEx(tmpimage,	tmpimage, MORPH_OPEN, noise_element);
			objimage = objimage.mul(tmpimage / 255) + (255 - tmpimage);

			imshow("interaction window", srcimage); //在交互窗口上显示原始图像
			imshow("result window", objimage); //在结果窗口上显示目标图像
		}
		break; //“按下”事件结束，退出switch

		case(EVENT_MOUSEMOVE): //移动鼠标
		{
			// 交互窗口与目标窗口的同位置追踪
			if (!select_flag)
			{
				pt_select = Point(x, y);
				pt_value = objimage.at<uchar>(y, x);
				objimage.copyTo(tmpimage);
				// 在原始图像上突出显示当前点
				circle(tmpimage, pt_select, 5, Scalar(0), -1);
				//在结果窗口上显示目标图像
				imshow("result window", tmpimage); 
			}

			// 消除背景操作
			if (select_flag)
			{
				pt_select = Point(x, y);
				pt_value = srcimage.at<uchar>(y, x);

				// 将与选择像素值相等的区域设置为白色
				for (int i = 0; i < srcimage.rows; i++)
				{
					uchar *srcdata = srcimage.ptr<uchar>(i);
					uchar *objdata = objimage.ptr<uchar>(i);
					for (int j = 0; j < srcimage.cols; j++)
					{
						if (srcdata[j] == pt_value) { objdata[j] = 255; }
					}
				}

				// 在原始图像上标出选择点
				circle(srcimage, pt_select, 5, Scalar(255), -1);

				// 开操作，去除相对的孤立点噪声
				tmpimage = objimage.clone() < 255 * 0.9316;
				//morphologyEx(tmpimage, tmpimage, MORPH_OPEN, noise_element);
				objimage = objimage.mul(tmpimage / 255) + (255 - tmpimage);

				imshow("interaction window", srcimage); //在交互窗口上显示原始图像
				imshow("result window", objimage); //在结果窗口上显示目标图像
			}
		}
		break; //“移动”事件结束，退出switch

		case(EVENT_LBUTTONUP): //抬起左键
		{
			select_flag = false;
		}
		break; //“抬起”事件结束，退出switch
	}
}

//擦除任意位置。x、y为图像坐标，而不是窗口坐标
void erase_any(int event, int x, int y, int flags, void*)
{
	uchar pt_value; //选择点的像素值
	Point pt_select; //选择点

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //按下左键
		{
			select_flag = true;
			pt_select = Point(x, y);

			circle(objimage, pt_select, 5, Scalar(255), -1); // 白色
			imshow("result window", objimage); //在结果窗口上显示目标图像
		}
		break; //“按下”事件结束，退出switch

		case(EVENT_MOUSEMOVE): //移动鼠标
		{
			// 交互窗口与目标窗口的同位置追踪
			if (!select_flag)
			{
				pt_select = Point(x, y);
				pt_value = objimage.at<uchar>(y, x);
				objimage.copyTo(tmpimage);
				// 在原始图像上突出显示当前点
				circle(tmpimage, pt_select, 5, Scalar(0), -1);
				//在结果窗口上显示目标图像
				imshow("result window", tmpimage);
			}

			// 擦除操作
			if (select_flag)
			{
				select_flag = true;
				pt_select = Point(x, y);

				circle(objimage, pt_select, 5, Scalar(255), -1); // 白色
				imshow("result window", objimage); //在结果窗口上显示目标图像
			}
		}
		break; //“移动”事件结束，退出switch

		case(EVENT_LBUTTONUP): //抬起左键
		{
			select_flag = false;
		}
		break; //“抬起”事件结束，退出switch
	}
}

//连接任意位置。x、y为图像坐标，而不是窗口坐标
void joint_any(int event, int x, int y, int flags, void*)
{
	uchar pt_value; //选择点的像素值
	Point pt_select; //选择点

	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //按下左键
		{
			select_flag = true;
			pt_select = Point(x, y);

			circle(objimage, pt_select, 5, Scalar(0), -1); // 黑色
			imshow("result window", objimage); //在结果窗口上显示目标图像
		}
		break; //“按下”事件结束，退出switch

		case(EVENT_MOUSEMOVE): //移动鼠标
		{
			// 交互窗口与目标窗口的同位置追踪
			if (!select_flag)
			{
				pt_select = Point(x, y);
				pt_value = objimage.at<uchar>(y, x);
				objimage.copyTo(tmpimage);
				// 在原始图像上突出显示当前点
				circle(tmpimage, pt_select, 5, Scalar(0), -1);
				//在结果窗口上显示目标图像
				imshow("result window", tmpimage);
			}

			// 连接操作
			if (select_flag)
			{
				select_flag = true;
				pt_select = Point(x, y);

				circle(objimage, pt_select, 5, Scalar(0), -1); // 白色
				imshow("result window", objimage); //在结果窗口上显示目标图像
			}
		}
		break; //“移动”事件结束，退出switch

		case(EVENT_LBUTTONUP): //抬起左键
		{
			select_flag = false;
		}
		break; //“抬起”事件结束，退出switch
	}
}

void fill_poly(int event, int x, int y, int flags, void*)
{
	Point pt_select; //选择点
	switch (event)
	{
		case(EVENT_LBUTTONDOWN): //按下左键
		{
			select_flag = true;
			pt_select = Point(x, y);
			points.push_back(pt_select);
			circle(objimage, pt_select, 2, Scalar(0), -1); // 黑色
			imshow("result window", objimage); //在结果窗口上显示目标图像
		}
		break; //“按下”事件结束，退出switch

		case(EVENT_LBUTTONUP): //抬起左键
		{
			select_flag = false;
		}
		break; //“抬起”事件结束，退出switch
	}
}