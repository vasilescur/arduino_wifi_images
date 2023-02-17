import serial

ser = serial.Serial('/dev/tty.usbmodem2102', 9600)

# image = ser.read_until('JPEGEND'.encode('ASCII'))
image = ser.read_until('$$$$$$$$$$'.encode('ASCII'))
image = image[:-10]

# print(image)


with open('./image.jpeg', 'wb') as file:
    file.write(image)
