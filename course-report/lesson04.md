## Experience 4：线性滤波

### 1. 本周内容

- 使用GDAL编程，使用6个卷积核进行卷积，观察效果，说明是哪一种滤波

### 2. 实现步骤

1. 建立win32控制台应用程序；

2. 在项目目录下导入GDAL库头文件、动态链接dll文件以及相应的lib库文件；

3. 编写程序，实现六个卷积核的滤波操作，程序如下：

   ```c++
   // lesson04.cpp : 定义控制台应用程序的入口点。
   // 卷积操作实现
   
   #include "stdafx.h"
   #include <string>
   #include <sstream>
   #include "gdal\gdal_priv.h"
   #pragma comment(lib,"gdal_i.lib")
   using namespace std;
   
   int main()
   {
   	int image_lenaXlen, image_lenaYlen, image_lenaBandNum;
       //输入图像
   	GDALDataset *image_lena;
   	//输出图像：卷积核1-6的操作输出
   	GDALDataset *poDst[6];
   	//输出路径
   	char* dstPath[6];
   	dstPath[0] = "output1.jpg";
   	dstPath[1] = "output2.jpg";
   	dstPath[2] = "output3.jpg";
   	dstPath[3] = "output4.jpg";
   	dstPath[4] = "output5.jpg";
   	dstPath[5] = "output6.jpg";
   	//输入图像路径
   	char* srcPath = "lena.jpg";
   	//图像缓存
   	GByte* buffer;
   	GByte* bufferout;
   	//GByte是unsigned char类型，一个字节最大255，
   	//运算过程中需要用double，涉及浮点运算
   	GDALAllRegister();
   
   	//打开图像
   	image_lena = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
   	//获取图像的宽高和波段数量信息
   	image_lenaXlen = image_lena->GetRasterXSize();
   	image_lenaYlen = image_lena->GetRasterYSize();
   	image_lenaBandNum = image_lena->GetRasterCount();
   
   	//创建输出图像
   	for (int i = 0; i < 6; i++) {
   		poDst[i] = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
   		dstPath[i],image_lenaXlen,image_lenaYlen,image_lenaBandNum,GDT_Byte,NULL);
   	}
   
   	//定义卷积核
   	double kernal[6][6][6] = {
   		{{0,0.2,0},{0.2,0.2,0.2},{0,0.2,0} },
   		{{0.2,0,0,0,0},{0,0.2,0,0,0},{0,0,0.2,0,0},{0,0,0,0.2,0},{0,0,0,0,0.2}},
   		{{-1,-1,-1},{-1,8,-1},{-1,-1,-1}},
   		{{-1,-1,-1},{-1,9,-1},{-1,-1,-1}},
   		{{-1,-1,0},{-1,0,1},{0,1,1}},
   		{{0.0120/25,0.1253/25,0.2736/25,0.1253/25,0.0120/25},
   		 {0.1253/25,0.3054/25,2.8514/25,1.3054/25,0.1253/25},
   		 {0.2736/25,2.8514/25,6.2279/25,2.8514/25,0.2736/25},
   		 {0.1253/25,1.3054/25,2.8514/25,1.3054/25,0.1253/25},
   		 {0.0120/25,0.1253/25,0.2736/25,0.1253/25,0.0120/25}}
   	};
   	//卷积核大小
   	int kernal_size[6] = {3,5,3,3,3,5};
   	
   	//分配空间
   	buffer = (GByte*)CPLMalloc(image_lenaXlen*image_lenaYlen * sizeof(GByte));
   	bufferout = (GByte*)CPLMalloc(image_lenaXlen*image_lenaYlen * sizeof(GByte));
   	for (int i = 0; i < 6; i++) {
   		//6个卷积核对应6个输出
   		int size = kernal_size[i];
   		for (int j = 0; j < image_lenaBandNum; j++) {
   			//读取
   			image_lena->GetRasterBand(j + 1)->RasterIO(GF_Read, 
   				0,0,image_lenaXlen,image_lenaYlen,buffer,image_lenaXlen,image_lenaYlen,GDT_Byte,0,0);
   			for (int k = 0; k < image_lenaXlen; k++) {
   				bufferout[k] = 0;
   				bufferout[image_lenaXlen*(image_lenaYlen - 1) + k] = 0;
   			}
   			for (int k = 0; k < image_lenaYlen; k++) {
   				bufferout[image_lenaXlen*k] = 0;
   				bufferout[image_lenaXlen*k + image_lenaXlen-1] = 0;
   			}
   
   			for (int k1 = 1; k1 < image_lenaYlen - 1; k1++) {
   				for (int k2 = 1; k2 < image_lenaXlen - 1; k2++) {
   					//求坐标(k2,k1)就是：k1 * image_lenaXlen +k2
   					//注意横的是Xlen，竖的是Ylen
   					double result = 0.0;
   					for (int index1 = 0; index1 < size; index1++) {
   						for (int index2 = 0; index2 < size; index2++) {
   							//(k2+index2-1,k1+index1-1)
   							result += kernal[i][index1][index2] * (double)buffer[(k1 + index1 - 1) * image_lenaXlen + k2 + index2 - 1];
   						}
   					}
   					if (result > 255) result = 255;
   					if (result < 0) result = 0;
   					int tmp = (int)result;
   					bufferout[k1 * image_lenaXlen + k2] = (unsigned char)tmp;
   				}
   			}
   			//写入到输出图像
   			poDst[i]->GetRasterBand(j + 1)->RasterIO(GF_Write,
   				0, 0, image_lenaXlen, image_lenaYlen, bufferout, image_lenaXlen, image_lenaYlen, GDT_Byte, 0, 0);
   		}
   	}
   
   	//清除内存
   	CPLFree(bufferout);
   	CPLFree(buffer);
   	//关闭dataset
   	for (int i = 0; i < 6; i++) {
   		GDALClose(poDst[i]);
   	}
   	GDALClose(image_lena);
   	system("PAUSE");
       return 0;
   }
   ```

### 3. 执行结果

原始图片：

![](http://ww1.sinaimg.cn/large/6deb72a3ly1fwo23bb045j20740740ty.jpg)

使用卷积核如下：

![](http://ww1.sinaimg.cn/large/6deb72a3ly1fwo2w4e7huj20go0ehabl.jpg)

![](http://ww1.sinaimg.cn/large/6deb72a3ly1fwo2x14c50j20gl05z0tf.jpg)



1. 使用卷积核1结果：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwp7bhn38zj30730720tr.jpg)

   卷积核1的作用为：**均值模糊**。将当前像素和它的四邻域的像素一起取平均，然后再除以5，或者直接在滤波器的5个地方取0.2的值。

2. 使用卷积核2结果：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwp7bpzxewj3072072dgs.jpg)

   卷积核2的效果是：**运动模糊**。只在左上-右下方向上模糊。

3. 使用卷积核3结果：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwp7bwd9saj3073073ac4.jpg)

   卷积核2的效果是：**边缘检测**。检测了所有方向上的边缘，因为卷积核之和为0，图像会暗。边缘检测滤波器卷积相当于求导的离散版本。

4. 使用卷积核4结果：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwp7c4o4emj3074072go2.jpg)

   卷积核4的效果是：**图像锐化**。计算当前点和周围点的差别，然后把这个差别加到原来的位置上。

5. 使用卷积核5结果：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwp7cbkt1xj3073073dhh.jpg)

   *加上128偏移之后：*

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwp7mgd6owj3072072766.jpg)

   可见，卷积核5是用来实现**浮雕效果**，这是45度浮雕。

6. 使用卷积核6结果：

   ![](http://ww1.sinaimg.cn/large/006OpZDely1fwp7cgf86tj3071071t9n.jpg)

   均值模糊不是很平滑，这个也是**实现模糊，但平滑很多**，类似于高斯模糊。

### 4. 其他

1. 实现的时候，注意```GByte(即unsigned char)```和```double```的相互转化，运算时要采用double运算，因为8位运算可能溢出并且不支持负数和浮点数运算。

2. 边界处理：直接定为```RGB(0,0,0)```。

3. buffer数组和实际图像像素值的转化关系是：

   ```c++
   求坐标(k2,k1)就是：buffer[k1 * image_lenaXlen +k2]
   ```