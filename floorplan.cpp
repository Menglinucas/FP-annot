// floorplan.cpp : �������̨Ӧ�ó������ڵ㡣

#include "stdafx.h" // �̶�����ͷ�ļ�
#include "userfuncs.h" // �����Զ��庯����ͷ�ļ�

//  main����
int main()
{
	string filename;
	Mat outline,stdoutline;
	//Mat extractol(string filename);
	//Mat standardol(Mat outline);

// ��1�� ���뻧��ͼ�ļ���
	cout << "��1��Please input the floor plan name:\n";
	cin >> filename;
	cout << "Succeed!\n\n";
	
// ��2�� ��ȡ����
	cout << "��2��Extract the outline:";
	outline = extractol(filename);
	cout << "Succeed!\n\n";

// ��3�� ��׼��
	cout << "��3��Standerization (1000X1000pixels):";
	stdoutline = standardol(outline);
	cout << "Succeed!\n\n";

// ��4�� ������
	/*namedWindow("��������");
	imshow("��������", outline);
	waitKey(0);*/
}
