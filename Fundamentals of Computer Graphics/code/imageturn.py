import cv2
import numpy as np

img=cv2.imread('test.ppm')
cv2.imwrite('test.bmp',img)
cv2.imshow('testppm',img)
cv2.waitKey()
cv2.destroyAllWindows()