from math import *
import numpy as np
import json


f= open("experiment_data/under_table_noise.json","r")
data = json.load(f)
f.close()

I_sr = sum(data[0])/len(data[0])
L_sr = sum(data[1])/len(data[1])
sigma_i = 0
sigma_l = 0
for i in range(0,len(data[0])):
    sigma_i += (data[0][i] - I_sr)**2/len(data[0])
    sigma_l += (data[1][i] - L_sr)**2/len(data[1])
sigma_i = sigma_i**0.5
sigma_l = sigma_l**0.5
print(sigma_i)#0.497
print(sigma_l)#0.797



