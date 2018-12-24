// lesson05.cpp : �������̨Ӧ�ó������ڵ㡣
// �����ͼ����ȫɫͼ���ں�

#include "stdafx.h"
#include "gdal\gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

int main()
{
	/*ȫ����ͼ����ȫɫͼ���ں�*/

	/*��������*/
	/*����·�������·��*/
	char *panPath = "American_Pan.bmp";//ȫɫͼ��
	char *mulPath = "American_Mul.bmp";//�����ͼ��
	char *fusPath = "American_Fus.tif";//�ں�ͼ��
	/*ȫɫͼ��*/
	GDALDataset *im_pan;
	/*�����ͼ��*/
	GDALDataset *im_mul;
	/*����ںϽ��*/
	GDALDataset *im_fus;
	/*ͼ���С*/
	int im_Xlen, im_Ylen,bandNum;
	/*�����ͼ��R��G��B������¼����*/
	float *bandR, *bandG, *bandB;
	/*�����ͼ��I������H������S������¼����*/
	float *bandI, *bandH, *bandS;
	/*ȫɫͼ���ǵ�ͨ����*/
	float *bandP;

	GDALAllRegister();
	/*������ͼ��*/
	im_pan = (GDALDataset*)GDALOpenShared(panPath, GA_ReadOnly);
	im_mul = (GDALDataset*)GDALOpenShared(mulPath, GA_ReadOnly);
	
	/*��ȡͼ���Ⱥ͸߶�*/
	im_Xlen = im_mul->GetRasterXSize();
	im_Ylen = im_mul->GetRasterYSize(); //��Ԥ����
	bandNum = im_mul->GetRasterCount();

	/*�������ͼ��*/
	im_fus = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
	fusPath,im_Xlen,im_Ylen,bandNum,GDT_Byte,NULL);

	/*Ϊ7���������洢�ռ�*/
	bandR = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandG = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandB = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandH = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandS = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandP = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));

	/*��ȡmulȫɫͼ���RGB*/
	im_mul->GetRasterBand(1)->RasterIO(GF_Read, 0, 0,
		im_Xlen,im_Ylen,bandR,im_Xlen,im_Ylen, GDT_Float32,0,0);
	im_mul->GetRasterBand(2)->RasterIO(GF_Read, 0, 0,
		im_Xlen, im_Ylen, bandG, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	im_mul->GetRasterBand(3)->RasterIO(GF_Read, 0, 0,
		im_Xlen, im_Ylen, bandB, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	/*��ȡpanȫɫͼ��ĵ�ͨ������ֵ*/
	im_pan->GetRasterBand(1)->RasterIO(GF_Read, 0, 0,
		im_Xlen, im_Ylen, bandP, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	
	/*RGB2IHS,���bandP��IHS2RGB*/
	for (int i = 0; i < im_Xlen*im_Ylen; i++)
	{
		bandH[i] = -sqrt(2.0f) / 6.0f*bandR[i] - sqrt(2.0f) / 6.0f*bandG[i] + sqrt(2.0f) / 3.0f*bandB[i];
		bandS[i] = 1.0f / sqrt(2.0f)*bandR[i] - 1 / sqrt(2.0f)*bandG[i];
		/*���bandP֮��ԭ��RGB*/
		bandR[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] + 1.0f / sqrt(2.0f)*bandS[i];
		bandG[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] - 1.0f / sqrt(2.0f)*bandS[i];
		bandB[i] = bandP[i] + sqrt(2.0f)*bandH[i];
	}
	/*���*/
	im_fus->GetRasterBand(1)->RasterIO(GF_Write,0,0,
		im_Xlen,im_Ylen,bandR,im_Xlen,im_Ylen,GDT_Float32,0,0);
	im_fus->GetRasterBand(2)->RasterIO(GF_Write, 0, 0,
		im_Xlen, im_Ylen, bandG, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	im_fus->GetRasterBand(3)->RasterIO(GF_Write, 0, 0,
		im_Xlen, im_Ylen, bandB, im_Xlen, im_Ylen, GDT_Float32, 0, 0);

	/*�ͷ��ڴ�*/
	CPLFree(bandR);
	CPLFree(bandG);
	CPLFree(bandB);
	CPLFree(bandH);
	CPLFree(bandS);
	CPLFree(bandP);
	/*�ر�dataset*/
	GDALClose(im_pan);
	GDALClose(im_mul);
	GDALClose(im_fus);
    return 0;
}

