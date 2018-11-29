// lesson05.cpp : 定义控制台应用程序的入口点。
// 多光谱图像与全色图像融合

#include "stdafx.h"
#include "gdal\gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

int main()
{
	/*全光谱图像与全色图像融合*/

	/*参数定义*/
	/*输入路径、输出路径*/
	char *panPath = "American_Pan.bmp";//全色图像
	char *mulPath = "American_Mul.bmp";//多光谱图像
	char *fusPath = "American_Fus.tif";//融合图像
	/*全色图像*/
	GDALDataset *im_pan;
	/*多光谱图像*/
	GDALDataset *im_mul;
	/*输出融合结果*/
	GDALDataset *im_fus;
	/*图像大小*/
	int im_Xlen, im_Ylen,bandNum;
	/*多光谱图像R、G、B分量记录缓存*/
	float *bandR, *bandG, *bandB;
	/*多光谱图像I分量、H分量和S分量记录缓存*/
	float *bandI, *bandH, *bandS;
	/*全色图像是单通道的*/
	float *bandP;

	GDALAllRegister();
	/*打开输入图像*/
	im_pan = (GDALDataset*)GDALOpenShared(panPath, GA_ReadOnly);
	im_mul = (GDALDataset*)GDALOpenShared(mulPath, GA_ReadOnly);
	
	/*获取图像宽度和高度*/
	im_Xlen = im_mul->GetRasterXSize();
	im_Ylen = im_mul->GetRasterYSize(); //已预处理
	bandNum = im_mul->GetRasterCount();

	/*创建输出图像*/
	im_fus = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
	fusPath,im_Xlen,im_Ylen,bandNum,GDT_Byte,NULL);

	/*为7个缓存分配存储空间*/
	bandR = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandG = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandB = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandH = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandS = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));
	bandP = (float*)CPLMalloc(im_Xlen*im_Ylen * sizeof(float));

	/*获取mul全色图像的RGB*/
	im_mul->GetRasterBand(1)->RasterIO(GF_Read, 0, 0,
		im_Xlen,im_Ylen,bandR,im_Xlen,im_Ylen, GDT_Float32,0,0);
	im_mul->GetRasterBand(2)->RasterIO(GF_Read, 0, 0,
		im_Xlen, im_Ylen, bandG, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	im_mul->GetRasterBand(3)->RasterIO(GF_Read, 0, 0,
		im_Xlen, im_Ylen, bandB, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	/*获取pan全色图像的单通道像素值*/
	im_pan->GetRasterBand(1)->RasterIO(GF_Read, 0, 0,
		im_Xlen, im_Ylen, bandP, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	
	/*RGB2IHS,添加bandP，IHS2RGB*/
	for (int i = 0; i < im_Xlen*im_Ylen; i++)
	{
		bandH[i] = -sqrt(2.0f) / 6.0f*bandR[i] - sqrt(2.0f) / 6.0f*bandG[i] + sqrt(2.0f) / 3.0f*bandB[i];
		bandS[i] = 1.0f / sqrt(2.0f)*bandR[i] - 1 / sqrt(2.0f)*bandG[i];
		/*添加bandP之后还原到RGB*/
		bandR[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] + 1.0f / sqrt(2.0f)*bandS[i];
		bandG[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] - 1.0f / sqrt(2.0f)*bandS[i];
		bandB[i] = bandP[i] + sqrt(2.0f)*bandH[i];
	}
	/*输出*/
	im_fus->GetRasterBand(1)->RasterIO(GF_Write,0,0,
		im_Xlen,im_Ylen,bandR,im_Xlen,im_Ylen,GDT_Float32,0,0);
	im_fus->GetRasterBand(2)->RasterIO(GF_Write, 0, 0,
		im_Xlen, im_Ylen, bandG, im_Xlen, im_Ylen, GDT_Float32, 0, 0);
	im_fus->GetRasterBand(3)->RasterIO(GF_Write, 0, 0,
		im_Xlen, im_Ylen, bandB, im_Xlen, im_Ylen, GDT_Float32, 0, 0);

	/*释放内存*/
	CPLFree(bandR);
	CPLFree(bandG);
	CPLFree(bandB);
	CPLFree(bandH);
	CPLFree(bandS);
	CPLFree(bandP);
	/*关闭dataset*/
	GDALClose(im_pan);
	GDALClose(im_mul);
	GDALClose(im_fus);
    return 0;
}

