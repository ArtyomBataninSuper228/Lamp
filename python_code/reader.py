import json
import os.path

import serial
import serial.tools.list_ports
import math
from threading import Thread
import time
from matplotlib import pyplot as plt
import dearpygui.dearpygui as dpg
ports = serial.tools.list_ports.comports()

doing_exp = [False]
Active_Data = [[], [], []]
print("Доступные порты:")
for port in ports:
  # Выводим имя порта, описание и аппаратный ID
  print(f"{port.device}: {port.description} [{port.hwid}]")
try:
    ser = serial.Serial('/dev/cu.usbmodem355B376734371', 2000000)
except:
    print("Serial port not found")
    ser = None


def do_exp_1():
    global doing_exp, Active_Data
    doing_exp[0] = True
    time.sleep(1)
    ser.write(b"6824")
    I = []
    L = []
    p = []
    for j in range(10000):
        i, l = map(int, ser.readline().split())
        I.append(i)
        L.append(l)
        p.append(j)
    doing_exp[0] = False
    Active_Data= [L, I, p]
    dpg.set_value('Light_tag', [p, I])
    dpg.set_value('I_tag', [p, L])
    dpg.set_axis_limits("x_axis", 0, 10000)
    dpg.set_axis_limits("y_axis", 0, max(max(I), max(L)))
def get_value():
    while doing_exp[0]:
        time.sleep(0.1)
        print("Waiting")
    ser.write(b"6826")

    return map(int, ser.readline().split())

class voltmeter:
    def __init__(self, x = 900, y = 250, r = 150, color1 = (255, 255, 255), color2 = (25, 255, 255), label = "Voltmeter"):
        self.angle = 0
        self.x = x
        self.y = y
        self.r = r
        y = self.y + self.r * math.sin((self.angle - 180) / 180 * math.pi)
        x = self.x + self.r * math.cos((self.angle - 180) / 180 * math.pi)
        self.line = dpg.draw_line((self.x, self.y), (int(x), int(y)))
        for angle in range(0, 181, 18):
            val = round((angle) /180 * 3.3, 2)

            x1 = self.x + self.r * math.cos((angle - 180) / 180 * math.pi)
            x2 = self.x + self.r * 1.4 * math.cos((angle - 180) / 180 * math.pi)
            y1 = self.y + self.r * math.sin((angle-180) * math.pi / 180)
            y2 = self.y + self.r * 1.4 * math.sin((angle-180) * math.pi / 180)
            dpg.draw_line((x1, y1), (x2, y2))
            dpg.add_text(str(val), pos=(x2, y2), color=color2)

        for angle in range(0, 181, 9):
            x1 = self.x + self.r * math.cos((angle - 180) / 180 * math.pi)
            x2 = self.x + self.r*1.2 * math.cos((angle - 180) / 180 * math.pi)
            y1 = self.y + self.r * math.sin((angle-180) * math.pi / 180)
            y2 = self.y + self.r * 1.2 * math.sin((angle-180) * math.pi / 180)
            dpg.draw_line((x1, y1), (x2, y2))
        dpg.add_text(label, pos=(self.x - self.r, self.y+0.3*self.r), color=color2)


    def update(self, angle):
        self.angle = angle
        y = self.y + self.r * math.sin(( self.angle - 180 ) / 180* math.pi)
        x = self.x + self.r * math.cos(( self.angle -180) / 180* math.pi)

        dpg.configure_item(self.line, p1=(x, y), p2 = (self.x, self.y))


dpg.create_context()
with dpg.font_registry():
    with dpg.font('/Users/artembatanin/PycharmProjects/messenger/notomono-regular.ttf', 25, default_font=True, id="Default25"):
        dpg.add_font_range_hint(dpg.mvFontRangeHint_Cyrillic)
dpg.bind_font("Default25")

dpg.create_viewport(title='Lamp', width=1500, height=900)
def update(v1, v2):
    while 1:
        a,b = get_value()
        v1.update(a/1000*180)
        v2.update(b /1000*180)
def save():
    name = dpg.get_value("filename")
    f = open(os.path.join("experiment_data",name), "w")
    json.dump(Active_Data, f)
    f.close()

with dpg.window(label="Lamp", width=1500, height=900, no_move=True, no_close=True, no_resize=True, no_collapse=True):
    V1 = voltmeter(label = "Датчик освещённости")
    V2 = voltmeter(x= 250, y = 250, label = "Датчик тока")
    t=Thread(target=update, args=(V1,V2))
    t.start()
    with dpg.group(horizontal=True, pos = (50, 350)):
        dpg.add_button(label="Start Experiment", callback=do_exp_1)
        dpg.add_button(label="Save_data", callback=save)
        dpg.add_input_text(hint="Введите имя файла", tag = "filename")
    with dpg.plot(label="Line Series", height=500, width=1200, pos = (50, 400)):
        # optionally create legend
        dpg.add_plot_legend(location=100)

        # REQUIRED: create x and y axes
        dpg.add_plot_axis(dpg.mvXAxis, label="Новмер выборки", tag = "x_axis")
        dpg.add_plot_axis(dpg.mvYAxis, label="Измеренное значение", tag="y_axis")

        # series belong to a y axis
        dpg.add_line_series(Active_Data[2], Active_Data[0], label="Освещённость", parent="y_axis", tag="Light_tag")
        dpg.add_line_series(Active_Data[2], Active_Data[1], label="Сила тока", parent="y_axis",tag="I_tag")








dpg.setup_dearpygui()
dpg.show_viewport()

dpg.start_dearpygui()
is_run = False
dpg.destroy_context()
