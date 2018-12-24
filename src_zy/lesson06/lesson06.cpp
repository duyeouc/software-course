// lesson06.cpp : �������̨Ӧ�ó������ڵ㡣
// ͼ��ֿ鴦��
// ���ַ�ʽ�ֿ�

#include "stdafx.h"
#include "gdal\gdal_priv.h"
#include<time.h>
#pragma comment(lib,"gdal_i.lib")

int main()
{
	clock_t start, finish;
	/*����ͼ��*/
	GDALDataset *im_pan; //ȫɫͼ��
	GDALDataset *im_mul; //�����ͼ��
	int im_Xlen, im_Ylen, bandNum;
	/*���ͼ��*/
	GDALDataset *im_ful1; //256*256
	GDALDataset *im_ful2; //256*width
	/*����*/
	float *bandR1, *bandG1, *bandB1;
	float *bandH1, *bandS1;
	float *bandP1;
	/*·��*/
	char *panPath = "Pan_large.tif";
	char *mulPath = "Mul_large.tif";
	char *ful1_Path = "American_ful1.tif";
	char *ful2_Path = "Amberican_ful2.tif";

	GDALAllRegister();

	im_pan = (GDALDataset*)GDALOpenShared(panPath, GA_ReadOnly);
	im_mul = (GDALDataset*)GDALOpenShared(mulPath, GA_ReadOnly);
	im_Xlen = im_mul->GetRasterXSize();
	im_Ylen = im_mul->GetRasterYSize();
	bandNum = im_mul->GetRasterCount();
	/*�������*/
	im_ful1 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		ful1_Path,im_Xlen,im_Ylen,bandNum,GDT_Byte,NULL);
	/*ͼ��ֿ���벢����*/
	int subXlen = 256, subYlen = 256;
	int XNum = im_Xlen / subXlen+1;
	int YNum = im_Ylen / subYlen+1;
	/*Ϊ��һ�ַ�ʽ���仺��*/
	bandR1 = (float*)CPLMalloc(subXlen*subYlen * sizeof(float));
	bandG1 = (float*)CPLMalloc(subXlen*subYlen * sizeof(float));
	bandB1 = (float*)CPLMalloc(subXlen*subYlen * sizeof(float));
	bandH1 = (float*)CPLMalloc(subXlen*subYlen * sizeof(float));
	bandS1 = (float*)CPLMalloc(subXlen*subYlen * sizeof(float));
	bandP1 = (float*)CPLMalloc(subXlen*subYlen * sizeof(float));
	
	//X�������һ��X��С
	int last_Xlen = im_Xlen - subXlen*(XNum - 1);
	//Y�������һ�еĴ�С
	int last_Ylen = im_Ylen - subYlen*(YNum - 1);

	printf("Style 1:\n");
	printf("ͼƬ��Ϣ:��С��%d*%d  �ֿ�������%d*%d ��������%d\n", im_Xlen, im_Ylen, XNum, YNum, bandNum);
	/*���յ�һ�ַ�ʽ���ֿ飺bandR,bandG,bandB*/
	start = clock();
	for (int j = 1; j <= YNum; j++) {
		printf("Y epoch:%d.....\n", j);
		if (j != YNum) {
			for (int i = 1; i <= XNum; i++) {
				//printf("  --X epoch:%d.....\n", i);
				if (i != XNum) {
					im_mul->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, subYlen, bandR1, subXlen, subYlen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(2)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, subYlen, bandG1, subXlen, subYlen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(3)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, subYlen, bandB1, subXlen, subYlen, GDT_Float32, 0, 0);
					im_pan->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, subYlen, bandP1, subXlen, subYlen, GDT_Float32, 0, 0);
					/*�任����*/
					for (int k = 0; k < subXlen*subYlen; k++)
					{
						bandH1[k] = -sqrt(2.0f) / 6.0f*bandR1[k] - sqrt(2.0f) / 6.0f*bandG1[k] + sqrt(2.0f) / 3.0f*bandB1[k];
						bandS1[k] = 1.0f / sqrt(2.0f)*bandR1[k] - 1 / sqrt(2.0f)*bandG1[k];
						/*���bandP֮��ԭ��RGB*/
						bandR1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] + 1.0f / sqrt(2.0f)*bandS1[k];
						bandG1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] - 1.0f / sqrt(2.0f)*bandS1[k];
						bandB1[k] = bandP1[k] + sqrt(2.0f)*bandH1[k];
					}
					/*�����ͼ��*/
					/*���*/
					im_ful1->GetRasterBand(1)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, subYlen, bandR1, subXlen, subYlen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(2)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, subYlen, bandG1, subXlen, subYlen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(3)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, subYlen, bandB1, subXlen, subYlen, GDT_Float32, 0, 0);
				}
				else {
					im_mul->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, subYlen, bandR1, last_Xlen, subYlen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(2)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, subYlen, bandG1, last_Xlen, subYlen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(3)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, subYlen, bandB1, last_Xlen, subYlen, GDT_Float32, 0, 0);
					im_pan->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, subYlen, bandP1, last_Xlen, subYlen, GDT_Float32, 0, 0);
					for (int k = 0; k < subXlen*last_Ylen; k++)
					{
						bandH1[k] = -sqrt(2.0f) / 6.0f*bandR1[k] - sqrt(2.0f) / 6.0f*bandG1[k] + sqrt(2.0f) / 3.0f*bandB1[k];
						bandS1[k] = 1.0f / sqrt(2.0f)*bandR1[k] - 1 / sqrt(2.0f)*bandG1[k];
						/*���bandP֮��ԭ��RGB*/
						bandR1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] + 1.0f / sqrt(2.0f)*bandS1[k];
						bandG1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] - 1.0f / sqrt(2.0f)*bandS1[k];
						bandB1[k] = bandP1[k] + sqrt(2.0f)*bandH1[k];
					}
					/*�����ͼ��*/
					/*���*/
					im_ful1->GetRasterBand(1)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, subYlen, bandR1, last_Xlen, subYlen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(2)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, subYlen, bandG1, last_Xlen, subYlen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(3)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, subYlen, bandB1, last_Xlen, subYlen, GDT_Float32, 0, 0);
				}
			}
		}
		else {
			for (int i = 1; i <= XNum; i++) {
				//printf("  --X epoch:%d.....\n", i);
				if (i != XNum) {
					im_mul->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, last_Ylen, bandR1, subXlen, last_Ylen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(2)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, last_Ylen, bandG1, subXlen, last_Ylen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(3)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, last_Ylen, bandB1, subXlen, last_Ylen, GDT_Float32, 0, 0);
					im_pan->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, last_Ylen, bandP1, subXlen, last_Ylen, GDT_Float32, 0, 0);
					/*�任����*/
					for (int k = 0; k < subXlen*last_Ylen; k++)
					{
						bandH1[k] = -sqrt(2.0f) / 6.0f*bandR1[k] - sqrt(2.0f) / 6.0f*bandG1[k] + sqrt(2.0f) / 3.0f*bandB1[k];
						bandS1[k] = 1.0f / sqrt(2.0f)*bandR1[k] - 1 / sqrt(2.0f)*bandG1[k];
						/*���bandP֮��ԭ��RGB*/
						bandR1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] + 1.0f / sqrt(2.0f)*bandS1[k];
						bandG1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] - 1.0f / sqrt(2.0f)*bandS1[k];
						bandB1[k] = bandP1[k] + sqrt(2.0f)*bandH1[k];
					}
					/*�����ͼ��*/
					/*���*/
					im_ful1->GetRasterBand(1)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, last_Ylen, bandR1, subXlen, last_Ylen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(2)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, last_Ylen, bandG1, subXlen, last_Ylen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(3)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						subXlen, last_Ylen, bandB1, subXlen, last_Ylen, GDT_Float32, 0, 0);
				}
				else {
					im_mul->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, last_Ylen, bandR1, last_Xlen, last_Ylen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(2)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, last_Ylen, bandG1, last_Xlen, last_Ylen, GDT_Float32, 0, 0);
					im_mul->GetRasterBand(3)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, last_Ylen, bandB1, last_Xlen, last_Ylen, GDT_Float32, 0, 0);
					im_pan->GetRasterBand(1)->RasterIO(GF_Read, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, last_Ylen, bandP1, last_Xlen, last_Ylen, GDT_Float32, 0, 0);
					/*�任����*/
					for (int k = 0; k < last_Xlen*last_Ylen; k++)
					{
						bandH1[k] = -sqrt(2.0f) / 6.0f*bandR1[k] - sqrt(2.0f) / 6.0f*bandG1[k] + sqrt(2.0f) / 3.0f*bandB1[k];
						bandS1[k] = 1.0f / sqrt(2.0f)*bandR1[k] - 1 / sqrt(2.0f)*bandG1[k];
						/*���bandP֮��ԭ��RGB*/
						bandR1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] + 1.0f / sqrt(2.0f)*bandS1[k];
						bandG1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] - 1.0f / sqrt(2.0f)*bandS1[k];
						bandB1[k] = bandP1[k] + sqrt(2.0f)*bandH1[k];
					}
					/*�����ͼ��*/
					/*���*/
					im_ful1->GetRasterBand(1)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, last_Ylen, bandR1, last_Xlen, last_Ylen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(2)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, last_Ylen, bandG1, last_Xlen, last_Ylen, GDT_Float32, 0, 0);
					im_ful1->GetRasterBand(3)->RasterIO(GF_Write, (i - 1)*subXlen, (j - 1)*subYlen,
						last_Xlen, last_Ylen, bandB1, last_Xlen, last_Ylen, GDT_Float32, 0, 0);
				}
			}
		}
	}
	finish = clock();
	double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("Total time of style 1 is %.2lf s\n", totaltime);
	/*�ͷ��ڴ�*/
	CPLFree(bandR1);
	CPLFree(bandG1);
	CPLFree(bandB1);
	CPLFree(bandH1);
	CPLFree(bandS1);
	CPLFree(bandP1);
	/*�ر�dataset*/
	GDALClose(im_ful1);
	printf("Style 1 end.\n\n");

	/*���յڶ��ַ�ʽ���ֿ飺*/
	/*�������*/
	im_ful2 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		ful2_Path, im_Xlen, im_Ylen, bandNum, GDT_Byte, NULL);
	/*Ϊ�ڶ��ַ�ʽ���·��仺��*/
	YNum = im_Ylen / subYlen + 1;
	last_Ylen = im_Ylen - subYlen*(YNum - 1);//���һ��Ylen��С
	bandR1 = (float*)CPLMalloc(im_Xlen*subYlen * sizeof(float));
	bandG1 = (float*)CPLMalloc(im_Xlen*subYlen * sizeof(float));
	bandB1 = (float*)CPLMalloc(im_Xlen*subYlen * sizeof(float));
	bandH1 = (float*)CPLMalloc(im_Xlen*subYlen * sizeof(float));
	bandS1 = (float*)CPLMalloc(im_Xlen*subYlen * sizeof(float));
	bandP1 = (float*)CPLMalloc(im_Xlen*subYlen * sizeof(float));
	printf("Style 2:\n");

	printf("ͼƬ��Ϣ:��С��%d*%d  �ֿ�������%d*%d ��������%d\n", im_Xlen, im_Ylen, 1, YNum, bandNum);
	start = clock();
	for (int i = 1; i <= YNum; i++) {
		printf("Y epoch:%d.....\n", i);
		if (i != YNum) {
			im_mul->GetRasterBand(1)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, subYlen, bandR1, im_Xlen, subYlen, GDT_Float32, 0, 0);
			im_mul->GetRasterBand(2)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, subYlen, bandG1, im_Xlen, subYlen, GDT_Float32, 0, 0);
			im_mul->GetRasterBand(3)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, subYlen, bandB1, im_Xlen, subYlen, GDT_Float32, 0, 0);
			im_pan->GetRasterBand(1)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, subYlen, bandP1, im_Xlen, subYlen, GDT_Float32, 0, 0);
			/*�任��IHS�ռ䲢�ںϺ����*/
			/*�任����*/
			for (int k = 0; k < im_Xlen*subYlen; k++)
			{
				bandH1[k] = -sqrt(2.0f) / 6.0f*bandR1[k] - sqrt(2.0f) / 6.0f*bandG1[k] + sqrt(2.0f) / 3.0f*bandB1[k];
				bandS1[k] = 1.0f / sqrt(2.0f)*bandR1[k] - 1 / sqrt(2.0f)*bandG1[k];
				/*���bandP֮��ԭ��RGB*/
				bandR1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] + 1.0f / sqrt(2.0f)*bandS1[k];
				bandG1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] - 1.0f / sqrt(2.0f)*bandS1[k];
				bandB1[k] = bandP1[k] + sqrt(2.0f)*bandH1[k];
			}
			/*�����ͼ��*/
			/*���*/
			im_ful2->GetRasterBand(1)->RasterIO(GF_Write, 0, (i - 1)*subYlen,
				im_Xlen, subYlen, bandR1, im_Xlen, subYlen, GDT_Float32, 0, 0);
			im_ful2->GetRasterBand(2)->RasterIO(GF_Write, 0, (i - 1)*subYlen,
				im_Xlen, subYlen, bandG1, im_Xlen, subYlen, GDT_Float32, 0, 0);
			im_ful2->GetRasterBand(3)->RasterIO(GF_Write, 0, (i - 1)*subYlen,
				im_Xlen, subYlen, bandB1, im_Xlen, subYlen, GDT_Float32, 0, 0);
		}
		else {
			im_mul->GetRasterBand(1)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, last_Ylen, bandR1, im_Xlen, last_Ylen, GDT_Float32, 0, 0);
			im_mul->GetRasterBand(2)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, last_Ylen, bandG1, im_Xlen, last_Ylen, GDT_Float32, 0, 0);
			im_mul->GetRasterBand(3)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, last_Ylen, bandB1, im_Xlen, last_Ylen, GDT_Float32, 0, 0);
			im_pan->GetRasterBand(1)->RasterIO(GF_Read, 0, (i - 1)*subYlen,
				im_Xlen, last_Ylen, bandP1, im_Xlen, last_Ylen, GDT_Float32, 0, 0);
			/*�任��IHS�ռ䲢�ںϺ����*/
			/*�任����*/
			for (int k = 0; k < im_Xlen*last_Ylen; k++)
			{
				bandH1[k] = -sqrt(2.0f) / 6.0f*bandR1[k] - sqrt(2.0f) / 6.0f*bandG1[k] + sqrt(2.0f) / 3.0f*bandB1[k];
				bandS1[k] = 1.0f / sqrt(2.0f)*bandR1[k] - 1 / sqrt(2.0f)*bandG1[k];
				/*���bandP֮��ԭ��RGB*/
				bandR1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] + 1.0f / sqrt(2.0f)*bandS1[k];
				bandG1[k] = bandP1[k] - 1.0f / sqrt(2.0f)*bandH1[k] - 1.0f / sqrt(2.0f)*bandS1[k];
				bandB1[k] = bandP1[k] + sqrt(2.0f)*bandH1[k];
			}
			/*�����ͼ��*/
			/*���*/
			im_ful2->GetRasterBand(1)->RasterIO(GF_Write, 0, (i - 1)*subYlen,
				im_Xlen, last_Ylen, bandR1, im_Xlen, last_Ylen, GDT_Float32, 0, 0);
			im_ful2->GetRasterBand(2)->RasterIO(GF_Write, 0, (i - 1)*subYlen,
				im_Xlen, last_Ylen, bandG1, im_Xlen, last_Ylen, GDT_Float32, 0, 0);
			im_ful2->GetRasterBand(3)->RasterIO(GF_Write, 0, (i - 1)*subYlen,
				im_Xlen, last_Ylen, bandB1, im_Xlen, last_Ylen, GDT_Float32, 0, 0);
		}
	}
	finish = clock();
	double totaltime2 = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("Total time of style 1 is %.2lf s\n", totaltime2);
	printf("Style 2 end.\n\n");

	CPLFree(bandR1);
	CPLFree(bandG1);
	CPLFree(bandB1);
	CPLFree(bandH1);
	CPLFree(bandS1);
	CPLFree(bandP1);
	/*�ر�dataset*/
	GDALClose(im_pan);
	GDALClose(im_mul);
	GDALClose(im_ful2);
    return 0;
}