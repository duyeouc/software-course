## Experience 3：简单抠像

### 1. 本周内容

- GDAL库的使用
- 利用GDAL库进行简单抠像

### 2. 实现步骤

1. 建立win32控制台应用程序；

2. 在项目目录下导入GDAL库头文件、动态链接dll文件以及相应的lib库文件；

3. 将superman.jpg中的超人抠图放进space.jpg，为了比较我新建了一个space_superman.jpg文件。实现的想法是：获取space图像像素值，获取superman图像像素值，遍历superman，如果不是绿幕，则将该像素点写入space图像像素值的缓冲区。最后将更新后的space图像缓冲区写进输出文件space_superman.jpg中。实现代码如下：

   ```c++
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
   ```

### 3. 执行结果

1. 最后的超人-太空图像如下：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwhciwxaqgj30hq0dbq5s.jpg)

### 4. 问题以及解决方案

1. VS2015调试时异常。

   **出错信息**：0xC0000374 (gdal18.dll)处(位于lesson02.exe 中)引发的异常: 0xC0000005: 写入位置 0xC0000005 时发生访问冲突。  如有适用于此异常的处理程序，该程序便可安全地继续运行。 

   **解决方案：**上一次实验也是遇到了这个问题，不明就里的解决了，这次发现了问题所在：

   代码中，创建输出图像的语句为：

   ```c++
   	//创建输出图像
   	poDst = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
   		dstPath,img_spaceXLen,img_spaceYLen,img_spaceBandNum,GDT_Byte,NULL);
   ```

   由于我写成了：

   ```c++
   	//创建输出图像
   	poDst = GetGDALDriverManager()->GetDriverByName("Gif")->Create(
   		dstPath,img_spaceXLen,img_spaceYLen,img_spaceBandNum,GDT_Byte,NULL);
   ```

   导致出现了异常，改正之后，程序正确编译执行。

   

   