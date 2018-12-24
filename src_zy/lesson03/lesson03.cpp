// lesson03.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include "./gdal/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")
using namespace std;
int main()
{
	int img_spaceXLen, img_spaceYLen, img_spaceBandNum;
	int img_supermanXLen, img_supermanYLen, img_supermanBandNum;
	//输入图像1
	GDALDataset* poSrc_space;
	//输入图像2
	GDALDataset* poSrc_superman;
	//输出图像
	GDALDataset* poDst;
	//输出图像路径
	char* dstPath = "space_superman.jpg";
	//输入图像路径1
	char* srcPath1 = "space.jpg";
	char* srcPath2 = "superman.jpg";
	//图像内存缓冲
	GByte* buffTmp[4];
	GByte* buffTmp2[4];
	GDALAllRegister();

	//打开图像
	poSrc_space = (GDALDataset*)GDALOpenShared(srcPath1, GA_ReadOnly);
	poSrc_superman = (GDALDataset*)GDALOpenShared(srcPath2, GA_ReadOnly);
	
	//获取太空图像的宽高和波段数量
	img_spaceXLen = poSrc_space->GetRasterXSize();
	img_spaceYLen = poSrc_space->GetRasterYSize();
	img_spaceBandNum = poSrc_space->GetRasterCount();
	img_supermanXLen = poSrc_superman->GetRasterXSize();
	img_supermanYLen = poSrc_superman->GetRasterYSize();
	img_supermanBandNum = poSrc_superman->GetRasterCount();
	
	//创建输出图像
	poDst = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath,img_spaceXLen,img_spaceYLen,img_spaceBandNum,GDT_Byte,NULL);

	//分配内存,分配space图片一个波段占用的内存大小
	for (int i = 0; i < img_spaceBandNum; i++) {
		buffTmp[i] = (GByte*)CPLMalloc(img_spaceXLen*img_spaceYLen * sizeof(GByte));
	}
	for (int i = 0; i < img_supermanBandNum; i++) {
		buffTmp2[i] = (GByte*)CPLMalloc(img_supermanXLen*img_supermanYLen * sizeof(GByte));
	}


	//一个一个波段的读取超人图片的像素值，如果不是绿幕，写入到space的图像缓存中,再写到目的图像
	for (int i = 0; i < img_spaceBandNum; i++) {
		poSrc_space->GetRasterBand(i + 1)->RasterIO(GF_Read,
		0,0,img_spaceXLen,img_spaceYLen,buffTmp[i],img_spaceXLen,img_spaceYLen,GDT_Byte,0,0);
	}

	for (int i = 0; i < img_supermanBandNum; i++) {
		poSrc_superman->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, img_supermanXLen, img_supermanYLen, buffTmp2[i], img_supermanXLen, img_supermanYLen, GDT_Byte, 0, 0);
	}

	//遍历每一个像素点,判断是不是绿幕，如果不是，写入space图像缓存区
	for (int k = 0; k < img_supermanYLen; k++) {
		for (int j = 0; j < img_supermanXLen; j++) {
			int flag = 1;
			if (buffTmp2[0][k*img_supermanXLen + j] >= 10 && buffTmp2[0][k*img_supermanXLen + j] <= 160) {
				if (buffTmp2[1][k*img_supermanXLen + j] >= 100 && buffTmp2[1][k*img_supermanXLen + j] <= 220) {
					if(buffTmp2[2][k*img_supermanXLen + j] >= 10 && buffTmp2[2][k*img_supermanXLen + j] <= 110){
						flag = 0;
					}
				}
			}
			if (flag == 1 ) { //如果flag是1，则说明这个像素值是超人的区域
				//那么要一个一个通道写入
				for (int i = 0; i < img_supermanBandNum; i++) {
					buffTmp[i][k*img_supermanXLen + j] = buffTmp2[i][k*img_supermanXLen + j];
				}
			}
		}
	}
	//写入dst_img，保存
	for (int i = 0; i < img_spaceBandNum; i++) {
		poDst->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, img_spaceXLen, img_spaceYLen, buffTmp[i], img_spaceXLen, img_spaceYLen, GDT_Byte, 0, 0);
		printf("... ... band %d processing ... ...\n", i);
	}
	//清除内存
	for (int i = 0; i < img_supermanBandNum; i++) {
		CPLFree(buffTmp2[i]);
	}
	for (int i = 0; i < img_spaceBandNum; i++) {
		CPLFree(buffTmp[i]);
	}
	//关闭Dataset
	GDALClose(poDst);
	GDALClose(poSrc_superman);
	GDALClose(poSrc_space);
    return 0;
}

