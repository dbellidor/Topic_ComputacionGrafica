// OpenCV library
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <list>
#include <time.h>
using namespace std;
using namespace cv;
#define VALUE_MAX 10000000

#define CPU

struct match {

	int diffRow;
	int diffCol;
	int diff;

}pos;
list<int> listx;
list<int> listy;
int contador=0;

// Funcion CPU

void GPU_ImgMatching(IplImage* sourceImg, IplImage* patternImg, int value);
// Funcion GPU
void CPU_matching(IplImage* sourceImg, IplImage* patternImg, int value);

__global__ void GPU_Kernel_ImgMatching(unsigned char * d_ImgSrc ,unsigned char *d_pImgSrc,int *d_diffDst,int w1,int h1, int w2, int h2,int value);

int main(int argc, char *argv[]){

	IplImage* sourceImg; 
	IplImage* patternImg;
	
  	
	sourceImg = cvLoadImage("Raspberry.png",0);
	patternImg = cvLoadImage("r1.png",0);

	GPU_ImgMatching(sourceImg,patternImg,83);
	CPU_matching(sourceImg,patternImg,83);
	list<int>::iterator itx;
	list<int>::iterator ity;
	CvPoint pt1, pt2;
	IplImage* original=cvLoadImage("Raspberry.png");
	for(itx=listx.begin(),ity=listy.begin();itx!=listx.end(),ity!=listy.end();itx++,ity++){
		//cout<<"x "<<*itx<<" y "<<*ity ;
		pt1.x = *itx;
		pt1.y = *ity;
		pt2.x = pt1.x + patternImg->width;
		pt2.y = pt1.y + patternImg->height;
		cvRectangle(original,cvPoint(pt1.x,pt1.y),cvPoint(pt2.x,pt2.y),CV_RGB(255,0,0),1,8);
	
	}
	cvSaveImage("match.png", original);


	return 0;

}

void CPU_matching(IplImage* sourceImg, IplImage* patternImg, int value){
	clock_t start, end;
	double cpu_time_used;
	start = clock();

	int w1=sourceImg->width;
	int h1=sourceImg->height;
	int w2=patternImg->width;
	int h2=patternImg->height;
	int R1= (w2*h2) - ((w2*h2*value)/100);
	int R2=0;
	int w=w1-w2;
	int h=h1-h2;
	int x1,y1;
	int x2,y2;
	bool bandera;
	uchar color1, color2;
	  for(y1=0 ; y1 < h ;y1++){
		for( x1=0 ; x1 < w ; x1++){
		    bandera=true;
		    for(y2=0 ; y2 < h2 && bandera ; y2++){
		        for(x2=0 ; x2 < w2 && bandera ; x2++){
			    color1 = sourceImg->imageData[(y1 + y2) * sourceImg->widthStep+x1+x2];
			    color2 = patternImg->imageData[y2 * patternImg->widthStep+x2];
		            if(abs(color2 - color1) >25){
		                R2++;
		                if(R2>R1){
		                    bandera=false;
		                }
		            }
		        }
		    }
		    if(R2<=R1){
		        if(listx.empty() && listy.empty()){
		            listx.push_back(x1);
			    listy.push_back(y1);
		            contador++;
		            
		        }else if(x1>listx.back()+w2/2 || y1>listy.back()+h2/2){
		            listx.push_back(x1);
			    listy.push_back(y1);
		            contador++;
		            
		        }
		    }
		    R2=0;
		}
    	  }

	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Tiempo CPU %lf\n", 100.0*cpu_time_used); 
}

__global__ void GPU_Kernel_ImgMatching(unsigned char * d_ImgSrc ,unsigned char *d_pImgSrc,int *d_diffDst,int w1,int h1, int w2, int h2,int value)
{
	
	int R1= (w2*h2) - ((w2*h2*value)/100);
	int R2=0;
        int w=w1-w2;
	int h=h1-h2;
	bool bandera;
	int x1,y1;
	int x2,y2;
	uchar color1, color2;
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	int contador=0;
	if(tid < w1 ) {
		for(y1=0 ; y1 < h ;y1++){
			for( x1=0 ; x1 < w ; x1++){
			    bandera=true;
			    for(y2=0 ; y2 < h2 && bandera ; y2++){
				for(x2=0 ; x2 < w2 && bandera ; x2++){
				    color1 = d_ImgSrc[(y1 + y2) * w1 +  tid +x2];
				    color2 = d_pImgSrc[y2 * w2 + x2];
				    if(abs(color2 - color1) >25){
				        R2++;
				        if(R2>R1){
				            bandera=false;
				        }
				    }
				}
			    }
			    if(R2<=R1){
				if(d_diffDst==NULL){
					d_diffDst[contador]=x1;
					contador++;
				}
			    }
			    R2=0;
			}
	    	  }
	}


}

void GPU_ImgMatching(IplImage* sourceImg, IplImage* patternImg, int value)
{
	
	int w1=sourceImg->width;
	int h1=sourceImg->height;
	int w2=patternImg->width;
	int h2=patternImg->height;

	// GPU Size
	//size_t d_sizeDiff = sizeof(int) * (w1 - w2 + 1) * (h1 - h2 + 1) ;
	size_t d_sizeDiff = sizeof(int) * 10;
	size_t d_sizeImg  = sizeof(unsigned char) * w1 * h1;
	size_t d_psizeImg = sizeof(unsigned char) * w2 * h2;


	// CPU
	unsigned char *h_ImgSrc  = (unsigned char*)(sourceImg->imageData);
	unsigned char *h_pImgSrc = (unsigned char*)(patternImg->imageData);
	int *h_diffDst = (int *)malloc(d_sizeDiff);

	// GPU 
	int *d_diffDst = NULL;
	unsigned char *d_ImgSrc  = NULL;
	unsigned char *d_pImgSrc = NULL;


	// GPU reserva de memoria
	cudaMalloc((void**)&d_diffDst, d_sizeDiff);
	cudaMalloc((void**)&d_ImgSrc , d_sizeImg);
	cudaMalloc((void**)&d_pImgSrc, d_psizeImg);


	//Copia de host a device
	cudaMemcpy(d_diffDst, h_diffDst, d_sizeDiff, cudaMemcpyHostToDevice);
	cudaMemcpy(d_pImgSrc, h_pImgSrc, d_psizeImg, cudaMemcpyHostToDevice);
	cudaMemcpy(d_ImgSrc , h_ImgSrc , d_sizeImg , cudaMemcpyHostToDevice);

	// Definir el bloque y los hilos
    	dim3 dimGrid(8);
   	dim3 dimBlock(128);
	// Cuda time profile
    	cudaEvent_t start, stop;
    	cudaEventCreate(&start);
    	cudaEventCreate(&stop);
    	cudaEventRecord(start, 0);

	// the kernel function
    	GPU_Kernel_ImgMatching<<<dimGrid, dimBlock>>>(d_ImgSrc, d_pImgSrc, d_diffDst, w1, h1, w2, h2,value);
	// Manejar el error de la función Kernel.
    	cudaError_t cuda_err = cudaGetLastError();
    	if ( cudaSuccess != cuda_err ){
        	printf("Antes de la llamada al kernel: error = %s\n", cudaGetErrorString (cuda_err));
        	exit(1) ;
    	}
	cudaEventRecord(stop, 0);
    	cudaEventSynchronize(stop);

    	float elapsedTime;
    	cudaEventElapsedTime(&elapsedTime, start, stop);
    	printf("Tiempo GPU :\t%13f seg\n", elapsedTime/1000);
	cudaEventDestroy(start);
    	cudaEventDestroy(stop);
	//Copiar memoria de device a host  
    	cudaMemcpy(h_diffDst, d_diffDst, d_sizeDiff, cudaMemcpyDeviceToHost);
}




