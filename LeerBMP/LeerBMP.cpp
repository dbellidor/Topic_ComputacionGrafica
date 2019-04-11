#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <string.h>
using namespace std;



typedef struct bmpFileHeader
{
    /* 2 bytes de identificación */
    unsigned int size;    /* Tamaño del archivo */
    unsigned short resv1; /* Reservado */
    unsigned short resv2; /* Reservado */
    unsigned int offset;  /* Offset hasta hasta los datos de imagen */
} bmpFileHeader;

typedef struct bmpInfoHeader
{
    unsigned int tam_cabezera;    /* Tamaño de la cabecera */
    unsigned int ancho;           /* Ancho */
    unsigned int alto;            /* Alto */
    unsigned short num_planos;    /* Planos de color (Siempre 1) */
    unsigned short bits_ppixel;   /* bits por pixel */
    unsigned int compresion;      /* compresión 0=no*/
    unsigned int tam_img;         /* tamaño de los datos de imagen */
    unsigned int res_x;           /* Resolución X en bits por metro */
    unsigned int res_y;           /* Resolución Y en bits por metro */
    unsigned int tam_tablaColor;  /* tam_tablaColor used en la paleta */
    unsigned int cont_col_Import; /* Colores importantes. 0 si son todos */
} bmpInfoHeader;

unsigned char *readBMP(string filename, bmpFileHeader *fheader, bmpInfoHeader *bInfoHeader)
{
    FILE *f;
    // bmpFileHeader header;   /* cabecera */
    unsigned char *imgdata; /* datos de imagen */
    unsigned short type;    /* 2 bytes identificativos */

    f = fopen(filename.c_str(), "rb");
    if (!f)
        return NULL; /* Si no podemos leer, no hay imagen*/

    /* Leemos los dos primeros bytes */
    fread(&type, sizeof(unsigned short), 1, f);
    if (type != 0x4D42) /* Comprobamos el formato BM*/
    {
        fclose(f);
        return NULL;
    }

    /* Leemos la cabecera de fichero completa */
    fread(fheader, sizeof(bmpFileHeader), 1, f);
    // fheader = &header;

    /* Leemos la cabecera de información completa */
    fread(bInfoHeader, sizeof(bmpInfoHeader), 1, f);

    /* Reservamos memoria para la imagen, ¿cuánta? Tanto como indique tam_img */
    imgdata = new unsigned char[bInfoHeader->tam_img];

    /* Nos situamos en el sitio donde empiezan los datos de imagen,
    nos lo indica el offset de la cabecera de fichero*/
    fseek(f, fheader->offset, SEEK_SET);

    /* Leemos los datos de imagen, tantos bytes como tam_img */
    fread(imgdata, bInfoHeader->tam_img, 1, f);

    fclose(f);

    return imgdata;
}

unsigned char *sumar(bmpInfoHeader info, unsigned char *img, int cuanto)
{
    int x, y;
    unsigned char r, g, b;
    unsigned char *edit;
    edit = new unsigned char[info.tam_img];
    
    /* Dibujamos la imagen */
    for (y = info.alto; y > 0; y--)
    {
        for (x = 0; x < info.ancho; x++)
        {
            b = (img[3 * (x + y * info.ancho)]);
            g = (img[3 * (x + y * info.ancho) + 1]);
            r = (img[3 * (x + y * info.ancho) + 2]);
            // cout << "B: " << int(b) << " G: " << int(g) << " R: " << int(r) << endl;
            edit[3 * (x + y * info.ancho)] = b + cuanto;
            edit[3 * (x + y * info.ancho) + 1] = g + cuanto;
            edit[3 * (x + y * info.ancho) + 2] = r + cuanto;

        }
    }
    return edit;
}
unsigned char *sumarg(bmpInfoHeader info, unsigned char *img, int cuanto)
{
    int x, y;
    unsigned char r, g, b;
    unsigned char *edit;
    edit = new unsigned char[info.tam_img];
    
    /* Dibujamos la imagen */
    for (y = info.alto; y > 0; y--)
    {
        for (x = 0; x < info.ancho; x++)
        {
            b = (img[3 * (x + y * info.ancho)]);
            g = (img[3 * (x + y * info.ancho) + 1]);
            r = (img[3 * (x + y * info.ancho) + 2]);
            // cout << "B: " << int(b) << " G: " << int(g) << " R: " << int(r) << endl;
            edit[3 * (x + y * info.ancho)] = b + cuanto;
            edit[3 * (x + y * info.ancho) + 1] = g + cuanto;
            edit[3 * (x + y * info.ancho) + 2] = r + cuanto;

        }
    }
    return edit;
}

void DisplayInfo(bmpInfoHeader *info)
{
    cout << "Tamaño de la cabecera: " << info->tam_cabezera << endl;
    cout << "Anchura: " << info->ancho << endl;
    cout << "Altura: " << info->alto << endl;
    cout << "Planos (1): " << info->num_planos << endl;
    cout << "Bits por pixel: " << info->bits_ppixel << endl;
    cout << "Compresión: " << info->compresion << endl;
    cout << "Tamaño de datos de imagen: " << info->tam_img << endl;
    cout << "Resolucón horizontal: " << info->res_x << endl;
    cout << "Resolucón vertical: " << info->res_y << endl;
    cout << "Colores en paleta: " << info->tam_tablaColor << endl;
    cout << "Colores importantes: " << info->cont_col_Import << endl;
}

void saveImg(string nom_saveBMP, bmpInfoHeader infoHead, bmpFileHeader fileHead, unsigned char *imgdata)
{
    // Guarda la imagen
    unsigned short type = 0x4D42;                     // BM
    FILE *outfile = fopen(nom_saveBMP.c_str(), "wb"); // wb -> w: writable b: binary, open as writable and binary
    if (outfile == NULL)
    {
        cout << "Archivo no encontrado ";
    }
    // Primero guardar la cabezera de identificacion BM, luego el filehead, luego el fileinfo, luego la data
    fwrite(&type, sizeof(unsigned short), 1, outfile);

    fwrite(&fileHead, sizeof(bmpFileHeader), 1, outfile);
    fwrite(&infoHead, sizeof(bmpInfoHeader), 1, outfile);
    fwrite(imgdata, infoHead.tam_img, 1, outfile);
    
    fclose(outfile);
}

int main(int argc, char *argv[])
{
    std::cout << "Lector de imagenes BMP" << '\n';

    if (argc != 2)
    {
        std::cout << "Run: ./exe img.bmp" << '\n';
        return 0;
    }
    else
    {
        bmpFileHeader fileHead;
        bmpInfoHeader infoHead;
        unsigned char *img;
        unsigned char *img_edit;
        img = readBMP(argv[1], &fileHead, &infoHead);
         DisplayInfo(&infoHead);
        img_edit = sumar(infoHead, img, 1);
        // saveImg("test_img2.bmp", infoHead, fileHead, img);
        saveImg("editada.bmp", infoHead, fileHead, img_edit);
        // printf("DATA %d", img);
        // std::cout << "DATA" << img << '\n';
        return 0;
    }
}
