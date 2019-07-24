#include <cuda.h>
#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <iostream>
#include "MedianFilter.h"
#include <time.h>

__global__ void kernel_filtro(unsigned char *inputImageKernel, unsigned char *outputImagekernel, int imageWidth, int imageHeight)
{
	// Set row and colum for thread.
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned char filterVector[9] = {0,0,0,0,0,0,0,0,0};   //Longitud de la ventana
	if((row==0) || (col==0) || (row==imageHeight-1) || (col==imageWidth-1))
				outputImagekernel[row*imageWidth+col] = 0; //Condiciones limite
	else {
		for (int x = 0; x < WINDOW_SIZE; x++) { 
			for (int y = 0; y < WINDOW_SIZE; y++){
				filterVector[x*WINDOW_SIZE+y] = inputImageKernel[(row+x-1)*imageWidth+(col+y-1)];   // llenamos el vector de la mediana
			}
		}
		for (int i = 0; i < 9; i++) {
			for (int j = i + 1; j < 9; j++) {
				if (filterVector[i] > filterVector[j]) { 
					//Ordenamiento
					char tmp = filterVector[i];
					filterVector[i] = filterVector[j];
					filterVector[j] = tmp;
				}
			}
		}
		outputImagekernel[row*imageWidth+col] = filterVector[4];   //Guardamos el elemento de la mediana
	}
}


__global__ void CMF_kernel(unsigned char* in, unsigned char * out, int c) {
	// k = input array index
	int k = c+blockDim.x*blockIdx.x+threadIdx.x;
	int M[WINDOW_SIZE] = {0}; //major array
	int e[WINDOW_SIZE] = {0}; //equal array
	int m[WINDOW_SIZE] = {0}; //minor array
	//window index
	int i = k-c+threadIdx.y;
	for (int j=k-c ; j<k-c+WINDOW_SIZE ; j++) {
		if (in[j] > in[i]) M[threadIdx.y]+=1;
		else if (in[j] < in[i]) m[threadIdx.y]+=1;
		else e[threadIdx.y]+=1;
	}
	for (int j = 0; j<WINDOW_SIZE; j++) {
		if (M[j]==c || m[j]==c || e[j]>=c) {
		out[k] = in[k-c+j];
		return;
		}
	}
}


bool GPU_filtro_mediana( Bitmap* image, Bitmap* outputImage, bool sharedMemoryUse ){
	//Eventos de cuda para manejar errores
	cudaEvent_t start,stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start);

	int width = image->Width();
	int height = image->Height();
	int TILE_SIZE=4;
	int size =  width * height * sizeof(char);
	//Inicializamos las imagenes
	unsigned char *deviceinputimage;
	cudaMalloc((void**) &deviceinputimage, size);

	cudaMemcpy(deviceinputimage, image->image, size, cudaMemcpyHostToDevice);

	unsigned char *deviceOutputImage;
	cudaMalloc((void**) &deviceOutputImage, size);
	//Tamanio del bloque y del grid
	dim3 dimBlock(TILE_SIZE, TILE_SIZE);
	dim3 dimGrid((int)ceil((float)image->Width() / (float)TILE_SIZE),(int)ceil((float)image->Height() / (float)TILE_SIZE));
	
	kernel_filtro<<<dimGrid, dimBlock>>>(deviceinputimage, deviceOutputImage, width, height);
	cudaEventRecord(stop);
	//Invocacion de la funcion kernel
	// Guardamos la salida en el host
	cudaMemcpy(outputImage->image, deviceOutputImage, size, cudaMemcpyDeviceToHost);

	//Liberar memoria
	cudaFree(deviceinputimage);
	cudaFree(deviceOutputImage);
	float time = 0;
	cudaEventElapsedTime(&time,start,stop);
	printf("Tiempo %f\n", 100.0*(time/1000)); 
	return true;
}

bool CMF_GPU( Bitmap* image, Bitmap* outputImage){

	cudaEvent_t start,stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start);
	int width = image->Width();
	int height = image->Height();
	int nthread=512;
	
	int size =  width * height * sizeof(char);
	
	int bkd=(nthread-(nthread % WINDOW_SIZE)) / WINDOW_SIZE;
	int grd=((size-(size % bkd))/bkd)+1;
	dim3 thdims(bkd, WINDOW_SIZE, 1);
	dim3 bkdims(grd, 1, 1);

	//Inicializar imagenes
	unsigned char *deviceinputimage;
	cudaMalloc((void**) &deviceinputimage, size);
	cudaMemcpy(deviceinputimage, image->image, size, cudaMemcpyHostToDevice);
	
	unsigned char *deviceOutputImage;
	cudaMalloc((void**) &deviceOutputImage, size);

	CMF_kernel<<<bkdims,thdims>>>(deviceinputimage, deviceOutputImage, 2);
	cudaEventRecord(stop);
	// Guardar imagen en host
	cudaMemcpy(outputImage->image, deviceOutputImage, size, cudaMemcpyDeviceToHost);
	
	//Liberar memoria
	cudaFree(deviceinputimage);
	cudaFree(deviceOutputImage);
	float time = 0;
	cudaEventElapsedTime(&time,start,stop);
	printf("Tiempo %f\n", 100.0*((time)/1000)); 
	return true;
}

