// lesson03.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//����ͼ��1
	GDALDataset* poSrc_space;
	//����ͼ��2
	GDALDataset* poSrc_superman;
	//���ͼ��
	GDALDataset* poDst;
	//���ͼ��·��
	char* dstPath = "space_superman.jpg";
	//����ͼ��·��1
	char* srcPath1 = "space.jpg";
	char* srcPath2 = "superman.jpg";
	//ͼ���ڴ滺��
	GByte* buffTmp[4];
	GByte* buffTmp2[4];
	GDALAllRegister();

	//��ͼ��
	poSrc_space = (GDALDataset*)GDALOpenShared(srcPath1, GA_ReadOnly);
	poSrc_superman = (GDALDataset*)GDALOpenShared(srcPath2, GA_ReadOnly);
	
	//��ȡ̫��ͼ��Ŀ�ߺͲ�������
	img_spaceXLen = poSrc_space->GetRasterXSize();
	img_spaceYLen = poSrc_space->GetRasterYSize();
	img_spaceBandNum = poSrc_space->GetRasterCount();
	img_supermanXLen = poSrc_superman->GetRasterXSize();
	img_supermanYLen = poSrc_superman->GetRasterYSize();
	img_supermanBandNum = poSrc_superman->GetRasterCount();
	
	//�������ͼ��
	poDst = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath,img_spaceXLen,img_spaceYLen,img_spaceBandNum,GDT_Byte,NULL);

	//�����ڴ�,����spaceͼƬһ������ռ�õ��ڴ��С
	for (int i = 0; i < img_spaceBandNum; i++) {
		buffTmp[i] = (GByte*)CPLMalloc(img_spaceXLen*img_spaceYLen * sizeof(GByte));
	}
	for (int i = 0; i < img_supermanBandNum; i++) {
		buffTmp2[i] = (GByte*)CPLMalloc(img_supermanXLen*img_supermanYLen * sizeof(GByte));
	}


	//һ��һ�����εĶ�ȡ����ͼƬ������ֵ�����������Ļ��д�뵽space��ͼ�񻺴���,��д��Ŀ��ͼ��
	for (int i = 0; i < img_spaceBandNum; i++) {
		poSrc_space->GetRasterBand(i + 1)->RasterIO(GF_Read,
		0,0,img_spaceXLen,img_spaceYLen,buffTmp[i],img_spaceXLen,img_spaceYLen,GDT_Byte,0,0);
	}

	for (int i = 0; i < img_supermanBandNum; i++) {
		poSrc_superman->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, img_supermanXLen, img_supermanYLen, buffTmp2[i], img_supermanXLen, img_supermanYLen, GDT_Byte, 0, 0);
	}

	//����ÿһ�����ص�,�ж��ǲ�����Ļ��������ǣ�д��spaceͼ�񻺴���
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
			if (flag == 1 ) { //���flag��1����˵���������ֵ�ǳ��˵�����
				//��ôҪһ��һ��ͨ��д��
				for (int i = 0; i < img_supermanBandNum; i++) {
					buffTmp[i][k*img_supermanXLen + j] = buffTmp2[i][k*img_supermanXLen + j];
				}
			}
		}
	}
	//д��dst_img������
	for (int i = 0; i < img_spaceBandNum; i++) {
		poDst->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, img_spaceXLen, img_spaceYLen, buffTmp[i], img_spaceXLen, img_spaceYLen, GDT_Byte, 0, 0);
		printf("... ... band %d processing ... ...\n", i);
	}
	//����ڴ�
	for (int i = 0; i < img_supermanBandNum; i++) {
		CPLFree(buffTmp2[i]);
	}
	for (int i = 0; i < img_spaceBandNum; i++) {
		CPLFree(buffTmp[i]);
	}
	//�ر�Dataset
	GDALClose(poDst);
	GDALClose(poSrc_superman);
	GDALClose(poSrc_space);
    return 0;
}

