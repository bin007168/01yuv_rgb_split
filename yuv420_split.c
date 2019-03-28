/******************************************************
** File Name   : yuv420_split.c
** The author  : hzbin
** E-mail      : hzbin@stonkam.com
** Created Time: 2019年03月26日 星期二 10时31分22秒
*******************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//分离YUV444P像素数据中的Y、U、V分量
int simplest_yuv444_split(char *url, int w, int h,int num){

    	int i = 0;
	FILE *fp=fopen(url,"rb+");
	FILE *fp1=fopen("output_444_y.y","wb+");
	FILE *fp2=fopen("output_444_u.y","wb+");
	FILE *fp3=fopen("output_444_v.y","wb+");
	unsigned char *pic=(unsigned char *)malloc(w*h*3);
 
	for(i=0;i<num;i++){
		fread(pic,1,w*h*3,fp);
		//Y
		fwrite(pic,1,w*h,fp1);
		//U
		fwrite(pic+w*h,1,w*h,fp2);
		//V
		fwrite(pic+w*h*2,1,w*h,fp3);
	}
 
	free(pic);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
 
	return 0;
}

//分离YUV420P像素数据中的Y、U、V分量
int simplest_yuv420_split(char *url, int w, int h,int num){
    
    	int i = 0;
	FILE *fp=fopen(url,"rb+");
	FILE *fp1=fopen("output_420_y.y","wb+");
	FILE *fp2=fopen("output_420_u.y","wb+");
	FILE *fp3=fopen("output_420_v.y","wb+");
 
	unsigned char *pic=(unsigned char *)malloc(w*h*3/2);
 
	for(i=0;i<num;i++){
 
		fread(pic,1,w*h*3/2,fp);
		//Y
		fwrite(pic,1,w*h,fp1);
		//U
		fwrite(pic+w*h,1,w*h/4,fp2);
		//V
		fwrite(pic+w*h*5/4,1,w*h/4,fp3);
	}
 
	free(pic);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
 
	return 0;
}

//分离RGB24像素数据中的R、G、B分量
int simplest_rgb24_split(char *url, int w, int h,int num){
	
	int i,j;
	FILE *fp=fopen(url,"rb+");
	FILE *fp1=fopen("output_r.y","wb+");
	FILE *fp2=fopen("output_g.y","wb+");
	FILE *fp3=fopen("output_b.y","wb+");
 
	unsigned char *pic=(unsigned char *)malloc(w*h*3);
 
	for(i=0;i<num;i++){
 
		fread(pic,1,w*h*3,fp);
 
		for(j=0;j<w*h*3;j=j+3){
			//R
			fwrite(pic+j,1,1,fp1);
			//G
			fwrite(pic+j+1,1,1,fp2);
			//B
			fwrite(pic+j+2,1,1,fp3);
		}
	}
 
	free(pic);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
 
	return 0;
}

//将RGB24格式像素数据封装为BMP图像
int simplest_rgb24_to_bmp(const char *rgb24path,int width,int height,const char *bmppath){
	typedef struct 
	{  
		long imageSize;
		long blank;
		long startPosition;
	}BmpHead;
 
	typedef struct
	{
		long  Length;
		long  width;
		long  height;
		unsigned short  colorPlane;
		unsigned short  bitColor;
		long  zipFormat;
		long  realSize;
		long  xPels;
		long  yPels;
		long  colorUse;
		long  colorImportant;
	}InfoHead;
 
	int i=0,j=0;
	BmpHead m_BMPHeader={0};
	InfoHead  m_BMPInfoHeader={0};
	char bfType[2]={'B','M'};
	int header_size=sizeof(bfType)+sizeof(BmpHead)+sizeof(InfoHead);
	unsigned char *rgb24_buffer=NULL;
	FILE *fp_rgb24=NULL,*fp_bmp=NULL;
 
	if((fp_rgb24=fopen(rgb24path,"rb"))==NULL){
		printf("Error: Cannot open input RGB24 file.\n");
		return -1;
	}
	if((fp_bmp=fopen(bmppath,"wb"))==NULL){
		printf("Error: Cannot open output BMP file.\n");
		return -1;
	}
 
	rgb24_buffer=(unsigned char *)malloc(width*height*3);
	fread(rgb24_buffer,1,width*height*3,fp_rgb24);
 
	m_BMPHeader.imageSize=3*width*height+header_size;
	m_BMPHeader.startPosition=header_size;
 
	m_BMPInfoHeader.Length=sizeof(InfoHead); 
	m_BMPInfoHeader.width=width;
	//BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
	m_BMPInfoHeader.height=-height;
	m_BMPInfoHeader.colorPlane=1;
	m_BMPInfoHeader.bitColor=24;
	m_BMPInfoHeader.realSize=3*width*height;
 
	fwrite(bfType,1,sizeof(bfType),fp_bmp);
	fwrite(&m_BMPHeader,1,sizeof(m_BMPHeader),fp_bmp);
	fwrite(&m_BMPInfoHeader,1,sizeof(m_BMPInfoHeader),fp_bmp);
 
	//BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
	//It saves pixel data in Little Endian
	//So we change 'R' and 'B'
	for(j =0;j<height;j++){
		for(i=0;i<width;i++){
			char temp=rgb24_buffer[(j*width+i)*3+2];
			rgb24_buffer[(j*width+i)*3+2]=rgb24_buffer[(j*width+i)*3+0];
			rgb24_buffer[(j*width+i)*3+0]=temp;
		}
	}
	fwrite(rgb24_buffer,3*width*height,1,fp_bmp);
	fclose(fp_rgb24);
	fclose(fp_bmp);
	free(rgb24_buffer);
	printf("Finish generate %s!\n",bmppath);
	
	return 0;
}

//将RGB24格式像素数据转换为YUV420P格式像素数据*****************************************************
unsigned char clip_value(unsigned char x,unsigned char min_val,unsigned char  max_val){
	
	if(x>max_val){
		return max_val;
	}else if(x<min_val){
		return min_val;
	}else{
		return x;
	}
}
 
//RGB to YUV420
int RGB24_TO_YUV420(unsigned char *RgbBuf,int w,int h,unsigned char *yuvBuf)
{
	int i,j;
	unsigned char *ptrY, *ptrU, *ptrV, *ptrRGB;
	memset(yuvBuf,0,w*h*3/2);
	ptrY = yuvBuf;
	ptrU = yuvBuf + w*h;
	ptrV = ptrU + (w*h*1/4);
	unsigned char y, u, v, r, g, b;
	for (j = 0; j<h;j++){
		ptrRGB = RgbBuf + w*j*3 ;
		for (i = 0;i<w;i++){
			
			r = *(ptrRGB++);
			g = *(ptrRGB++);
			b = *(ptrRGB++);
			y = (unsigned char)( ( 66 * r + 129 * g +  25 * b + 128) >> 8) + 16  ;          
			u = (unsigned char)( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128 ;          
			v = (unsigned char)( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128 ;
			*(ptrY++) = clip_value(y,0,255);
			if (j%2==0&&i%2 ==0){
				*(ptrU++) =clip_value(u,0,255);
			}
			else{
				if (i%2==0){
				*(ptrV++) =clip_value(v,0,255);
				}
			}
		}
	}
	return 0;
}


int simplest_rgb24_to_yuv420(char *url_in, int w, int h,int num,char *url_out){
	
	int i;
	
	FILE *fp=fopen(url_in,"rb+");
	FILE *fp1=fopen(url_out,"wb+");
 
	unsigned char *pic_rgb24=(unsigned char *)malloc(w*h*3);
	unsigned char *pic_yuv420=(unsigned char *)malloc(w*h*3/2);
 
	for(i=0;i<num;i++){
		fread(pic_rgb24,1,w*h*3,fp);
		RGB24_TO_YUV420(pic_rgb24,w,h,pic_yuv420);
		fwrite(pic_yuv420,1,w*h*3/2,fp1);
	}
 
	free(pic_rgb24);
	free(pic_yuv420);
	fclose(fp);
	fclose(fp1);
 
	return 0;
}
//**********************************************************************************************

//生成RGB24格式的彩条测试图************************************************************************
int simplest_rgb24_colorbar(int width, int height,char *url_out){
 
	unsigned char *data=NULL;
	int barwidth;
	char filename[100]={0};
	FILE *fp=NULL;
	int i=0,j=0;
 
	data=(unsigned char *)malloc(width*height*3);
	barwidth=width/8;
 
	if((fp=fopen(url_out,"wb+"))==NULL){
		printf("Error: Cannot create file!");
		return -1;
	}
 
	for(j=0;j<height;j++){
		for(i=0;i<width;i++){
			int barnum=i/barwidth;
			switch(barnum){
			case 0:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=255;
				break;
				   }
			case 1:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			case 2:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=255;
				break;
				   }
			case 3:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			case 4:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=255;
				break;
				   }
			case 5:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			case 6:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=255;
 
				break;
				   }
			case 7:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			}
 
		}
	}
	fwrite(data,width*height*3,1,fp);
	fclose(fp);
	free(data);
 
	return 0;
}
//*************************************************************

int main(int argc,char *argv[])
{
//	simplest_yuv420_split("lena_256x256_yuv420p.yuv",256,256,1);
	
//	simplest_yuv444_split("lena_256x256_yuv444p.yuv",256,256,1);
	
	simplest_rgb24_split("cie1931_500x500.rgb", 500, 500,1);
	
	//simplest_rgb24_to_yuv420("lena_256x256_rgb24.rgb",256,256,1,"output_lena.yuv");
	simplest_rgb24_to_yuv420("cie1931_500x500.rgb",500,500,1,"output_lena123.yuv");
	
	simplest_rgb24_colorbar(640, 360,"colorbar_640x360.rgb");
	simplest_rgb24_colorbar(1280, 720,"colorbar_1280x720.rgb");
	simplest_rgb24_colorbar(1920, 1080,"colorbar_1980x1080.rgb");
	
	simplest_rgb24_to_bmp("colorbar_1280x720.rgb",1280,720,"output_lena_1280x720.bmp");
	simplest_rgb24_to_bmp("colorbar_1980x1080.rgb",1920,1080,"output_lena_1920x1080.bmp");
	
	return 0;
}
