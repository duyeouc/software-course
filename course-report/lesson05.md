# Experience 5：IHS遥感图像融合

### 1.本周内容

- 了解IHS颜色模型（亮度、色调、饱和度）以及IHS变换图像融合，其基本思想是在IHS空间中，将低空间分辨率的多光谱图像的亮度分量用高空间分辨率的灰度图象替换。 

- 使用GDAL编程，对多光谱图像和全色图像进行融合。

### 2. 实验步骤

- (1) 将多光谱影像重采样到与全色影像具有相同的分辨率；
- (2) 将多光谱图像的Ｒ、Ｇ、Ｂ三个波段转换到IHS空间，得到Ｉ、Ｈ、Ｓ三个分量；
- (3) 以Ｉ分量为参考，对全色影像进行直方图匹配
- (4) 用全色影像替代Ｉ分量，然后同Ｈ、Ｓ分量一起逆变换到RGB空间，从而得到融合图像。

### 3. 编程实现

实验实现代码如下：

```c++
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
```

### 4.实验结果

实验得到了多光谱图像和全色图像的融合结果，如下所示：

![](http://ww1.sinaimg.cn/large/00632YXjly1fxpa3apar4j30he0hdnl8.jpg)

### 5. 其他问题

本实验主要的问题是RGB颜色空间到IHS空间的转换，在IHS空间完成融合后，转换回RGB颜色空间，重点在于转换公式：

![](http://ww1.sinaimg.cn/large/00632YXjly1fxpa5xs6odj30hu07176c.jpg)

转换矩阵tran1将RGB->IHS，具体转换为：

`bandH[i] = -sqrt(2.0f) / 6.0f*bandR[i] - sqrt(2.0f) / 6.0f*bandG[i] + sqrt(2.0f) / 3.0f*bandB[i];`

`bandS[i] = 1.0f / sqrt(2.0f)*bandR[i] - 1 / sqrt(2.0f)*bandG[i];`

转换矩阵tran2将IHS->RGB，具体转换为：

```c++
bandR[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] + 1.0f / sqrt(2.0f)*bandS[i];
bandG[i] = bandP[i] - 1.0f / sqrt(2.0f)*bandH[i] - 1.0f / sqrt(2.0f)*bandS[i];
bandB[i] = bandP[i] + sqrt(2.0f)*bandH[i];
```
