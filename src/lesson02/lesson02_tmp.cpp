#include "stdafx.h"
#include <iostream>
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
using namespace std;

int main()
{
	int StartX = 100;
	int StartY = 100;
	int tmpXlen = 200;
	int tmpYlen = 150;
	//����ͼ��
	GDALDataset* poSrcDs;
	//���ͼ��
	GDALDataset* poDstDs;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	//����ͼ��·��
	char* srcPath = "QingDao.jpg";
	//���ͼ��·��
	char* dstPath = "res.tif";
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
		if (i == 0) { //���ĺ�ɫͨ��
			for (int jj = StartY; jj < tmpYlen+StartY; jj++) {
				for (int ii = StartX; ii < StartX+tmpXlen; ii++) {
					buffTmp[jj*imgXlen + ii] = (GByte)255;
				}
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
	return 0;
}