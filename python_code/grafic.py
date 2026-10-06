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
    if(max(data[2]) >= 10):
        for j in range(len(data[2])):
            data[2][j] = data[2][j]/5000
    f.close()
    plt.style.use('seaborn-v0_8-paper')
    plt.figure(figsize=(32, 18))
    plt.errorbar(data[2], data[1], yerr=0.797, fmt=".")
    plt.errorbar(data[2], data[0], yerr=0.497, fmt=".")
    plt.xlabel("Время с")
    plt.ylabel("Измеренное значение")
    plt.savefig(os.path.join("experiment_data", "errors_"+i[:-5]+ ".png"), dpi = 300)
    plt.close()

    plt.style.use('seaborn-v0_8-paper')
    plt.figure(figsize=(20, 12))
    plt.plot(data[2], data[1])
    plt.plot(data[2], data[0])
    plt.xlabel("Время с")
    plt.ylabel("Измеренное значение")
    plt.savefig(os.path.join("experiment_data",   i[:-5] + ".png"), dpi=300)
    plt.close()
