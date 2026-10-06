import serial
from matplotlib import pyplot as plt
import json

f = open("experiment_data/fire_1.json", "r")
data = json.load(f)
f.close()
plt.plot(data[2], data[1])
plt.plot(data[2], data[0])
plt.show()