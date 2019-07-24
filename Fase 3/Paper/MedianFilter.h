#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include "Bitmap.h"
//#define WINDOW_SIZE (3)
//Define the size of the window that will be used for filtering.
#ifndef WINDOW_SIZE
#define WINDOW_SIZE (5)
#endif

//    //CPU Filtro de la mediana
    void CPU_filtro_mediana( Bitmap* image, Bitmap* outputImage );
//
//    //GPU Filtro de la mediana
    bool GPU_filtro_mediana( Bitmap* image, Bitmap* outputImage, bool sharedMemoryUse );
    bool CMF_GPU( Bitmap* image, Bitmap* outputImage );
//
    #endif
