// lesson04.cpp : �������̨Ӧ�ó������ڵ㡣
// �������ʵ��

#include "stdafx.h"
#include <string>
#include <sstream>
#include "gdal\gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")
using namespace std;

int main()
{
	int image_lenaXlen, image_lenaYlen, image_lenaBandNum;
    //����ͼ��
	GDALDataset *image_lena;
	//���ͼ�񣺾����1-6�Ĳ������
	GDALDataset *poDst[6];
	//���·��
	char* dstPath[6];
	dstPath[0] = "output1.jpg";
	dstPath[1] = "output2.jpg";
	dstPath[2] = "output3.jpg";
	dstPath[3] = "output4.jpg"; 
	dstPath[4] = "output5.jpg";//��128ƫ��
	dstPath[5] = "output6.jpg";
	//����ͼ��·��
	char* srcPath = "lena.jpg";
	//ͼ�񻺴�
	GByte* buffer;
	GByte* bufferout;
	//GByte��unsigned char���ͣ�һ���ֽ����255��
	//�����������Ҫ��double���漰��������
	GDALAllRegister();

	//��ͼ��
	image_lena = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	//��ȡͼ��Ŀ�ߺͲ���������Ϣ
	image_lenaXlen = image_lena->GetRasterXSize();
	image_lenaYlen = image_lena->GetRasterYSize();
	image_lenaBandNum = image_lena->GetRasterCount();

	//�������ͼ��
	for (int i = 0; i < 6; i++) {
		poDst[i] = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath[i],image_lenaXlen,image_lenaYlen,image_lenaBandNum,GDT_Byte,NULL);
	}

	//��������
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
	//����˴�С
	int kernal_size[6] = {3,5,3,3,3,5};
	
	//����ռ�
	buffer = (GByte*)CPLMalloc(image_lenaXlen*image_lenaYlen * sizeof(GByte));
	bufferout = (GByte*)CPLMalloc(image_lenaXlen*image_lenaYlen * sizeof(GByte));
	for (int i = 0; i < 6; i++) {
		//6������˶�Ӧ6�����
		int size = kernal_size[i];
		for (int j = 0; j < image_lenaBandNum; j++) {
			//��ȡ
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
					//������(k2,k1)���ǣ�k1 * image_lenaXlen +k2
					//ע������Xlen��������Ylen
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
			//д�뵽���ͼ��
			poDst[i]->GetRasterBand(j + 1)->RasterIO(GF_Write,
				0, 0, image_lenaXlen, image_lenaYlen, bufferout, image_lenaXlen, image_lenaYlen, GDT_Byte, 0, 0);
		}
	}

	//����ڴ�
	CPLFree(bufferout);
	CPLFree(buffer);
	//�ر�dataset
	for (int i = 0; i < 6; i++) {
		GDALClose(poDst[i]);
	}
	GDALClose(image_lena);
	system("PAUSE");
    return 0;
}

