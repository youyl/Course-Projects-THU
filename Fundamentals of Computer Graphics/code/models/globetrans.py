import cv2
import numpy as np

img=cv2.imread('globe.png')
img2=cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

h,w,_=img2.shape

print (h,w)

print(str(img2[1,1,0])+'\n')

file=open('globe.txt','w')
file.write(str(h)+'\n');
file.write(str(w)+'\n');
for i in range(0,h):
	for j in range(0,w):
		for k in range(0,3):
			file.write(str(img2[i,j,k])+'\n');
file.close