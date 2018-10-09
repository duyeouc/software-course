//task2
/*
	Ҫ��
	������ͼ������ʼ�㣨300��300�������Ϊ100���أ��߶�Ϊ50���ص�������Ϊ��ɫ��
	ͬʱ��������ͼ������ʼ��Ϊ��500��500�������Ϊ50���أ��߶�Ϊ100���ص�������Ϊ��ɫ��
*/
#include "stdafx.h"
#include <iostream>
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
using namespace std;

void task2()
{
	int StartX = 300;
	int StartY = 300;
	int tmpXlen = 100;
	int tmpYlen = 50;
	int StartX2 = 500;
	int StartY2 = 500;
	int tmpXlen2 = 50;
	int tmpYlen2 = 100;
	//����ͼ��
	GDALDataset* poSrcDs;
	//���ͼ��
	GDALDataset* poDstDs;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	//����ͼ��·��
	char* srcPath = "QingDao.jpg";
	//���ͼ��·��
	char* dstPath = "task2_QingDao2.tif";
	//ͼ���ڴ�洢
	GByte* buffTmp;
	//ͼ�񲨶���
	int i, bandNum;
	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDs = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//��ȡͼ��߶ȣ���ȺͲ�������
	imgXlen = poSrcDs->GetRasterXSize();
	imgYlen = poSrcDs->GetRasterYSize();
	bandNum = poSrcDs->GetRasterCount();
	//�����ȡ���
	cout << "Image X Length:" << imgXlen << endl;
	cout << "Image Y Length:" << imgYlen << endl;
	cout << "Band number:" << bandNum << endl;
	//����ͼ��ĸ߶ȿ�ȷ����ڴ�
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//�������ͼ��
	poDstDs = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//һ��һ�����ε����룬Ȼ��һ��һ�����ε����
	for (i = 0; i < bandNum; i++) {
		poSrcDs->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		//����ָ����������ͨ��Ϊ��ɫ
		for (int jj = StartY; jj < tmpYlen + StartY; jj++) {
			for (int ii = StartX; ii < StartX + tmpXlen; ii++) {
				buffTmp[jj*imgXlen + ii] = (GByte)255;
			}
		}
		//����ָ����������ͨ��Ϊ��ɫ
		for (int jj = StartY2; jj < tmpYlen2 + StartY2; jj++) {
			for (int ii = StartX2; ii < StartX2 + tmpXlen2; ii++) {
				buffTmp[jj*imgXlen + ii] = (GByte)0;
			}
		}
		poDstDs->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf("... ... band %d processing ... ...\n", i);
	}
	//����ڴ�
	CPLFree(buffTmp);
	//�ر�dataset
	GDALClose(poDstDs);
	GDALClose(poSrcDs);
	system("PAUSE");
}