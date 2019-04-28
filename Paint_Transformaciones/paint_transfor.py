from tkinter import *              
from tkinter import ttk

import tkFileDialog
from PIL import Image, ImageTk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib import pyplot as plt
from matplotlib.figure import Figure
import cv2
import numpy as np
nombre_img="" 
img_tk=None
img_cv=None
from ope_img import *
class SampleApp(Tk):
    
    
    def __init__(self, *args, **kwargs):
        Tk.__init__(self, *args, **kwargs)

        # the container is where we'll stack a bunch of frames
        # on top of each other, then the one we want visible
        # will be raised above the others
        container = Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}
        for F in (ventana_principal, ventana_tran_lineales):
            page_name = F.__name__
            frame = F(parent=container, controller=self)
            self.frames[page_name] = frame

            # put all of the pages in the same location;
            # the one on the top of the stacking order
            # will be the one that is visible.
            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame("ventana_principal")

    def show_frame(self, page_name):
        '''Show a frame for the given page name'''
	global nombre_img
        frame = self.frames[page_name]
	
	if page_name == "ventana_tran_lineales":
		#print("Desde ven_principal " + nombre_img)
		if len(nombre_img) > 0:
			frame.tkraise()
			global img_tk
			label = Label (frame, image = img_tk)
			label.place(x=30, y=200)
			frame.mainloop()
			
	else:
		
		frame.tkraise()


class ventana_principal(Frame):
    
    def __init__(self, parent, controller):
        Frame.__init__(self, parent)
        self.controller = controller
        controller.title("Transformaciones en imagenes")
	controller.geometry("950x700")
	#Anadir combobox
	combo_tipos_ope = ttk.Combobox(self)
	combo_tipos_ope.place(x=300,y=100)
	combo_tipos_ope['values']=('Transfor Lineales','Transfor Polinomicas')
	combo_tipos_ope.current(0)
	#Boton para cargar la imagen
	boton_cargar_img=Button(self,text="Cargar Imagen",command=self.cargar_img).place(x=100,y=100)
	#boton_cargar_img.pack()
        ingresar_img = Button(self, text="Iniciar",command=lambda: controller.show_frame("ventana_tran_lineales")).place(x=500,y=200)
        #ingresar_img.pack()

    def cargar_img(self):
	global nombre_img
	nombre_img=tkFileDialog.askopenfilename(initialdir = "/home/diego/Documentos/Top Grafica",title = "Select file",filetypes = (("bmp files","*.bmp"),("all files","*.*")))
	global img_tk
	global img_cv
	img_tk = ImageTk.PhotoImage(Image.open (nombre_img).resize((330, 330)))
	img_cv=cv2.imread(nombre_img)
	img_cv = cv2.resize(img_cv, (330,330))
	img_cv = cv2.cvtColor(img_cv, cv2.COLOR_BGR2RGBA)
	label = Label (self, image = img_tk)
	label.place(x=60, y=150)
	self.mainloop()

class ventana_tran_lineales(Frame):
    
    def __init__(self, parent, controller):
        Frame.__init__(self, parent)
        self.controller = controller
	
	#tran_lineales=ttk.Notebook(self)
	#tran_lineales.pack(fill='both',expand='yes')
	
	self.var_suma=IntVar()
	self.var_mul=DoubleVar()
	self.var_mul.set(1)
	self.var_div=DoubleVar()
	self.var_div.set(0.2)
	self.label_img=Label(self, image ='')

	boton_sum=Button(self,text="Suma - Resta",command=self.sum).place(x=50,y=20)
	boton_mult=Button(self,text="Multiplicacion",command=self.mult).place(x=50,y=70)
	boton_div=Button(self,text="Division",command=self.div_).place(x=350,y=70)
	boton_equali=Button(self,text="Equalizacion",command=self.equali).place(x=350,y=20)
	boton_gamma=Button(self,text="Gamma",command=self.gamma).place(x=600,y=20)
	boton_gris=Button(self,text="Gris",command=self.gris).place(x=500,y=20)
	
	self.bar_suma=Scale(self, from_=-50, to=50,orient=HORIZONTAL,variable=self.var_suma).place(x=200,y=8)
	self.bar_multi=Scale(self, from_=-2, to=2,resolution=1,orient=HORIZONTAL,variable=self.var_mul).place(x=200,y=60)
	
	self.bar_div=Scale(self, from_=-2.5, to=2.5,resolution=0.2,orient=HORIZONTAL,variable=self.var_div).place(x=500,y=60)
	self.button = Button(self, text="Regresar al inicio",command=lambda: controller.show_frame("ventana_principal")).place(x=300,y=650)
        
	
	

    def sum(self):
	global img_cv
	s=self.var_suma.get()
	#temp2=img_cv
	img_cv=img_sum(img_cv,s)
	ims = Image.fromarray(img_cv)
	imgs = ImageTk.PhotoImage(image=ims)

	figure = Figure(figsize=(5.4, 4), dpi=100)
	figure.clear()
	plot = figure.add_subplot(1, 1, 1)
	color = ('r','g','b')
	for i, c in enumerate(color):
	    hist_s = cv2.calcHist([img_cv], [i], None, [256], [0, 256])
	    plot.plot(hist_s, color=c )

        plot.set_xlabel('Intensidad de iluminacion')
	plot.set_ylabel('Cantidad de pixeles')
	plot.set_xlim([0,256])

	canvas = FigureCanvasTkAgg(figure,self)
	canvas.get_tk_widget().place(x=400, y=200)
	
	self.label_img=Label(self,image = imgs)
	self.label_img.place(x=30, y=200)
	self.label_img=imgs

	
    def mult(self):
	global img_cv
	s=self.var_mul.get()
	img_cv=img_multi(img_cv,s)
	ims = Image.fromarray(img_cv)
	imgs = ImageTk.PhotoImage(image=ims)
	
	figure = Figure(figsize=(5.4, 4), dpi=100)
	figure.clear()
	plot = figure.add_subplot(1, 1, 1)
	color = ('r','g','b')
	for i, c in enumerate(color):
	    hist_s = cv2.calcHist([img_cv], [i], None, [256], [0, 256])
	    plot.plot(hist_s, color=c )

        plot.set_xlabel('Intensidad de iluminacion')
	plot.set_ylabel('Cantidad de pixeles')
	plot.set_xlim([0,256])

	canvas = FigureCanvasTkAgg(figure,self)
	canvas.get_tk_widget().place(x=400, y=200)
	
	self.label_img=Label(self,image = imgs)
	self.label_img.place(x=30, y=200)
	self.label_img=imgs

    def div_(self):
	global img_cv
	s=self.var_div.get()
	img_cv=img_div(img_cv,s)
	ims = Image.fromarray(img_cv)
	imgs = ImageTk.PhotoImage(image=ims)
	
	figure = Figure(figsize=(5.4, 4), dpi=100)
	figure.clear()
	plot = figure.add_subplot(1, 1, 1)
	color = ('r','g','b')
	for i, c in enumerate(color):
	    hist_s = cv2.calcHist([img_cv], [i], None, [256], [0, 256])
	    plot.plot(hist_s, color=c )

        plot.set_xlabel('Intensidad de iluminacion')
	plot.set_ylabel('Cantidad de pixeles')
	plot.set_xlim([0,256])

	canvas = FigureCanvasTkAgg(figure,self)
	canvas.get_tk_widget().place(x=400, y=200)
	
	self.label_img=Label(self,image = imgs)
	self.label_img.place(x=30, y=200)
	self.label_img=imgs

    def equali(self):
	global img_cv
	img_cv=equali(img_cv)

	"""
	img_cv = cv2.imread(nombre_img)
	b, g, r = cv2.split(img_cv)
	print img_cv[2]
	red = cv2.equalizeHist(r)
	green = cv2.equalizeHist(g)
	blue = cv2.equalizeHist(b)
	img_cv=cv2.merge((blue, green, red))
        """
	
		
	figure = Figure(figsize=(5.4, 4), dpi=100)
	figure.clear()
	plot = figure.add_subplot(1, 1, 1)
	if (img_cv[0][0][0]==img_cv[0][0][1]==img_cv[0][0][2]):
		hist_s = cv2.calcHist([img_cv], [0], None, [256], [0, 256])
		plot.plot(hist_s, color='gray' )
	else:
		color = ('b','g','r')
		for i, c in enumerate(color):
	    		hist_s = cv2.calcHist([img_cv], [i], None, [256], [0, 256])
	    		plot.plot(hist_s, color=c )

        plot.set_xlabel('Intensidad de iluminacion')
	plot.set_ylabel('Cantidad de pixeles')
	plot.set_xlim([0,256])

	canvas = FigureCanvasTkAgg(figure,self)
	canvas.get_tk_widget().place(x=400, y=200)

	img_cv = cv2.resize(img_cv, (330,330))
	img_cv = cv2.cvtColor(img_cv, cv2.COLOR_BGR2RGBA)
	ims = Image.fromarray(img_cv)
	imgs = ImageTk.PhotoImage(image=ims)

	self.label_img=Label(self,image = imgs)
	self.label_img.place(x=30, y=200)
	self.label_img=imgs



    def gamma(self):
	global img_cv
	img_cv=img_gamma(img_cv,1)
	ims = Image.fromarray(img_cv)
	imgs = ImageTk.PhotoImage(image=ims)

	figure = Figure(figsize=(5.4, 4), dpi=100)
	figure.clear()
	plot = figure.add_subplot(1, 1, 1)
	color = ('r','g','b')
	for i, c in enumerate(color):
	    hist_s = cv2.calcHist([img_cv], [i], None, [256], [0, 256])
	    plot.plot(hist_s, color=c )

        plot.set_xlabel('Intensidad de iluminacion')
	plot.set_ylabel('Cantidad de pixeles')
	plot.set_xlim([0,256])

	canvas = FigureCanvasTkAgg(figure,self)
	canvas.get_tk_widget().place(x=400, y=200)
	
	self.label_img=Label(self,image = imgs)
	self.label_img.place(x=30, y=200)
	self.label_img=imgs
	
    def gris(self):
	global img_cv
	img_cv=img_gris(img_cv)
	ims = Image.fromarray(img_cv)
	imgs = ImageTk.PhotoImage(image=ims)

	hist_s = cv2.calcHist([img_cv], [0], None, [256], [0, 256])
	figure = Figure(figsize=(5.4, 4), dpi=100)
	figure.clear()
	plot = figure.add_subplot(1, 1, 1)
	plot.plot(hist_s, color='gray' )

	plot.set_xlabel('Intensidad de iluminacion')
	plot.set_ylabel('Cantidad de pixeles')
	plot.set_xlim([0,256])

	canvas = FigureCanvasTkAgg(figure,self)
	canvas.get_tk_widget().place(x=400, y=200)

	self.label_img=Label(self,image = imgs)
	self.label_img.place(x=30, y=200)
	self.label_img=imgs
	



if __name__ == "__main__":
    app= SampleApp()
    app.mainloop()
