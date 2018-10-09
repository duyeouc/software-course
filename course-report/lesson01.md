## Experience 1：GDAL +  VS2015配置

### 1. 本周内容

- 了解遥感
- 了解本课程的主要任务是：实现多光谱图像与全色图像融合，生成融合图像
- 了解GDAL库，地址为<https://www.gdal.org/>
- 配置Visual Studio 2015，建立第一个程序，实现图像格式转换

### 2. 实现步骤

1. 建立win32控制台应用程序

2. 在项目目录下导入GDAL库头文件、动态链接dll文件以及相应的lib库文件

3. 主函数内编写代码

   ```
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
   ```

4. 运行。

### 3. 问题以及解决

1. 导入头文件include "./gdal/gdal_priv.h"时，导入了库文件后依旧无法编译通过，在下面添加：

   ```
   #pragma comment(lib, "gdal_i.lib")
   ```

   问题得到解决。

2. 必须将动态链接文件gdal18.dll也导入项目目录下。

![](http://ss1.sinaimg.cn/large/006OpZDely1fvtnasy1ncj30ev07mjrz&690)

### 4. GDAL库函数的使用记录

1. 读取图像
   ```
    GDALAllRegister();    
    GDALDataset *poDataset;
    QString filename;
    filename=QFileDialog::getOpenFileName(this,
                                          tr("Choose Images"),
                                          tr("All Fles (*.*)"));
    //Open the image
    QByteArray ba = filename.toLatin1();
    poDataset = (GDALDataset*) GDALOpen( ba.data(),GA_ReadOnly );
   ```

2. 获取图像的基本信息

   描述信息：const char*  GDALDataset::GetDriver()->GetDescription()，通常是图像的格式<br>
   图像大小：  <br>
      图像宽度  int  GDALDataset::GetRasterXSize() <br>
      图像高度  int  GDALDataset::GetRasterYSize() <br>
      波段数：  int  GDALDataset::GetRasterCount()  <br>
   获取波段的方法： 
         ```
         GDALRasterBand *poBand;
         poBand = poDataset->GetRasterBand(i)     poBand为指向第i个波段的指
         ```
   波段尺寸：
         ```
         int  poBand->GetXSize();
         int  poBand->GetYSize()
         ```
3. 关于RasterIO
   
   函数原型：
   ```
   CPLErr GDALRasterBand::RasterIO (   GDALRWFlag eRWFlag,
   int     nXOff,
   int     nYOff,
   int     nXSize,
   int     nYSize,
   void *  pData,
   int     nBufXSize,
   int     nBufYSize,
   GDALDataType    eBufType,
   int     nPixelSpace,
   int     nLineSpace 
   )
   ```
   参数解释：
   ```
   GDALRWFlag：GF_Read 和GF_Write，分别表示读取数据和写入数据;
   nXOff, nYOff：表示读取或者写入图像数据的起始坐标图像的左上角坐标;
   nXSize, nYSize:
      表示读取或者写入图像数据的窗口大小，nXSize表示宽度，nYSize表示高度;
   pData:指向存储数据的一个指针;
   nBufXSize,nBufYSize:
       指定缓冲区的大小;
       pData的大小应当是nBufXSize×nBufYSize;
   eBufType:缓冲区数据类型;
   nPixelSpace，nLineSpace：
       0作为缺省值;
       可以用于控制存取的内存数据的排列顺序;
       可以使用这两个参数将图像数据按照另一种组织形式读取内存缓冲区中;
   
   ```
