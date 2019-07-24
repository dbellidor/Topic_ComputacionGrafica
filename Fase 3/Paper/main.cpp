#include<stdio.h>
#include<iostream>
#include "MedianFilter.h"
#include "Bitmap.h"
#include <ctime>
#include <time.h>
const int window_size = WINDOW_SIZE;
#define ITERATIONS (2)

int CompareBitmaps( Bitmap* inputA, Bitmap* inputB ){
	int differentpixels = 0; //Inicializamos la variable de diferencia
	if((inputA->Height() != inputB->Height()) || (inputA->Width() != inputB->Width())) // Compruebe la condición de altura y anchura .
		return -1;
	for(int height=1; height<inputA->Height()-1; height++){
		for(int width=1; width<inputA->Width()-1; width++){
			if(inputA->GetPixel(width, height) != inputB->GetPixel(width, height))
				differentpixels++; // Incrementamos la diferencia
		}
	}
	return differentpixels;
}

void CPU_filtro_mediana( Bitmap* image, Bitmap* outputImage )
{
	int tam_vector=9;
	int media=tam_vector/2;
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	unsigned char filterVector[tam_vector] = {0,0,0,0,0,0,0,0,0}; //Longitud de la ventana
	for(int row=0; row<image->Height(); row++){
		for(int col=0; col<image->Width(); col++){
			if((row==0) || (col==0) || (row==image->Height()-1) || (col==image->Width()-1)) //Condiciones limite
				outputImage->SetPixel(col, row, 0);
			else {
				for (int x = 0; x < WINDOW_SIZE; x++) {
					for (int y = 0; y < WINDOW_SIZE; y++){
						filterVector[x*WINDOW_SIZE+y] = image->GetPixel( (col+y-1),(row+x-1));  // llenamos el vector de la mediana
					}
				}
				for (int i = 0; i < 9; i++) {
					for (int j = i + 1; j < tam_vector; j++) {
						if (filterVector[i] > filterVector[j]) {
							char tmp = filterVector[i];
							filterVector[i] = filterVector[j];
							filterVector[j] = tmp;
						}
					}
				}
				outputImage->SetPixel(col, row, filterVector[media]); //Guardamos el elemento de la mediana
			}
		}
	}
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Tiempo %lf\n", 100.0*(cpu_time_used)); 

}


int main()
{
	Bitmap* originalImage = new Bitmap();
	Bitmap* resultImageCPU = new Bitmap();
	Bitmap* resultImageGPU_paper = new Bitmap();
	Bitmap* resultImageGPU_normal = new Bitmap();

	float tcpu, tgpu, tgpushared; //Variables de tiempo
	clock_t start, end;

	const char* file="RIT.bmp";
	originalImage->Load(file);
	resultImageCPU->Load(file);
	resultImageGPU_paper->Load(file);
	resultImageGPU_normal->Load(file);
	printf("Con el kernel del paper \n"); 
	for (int i = 0; i < ITERATIONS; i++)
	{	

		CMF_GPU(originalImage, resultImageGPU_paper);
	}

	printf("Con el kernel normal \n"); 
	for (int i = 0; i < ITERATIONS; i++)
	{	
		GPU_filtro_mediana(originalImage, resultImageGPU_normal, false);
	}

	/*printf("Con CPU \n"); 
	for (int i = 0; i < ITERATIONS; i++)
	{	
		CPU_filtro_mediana(originalImage, resultImageCPU);
	}*/
	
	//resultImageCPU->Save("salida_cpu.bmp");
	//resultImageGPU_paper->Save("salida_gpu_paper.bmp");
	resultImageGPU_normal->Save("salida_gpu_normal.bmp");

}
