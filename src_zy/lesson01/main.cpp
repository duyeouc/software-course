// main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
using namespace std;

int main()
{
	//输入图像
	GDALDataset* poSrcDs;
	//输出图像
	GDALDataset* poDstDs;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	//输入图像路径
	char* srcPath = "trees.jpg";
	//输出图像路径
	char* dstPath = "res.tif";
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
		dstPath,imgXlen,imgYlen,bandNum,GDT_Byte,NULL);
	//一个一个波段的输入，然后一个一个波段的输出
	for (i = 0; i < bandNum; i++) {
		poSrcDs->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
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
	return 0;
}

