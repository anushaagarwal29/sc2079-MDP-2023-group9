#!/bin/bash
'''
    Python script to setup a server to receive commands from PC 
    and send over serial to STM32F.
'''
import socket
import serial
from time import sleep


def send_command(data, ser):
    ser.write(f'{data}'.encode('utf-8'))
    #print(ser.readline().decode('utf-8'))
    #cc = str(ser.readline())
    #print(cc[2:][:-5])
    print('done')


def main():
    HOST = "0.0.0.0" # listen for connection on any interface
    PORT = 4444 # listen on port 4444
    ser = serial.Serial("/dev/ttyUSB0", 115200)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        while True:
            s.listen()
            print("Listening for connection on port 4444")
            conn, addr = s.accept() # PC initiates connection to RPi
            with conn:
                try:
                    print(f"Connected by {addr}")
                    while True:
                        data = conn.recv(1024)
                        print(data.decode('utf-8'))
                        if not data:
                            print("Connection closed by client")
                            break
                        send_command(data, ser)
                        conn.send('Data transfer successful'.encode('utf-8'))
                except IOError:
                    pass
    ser.close()


if __name__ == '__main__':
    main()

