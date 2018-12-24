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
	dstPath[4] = "output5.jpg";//加128偏移
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
					if (i == 4) {
						result += 128;
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

