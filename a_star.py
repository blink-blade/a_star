import os
from ctypes import CDLL

os.system("gcc -shared -Wl,-soname,adder -o a_star.so -fPIC a_star.c")

a_star = CDLL('./a_star.so')

