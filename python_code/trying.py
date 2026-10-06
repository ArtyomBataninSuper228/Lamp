import serial


ser = serial.Serial('/dev/cu.usbmodem355B376734371', 2000000)
ser.write(b"6824")
while 1:
    print(ser.readline())