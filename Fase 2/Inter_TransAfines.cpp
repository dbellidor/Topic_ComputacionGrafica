/*
		g++ Inter_TransAfines.cpp -o salida `pkg-config --cflags --libs opencv`
		./salida

*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdint.h>
#include <climits>
#include <math.h>

//Valor de PI
#define PI 3.14159265

using namespace cv;
using namespace std;

//	Cambiar el tamaño de una imagen usando  nearestNieghbour

void nearestNeighbour(Mat& a){
	int rows, cols;
	cout << "Filas iniciales "<<a.rows << " Columnas iniciales " << a.cols<<endl;
	cout << "Ingrese el numero de rows y cols finales : ";
	cin>>rows>>cols;
	Mat x(rows,cols,CV_8UC3,Scalar(0,0,0));
	for(int i=0;i<3;i++){
		for(int r=0;r<rows;r++){
			for(int c=0;c<cols; c++){ 	
				x.at<Vec3b>(r,c)[i] = a.at<Vec3b>((r)*a.rows/rows,(c)*a.cols/cols )[i];
			}
		}
	}
	Mat ix;
	resize(a,ix,Size(x.cols,x.rows),0,0,0);
	namedWindow("Salida",WINDOW_AUTOSIZE);
	namedWindow("Entrada",WINDOW_AUTOSIZE);
	imshow("Salida",x);
	imshow("Entrada", a);
	moveWindow("Entrada", 200, 400);
	moveWindow("Salida", 200+ a.cols + 10, 400);
	
}

//	Function to rotate an image with the degree user enter in console.

void rotacion(Mat a){
	int deg;
	cout << "Ingrese el grado en el que desea rotar una imagen: ";
	cin>>deg;
	if (deg < 0){
		deg = 360 + deg;
	}
	float width, height;
	if ((deg <90 && deg>=0) || (deg < 270 && deg >=180)){
		height = a.cols*abs(cos(deg*PI/180)) + a.rows*abs(sin(deg*PI/180));
		width = a.rows*abs(sin(deg*PI/180)) + a.cols*abs(cos(deg*PI/180));
	}else if(deg <180 && deg>=90 || (deg < 360 && deg >=270)){
		height = a.rows*abs(cos(deg*PI/180)) + a.cols*abs(sin(deg*PI/180));
		width = a.cols*abs(sin(deg*PI/180)) + a.rows*abs(cos(deg*PI/180));
	}
	Mat x(height,width,CV_8UC3,Scalar(0,0,0));
	float m[3][3] = {0};
	m[0][0] = cos(deg*PI/180);
	m[0][1] = sin(deg*PI/180);
	m[1][0] = -sin(deg*PI/180);
	m[1][1] = cos(deg*PI/180);
	m[2][2] = 1;
	Mat aClone = x.clone();
	for(int i=0;i<3;i++){
		for(int r=(aClone.rows-a.rows)/2;r<a.rows+int((aClone.rows-a.rows)/2);r++){
			for(int c=int((aClone.cols-a.cols)/2);c<a.cols+int((aClone.cols-a.cols)/2); c++){
				aClone.at<Vec3b>(r,c)[i] = a.at<Vec3b>(r-int((aClone.rows-a.rows)/2),c - int((aClone.cols-a.cols)/2))[i];			
			}
		}
	}
	for(int i=0;i<3;i++){
		for(int r=0;r<x.rows;r++){
			for(int c=0;c<x.cols; c++){
				float x1 = m[0][0]*(r-x.rows/2) + m[0][1]*(c-x.cols/2);
				float y1 = m[1][0]*(r-x.rows/2) + m[1][1]*(c-x.cols/2);
				if (x1+x.rows/2<0 || x1+x.rows/2>=aClone.rows || y1+x.cols/2<0 || y1+x.cols/2>aClone.cols){
					x.at<Vec3b>(r,c)[i] = 0; 
				}else {
					x.at<Vec3b>(r,c)[i] = aClone.at<Vec3b>(x1+x.rows/2,y1+x.cols/2)[i];
				}			
			}
		}
	}
	namedWindow("Salida",WINDOW_AUTOSIZE);
	namedWindow("Entrada",WINDOW_AUTOSIZE);
	imshow("Salida",x);
	imshow("Entrada", a);
	moveWindow("Entrada", 200, 400);
	moveWindow("Salida", 200+ a.cols + 10, 400);
}


//	Funcion para realizar la  perspectiva en imagen.

void perspectiva(Mat a){
	float xpers,ypers;
	cout << "Enter el factor de perspectiva  (0-1) en la dirección horizontal vertical : ";
	cin>>xpers>>ypers;
	if (xpers<0 ||xpers>1 || ypers<0 || ypers>1){
		cout <<"Ingrese un numero valido !!!"<<endl;
		return;
	}
	Mat x(a.rows + a.cols*ypers,a.cols + a.rows*xpers,CV_8UC3,Scalar(0,0,0));
	float m[3][3] = {0};
	m[0][0] = 1/(1-xpers*ypers);
	m[0][1] = -ypers/(1-xpers*ypers);
	m[1][0] = -xpers/(1-xpers*ypers);
	m[1][1] = 1/(1-xpers*ypers);
	m[2][2] = 1;

	for(int i=0;i<3;i++){
		for(int r=0;r<x.rows;r++){
			for(int c=0;c<x.cols; c++){
				float x1 = m[0][0]*(r) + m[0][1]*(c);
				float y1 = m[1][0]*(r) + m[1][1]*(c);
				if (x1<0 || x1>=a.rows || y1<0 || y1>a.cols){
					x.at<Vec3b>(r,c)[i] = 0;
				}else {
					x.at<Vec3b>(r,c)[i] = a.at<Vec3b>(x1,y1)[i];
				}			
			}
		}
	}
	namedWindow("Salida",WINDOW_AUTOSIZE);
	namedWindow("Entrada",WINDOW_AUTOSIZE);
	imshow("Salida",x);
	imshow("Entrada", a);
	moveWindow("Entrada", 200, 400);
	moveWindow("Salida", 200+ a.cols + 10, 400);
}

// Funcion para realizar la traslacion de las imagenes en direccion horizontal y vertical.

void traslacion(Mat a){
	int xtras,ytras;
	cout <<"Ingrese la direccion horizontal y vertical para trasladar la imagen.: ";
	cin>>ytras>>xtras;
	Mat x(a.rows,a.cols,CV_8UC3,Scalar(0,0,0));

	for(int i=0;i<3;i++){
		for(int r=0;r<a.rows;r++){
			for(int c=0;c<a.cols; c++){
				int x1 = r-xtras;
				int y1 = c-ytras ;
				
				if (x1<0 || x1>=a.rows || y1<0 || y1>a.cols){
					x.at<Vec3b>(r,c)[i] = 0;
				}else {
					x.at<Vec3b>(r,c)[i] = a.at<Vec3b>(x1,y1)[i];
				}
			}
		}
	}
	namedWindow("Salida",WINDOW_AUTOSIZE);
	namedWindow("Entrada",WINDOW_AUTOSIZE);
	imshow("Salida",x);
	imshow("Entrada", a);
	moveWindow("Entrada", 200, 400);
	moveWindow("Salida", 200+ a.cols + 10, 400);
}

//	Funcion para realizar escalado en direccion horizontal y vertical.

void escalar(Mat a){
	float xescalar,yescalar;
	cout << "Ingrese el escalar en direccion horizontal y vertical (mayor que 0): ";
	cin>>xescalar>>yescalar;
	Mat x(int(a.rows*yescalar),int(a.cols*xescalar),CV_8UC3,Scalar(0,0,0));
	for(int i=0;i<3;i++){
		for(int r=0;r<x.rows;r++){
			for(int c=0;c<x.cols; c++){
				float x1 = r/yescalar;
				float y1 = c/xescalar;
				
				if (x1<0 || x1>=a.rows || y1<0 || y1>a.cols){
					x.at<Vec3b>(r,c)[i] = 0;
				}else {
					x.at<Vec3b>(r,c)[i] = a.at<Vec3b>(x1,y1)[i];
				}
			}
		}
	}
	namedWindow("Salida",WINDOW_AUTOSIZE);
	namedWindow("Entrada",WINDOW_AUTOSIZE);
	imshow("Salida",x);
	imshow("Entrada", a);
	moveWindow("Entrada", 200, 400);
	moveWindow("Salida", 200+ a.cols + 10, 400);
}


//	Funcion para realizar la interpolacion bilineal en la imagen para redimensionarla a una nueva dimension.


void interpolacionbilineal(Mat a){
	int rows, cols;
	cout << "Filas iniciales "<<a.rows << " Columnas iniciales " << a.cols<<endl;
	cout << "Ingrese el numero de rows y cols finales: ";
	cin>>rows>>cols;
	Mat x(rows,cols,CV_8UC3,Scalar(0,0,0));
	int count = 0;
	for(int i=0;i<3;i++){
		for(int r=0;r<rows;r++){
			for(int c=0;c<cols; c++){
				//cout << ++count << " row: "<<r << " cols " <<c<< endl;
				float x1 = a.cols*c/cols + a.cols/(2*cols);
				float y1 = a.rows*r/rows + a.rows/(2*rows);
				float n1,n2,n3,n4;
				if (((int)x1 + 0.5 - x1) > 0){
					n2 = (int)x1 + 0.5;
					n1 = (int)x1 - 0.5;
				}else{
					n1 = (int)x1 + 0.5;
					n2 = (int)x1 + 1.5;
				}
				if (((int)y1 + 0.5 - y1) > 0){
					n4 = (int)y1 + 0.5;
					n3 = (int)y1 - 0.5;
				}else{
					n3 = (int)y1 + 0.5;
					n4 = (int)y1 + 1.5;
				}
				float alpha = x1-n1;
				float beta = y1-n3;
				if (n1<0.5) {n1=0.5;}
				if (n3<0.5) n3=0.5;
				if (n2>a.cols-0.5) n2=a.cols-0.5;
				if (n4>a.rows-0.5) n4=a.rows-0.5;
				x.at<Vec3b>(r,c)[i] = alpha*beta*a.at<Vec3b>(int(n4), int(n2))[i] + alpha*(1-beta)*a.at<Vec3b>(int(n3), int(n2))[i] + beta*(1-alpha)*a.at<Vec3b>(int(n4), int(n1))[i] + (1-alpha)*(1-beta)*a.at<Vec3b>(int(n3), int(n1))[i];
			}
		}
	}
	Mat ix;
	resize(a,ix,Size(x.cols,x.rows),0,0,0);
	namedWindow("Salida",WINDOW_AUTOSIZE);
	namedWindow("Entrada",WINDOW_AUTOSIZE);
	imshow("Salida",x);
	imshow("Entrada", a);
	moveWindow("Entrada", 200, 400);
	moveWindow("Salida", 200+ a.cols + 10, 400);
}

int main( int argc, char** argv ) {
	cout<<"Ingrese la operacion a ejecutar\n"<<endl;
	cout<<"1 Redimensionar una imagen"<<endl;
	cout<<"2 Traslacion"<<endl;
	cout<<"3 Rotar una imagen"<<endl;
	cout<<"4 Escalar una imagen"<<endl;
	cout<<"5 Perspectiva "<<endl;
	cout<<"0 para salir"<<endl;
	int n;
	cin>>n;
	if (n==1) {
		cout<<"Ingrese el nombre de la imagen: ";
		string nombre;
		cin>>nombre;
		Mat a = imread(nombre);
		int flag=1;
		cout<<"1 para la transformacion por nearest neighbour y 2 para la interpolacion Bilineal : ";
		cin>>flag;
		if (flag==1)  {
			nearestNeighbour(a);
		}
		else  {
			interpolacionbilineal(a);
		}
	}
	else if (n==2) {
		cout<<"Ingrese el nombre de la imagen: ";
		string nombre;
		cin>>nombre;
		Mat a = imread(nombre);
		traslacion(a);
	}
	else if (n==3) {
		cout<<"Ingrese el nombre de la imagen: ";
		string nombre;
		cin>>nombre;
		Mat a = imread(nombre);
		rotacion(a);
	}
	else if (n==4) {
		cout<<"Ingrese el nombre de la imagen: ";
		string nombre;
		cin>>nombre;
		Mat a = imread(nombre);
		escalar(a);
	}
	else if (n==5) {
		cout<<"Ingrese el nombre de la imagen: ";
		string nombre;
		cin>>nombre;
		Mat a = imread(nombre);
		perspectiva(a);
	}
	else  {
	  cout<<"Ingrese un numero valido\n"<<endl;
	}
	waitKey();
  return 0;
}
 
