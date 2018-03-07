// floorplan.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h" // 固定函数头文件
#include "userfuncs.h" // 包含自定义函数的头文件

//  main函数
int main()
{
	string filename;
	Mat outline,stdoutline;
	//Mat extractol(string filename);
	//Mat standardol(Mat outline);

// 【1】 输入户型图文件名
	cout << "【1】Please input the floor plan name:\n";
	cin >> filename;
	cout << "Succeed!\n\n";
	
// 【2】 提取轮廓
	cout << "【2】Extract the outline:";
	outline = extractol(filename);
	cout << "Succeed!\n\n";

// 【3】 标准化
	cout << "【3】Standerization (1000X1000pixels):";
	stdoutline = standardol(outline);
	cout << "Succeed!\n\n";

// 【4】 输出结果
	/*namedWindow("贴边轮廓");
	imshow("贴边轮廓", outline);
	waitKey(0);*/
}
