import serial
from matplotlib import pyplot as plt
import json
import os
f = open("experiment_data/cool_data_2.json", "r")

data = json.load(f)
f.close()
plt.plot(data[2], data[1])
plt.plot(data[2], data[0])
##plt.show()
plt.close()
for i in os.listdir("experiment_data"):
    if not i.endswith(".json"):
        continue
    f = open(os.path.join("experiment_data", i), "r")
    data = json.load(f)
    f.close()
    plt.plot(data[2], data[1])
    plt.plot(data[2], data[0])
    plt.savefig(os.path.join("experiment_data", i[:-5]+ ".png"))
    plt.close()
