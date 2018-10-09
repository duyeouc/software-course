## Experience 2：利用GDAL进行图像像素值修改

### 1. 本周内容

- GDAL库的简单学习
- 利用GDAL库进行图像像素值的修改

### 2. 实现步骤

1. 建立win32控制台应用程序；

2. 在项目目录下导入GDAL库头文件、动态链接dll文件以及相应的lib库文件；

3. 本周有两个任务，第一个任务是修改指定区域红色通道像素值，第二个任务是修改两指定区域为白色/黑色，任务二即为“照葫芦画瓢”，确定环境后，建立task1.cpp文件，解决任务一，代码如下：

   ```
   #include "stdafx.h"
   #include <iostream>
   #include "./gdal/gdal_priv.h"
   #pragma comment(lib, "gdal_i.lib")
   using namespace std;
   
   void task1()
   {
   	int StartX = 100;
   	int StartY = 100;
   	int tmpXlen = 200;
   	int tmpYlen = 150;
   	//输入图像
   	GDALDataset* poSrcDs;
   	//输出图像
   	GDALDataset* poDstDs;
   	//图像的宽度和高度
   	int imgXlen, imgYlen;
   	//输入图像路径
   	char* srcPath = "QingDao.jpg";
   	//输出图像路径
   	char* dstPath = "task1_QingDao2.tif";
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
   		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
   	//一个一个波段的输入，然后一个一个波段的输出
   	for (i = 0; i < bandNum; i++) {
   		poSrcDs->GetRasterBand(i + 1)->RasterIO(GF_Read,
   			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
   		if (i == 0) { //更改红色通道
   			for (int jj = StartY; jj < tmpYlen + StartY; jj++) {
   				for (int ii = StartX; ii < StartX + tmpXlen; ii++) {
   					buffTmp[jj*imgXlen + ii] = (GByte)255;
   				}
   			}
   		}
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
   }
   ```

   建立task2.cpp文件，解决任务二，代码如下：

   ```
   //task2
   /*
   	要求：
   	把输入图像中起始点（300，300），宽度为100像素，高度为50像素的区域置为白色，
   	同时，把输入图像中起始点为（500，500），宽度为50像素，高度为100像素的区域置为黑色。
   */
   #include "stdafx.h"
   #include <iostream>
   #include "./gdal/gdal_priv.h"
   #pragma comment(lib, "gdal_i.lib")
   using namespace std;
   
   void task2()
   {
   	int StartX = 300;
   	int StartY = 300;
   	int tmpXlen = 100;
   	int tmpYlen = 50;
   	int StartX2 = 500;
   	int StartY2 = 500;
   	int tmpXlen2 = 50;
   	int tmpYlen2 = 100;
   	//输入图像
   	GDALDataset* poSrcDs;
   	//输出图像
   	GDALDataset* poDstDs;
   	//图像的宽度和高度
   	int imgXlen, imgYlen;
   	//输入图像路径
   	char* srcPath = "QingDao.jpg";
   	//输出图像路径
   	char* dstPath = "task2_QingDao2.tif";
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
   		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
   	//一个一个波段的输入，然后一个一个波段的输出
   	for (i = 0; i < bandNum; i++) {
   		poSrcDs->GetRasterBand(i + 1)->RasterIO(GF_Read,
   			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
   		//更改指定区域所有通道为白色
   		for (int jj = StartY; jj < tmpYlen + StartY; jj++) {
   			for (int ii = StartX; ii < StartX + tmpXlen; ii++) {
   				buffTmp[jj*imgXlen + ii] = (GByte)255;
   			}
   		}
   		//更改指定区域所有通道为黑色
   		for (int jj = StartY2; jj < tmpYlen2 + StartY2; jj++) {
   			for (int ii = StartX2; ii < StartX2 + tmpXlen2; ii++) {
   				buffTmp[jj*imgXlen + ii] = (GByte)0;
   			}
   		}
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
   }
   ```

   主函数文件lesson02_tmp.cpp中，主函数编写如下：

   ```
   #include "stdafx.h"
   #include <iostream>
   #include "./gdal/gdal_priv.h"
   #pragma comment(lib, "gdal_i.lib")
   using namespace std;
   
   int main()
   {
   	void task1();
   	void task2(); //声明函数
   	//task1();
   	task2();
   	return 0;
   }
   ```

### 3. 执行结果

1. 任务一更改像素值后的图片如下所示：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fw2d8el5epj30o30hc7ge.jpg)

2. 任务二更改像素值后图片如下：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fw2d9j1atzj30o40hdn96.jpg)

### 4. 问题以及解决方案

1. VS2015调试时异常。

   **出错信息**：”C:\Windows\SysWOW64\ntdll.dll“ 已加载“C:\Windows\SysWOW64\ntdll.dll”。无法查找或打开 PDB 文件。 无法查找或打开 PDB 文件。

   **解决方案：**设置VS2015调试选项，即:

   ​	`调试-->选项和设置-->勾选启用源服务器支持-->符号-->勾选微软符号服务器-->运行加载。`

2. 不知道怎么回事的bug。

   **出错信息**：0xC0000374 (gdal18.dll)处(位于lesson02.exe 中)引发的异常: 0xC0000005: 写入位置 0xC0000005 时发生访问冲突。  如有适用于此异常的处理程序，该程序便可安全地继续运行。 

   **解决方案**：改成debug调试、release调试都失败了。想来，重启VS，不管用；重启电脑，不管用；运行lesson01的代码确是能成功的，那可能是自己的代码有问题。

   于是，更改代码，不采用老师的代码，我在将原图片复制到目的图片的过程中，顺便将指定区域的像素值更改了，重新编译运行，发现神奇的解决了这个bug。问题解决。

   

   