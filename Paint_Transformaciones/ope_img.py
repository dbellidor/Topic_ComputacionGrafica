import cv2
import numpy as np
from math import pow
def img_sum(img,inc):
	alto,ancho,bpp = np.shape(img)
	for px in range(0,alto):
		for py in range(0,ancho):
			r=img[px][py][0]
			g=img[px][py][1]
			b=img[px][py][2]
			if (r + inc) > 255:
				img[px][py][0]=255
			else:
				img[px][py][0]=r+inc
			if (g + inc) > 255:
				img[px][py][1]=255
			else:
				img[px][py][1]=g+inc
			if (b + inc) > 255:
				img[px][py][2]=255
			else:
				img[px][py][2]=b+inc
	return img

def img_multi(img,inc):
	alto,ancho,bpp = np.shape(img)
	for px in range(0,alto):
		for py in range(0,ancho):
			r=img[px][py][0]
			g=img[px][py][1]
			b=img[px][py][2]
			img[px][py][0]=r*inc
			img[px][py][1]=g*inc
			img[px][py][2]=b*inc
	return img

def img_div(img,inc):
	alto,ancho,bpp = np.shape(img)
	for px in range(0,alto):
		for py in range(0,ancho):
			r=img[px][py][0]
			g=img[px][py][1]
			b=img[px][py][2]
			img[px][py][0]=r/inc
			img[px][py][1]=g/inc
			img[px][py][2]=b/inc
	return img

def img_gris(img):
	alto,ancho,bpp = np.shape(img)
	for px in range(0,alto):
		for py in range(0,ancho):
			r=img[px][py][0]
			g=img[px][py][1]
			b=img[px][py][2]
			gris=0.2989 * r + 0.5870 * g + 0.1140 * b
			img[px][py][0]=img[px][py][1]=img[px][py][2]=gris
	return img

def img_gamma(img,gamma):
	invgamma=1.0 / gamma;
	alto,ancho,bpp = np.shape(img)
	for px in range(0,alto):
		for py in range(0,ancho):
			r=img[px][py][0]
			g=img[px][py][1]
			b=img[px][py][2]
			img[px][py][0]=((r / 255.0) ** invgamma) * 255
			img[px][py][1]=((g / 255.0) ** invgamma) * 255
			img[px][py][2]=((b / 255.0) ** invgamma) * 255
	return img
def equali(img):
	img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
	b, g, r = cv2.split(img)
	red = cv2.equalizeHist(r)
	green = cv2.equalizeHist(g)
	blue = cv2.equalizeHist(b)
	img=cv2.merge((blue, green, red))
	return img		
