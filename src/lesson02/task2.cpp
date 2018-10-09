//task2
/*
	要求：
	把输入图像中起始点（300，300），宽度为100像素，高度为50像素的区域置为白色，
	同时，把输入图像中起始点为（500，500），宽度为50像素，高度为100像素的区域置为黑色。
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
	//输入图像
	GDALDataset* poSrcDs;
	//输出图像
	GDALDataset* poDstDs;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	//输入图像路径
	char* srcPath = "QingDao.jpg";
	//输出图像路径
	char* dstPath = "task2_QingDao2.tif";
	//图像内存存储
	GByte* buffTmp;
	//图像波段数
	int i, bandNum;
	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDs = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//获取图像高度，宽度和波段数量
	imgXlen = poSrcDs->GetRasterXSize();
	imgYlen = poSrcDs->GetRasterYSize();
	bandNum = poSrcDs->GetRasterCount();
	//输出获取结果
	cout << "Image X Length:" << imgXlen << endl;
	cout << "Image Y Length:" << imgYlen << endl;
	cout << "Band number:" << bandNum << endl;
	//根据图像的高度宽度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDs = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//一个一个波段的输入，然后一个一个波段的输出
	for (i = 0; i < bandNum; i++) {
		poSrcDs->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		//更改指定区域所有通道为白色
		for (int jj = StartY; jj < tmpYlen + StartY; jj++) {
			for (int ii = StartX; ii < StartX + tmpXlen; ii++) {
				buffTmp[jj*imgXlen + ii] = (GByte)255;
			}
		}
		//更改指定区域所有通道为黑色
		for (int jj = StartY2; jj < tmpYlen2 + StartY2; jj++) {
			for (int ii = StartX2; ii < StartX2 + tmpXlen2; ii++) {
				buffTmp[jj*imgXlen + ii] = (GByte)0;
			}
		}
		poDstDs->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		printf("... ... band %d processing ... ...\n", i);
	}
	//清除内存
	CPLFree(buffTmp);
	//关闭dataset
	GDALClose(poDstDs);
	GDALClose(poSrcDs);
	system("PAUSE");
}