#!/usr/bin
'''
    Before running this script, execute the command "sudo hciconfig hci0 piscan" to advertise
    the bluetooth server.
    Finally, execute this script with "sudo python3 bluetooth-test.py"
'''
from bluetooth import *
import socket
import os
import select
import sys
import serial
from time import sleep
from ImageRec.imageRec import Snapper
from hashtable import HashTable
import threading
import time

# called when we want to send data to stm32 and also expect a response
def send_receive_command_to_stm32(data, ser):
    ser.write(data)
    reply = ser.readline()
    print('done')
    return reply

def obstacle_two_reached(ser):
    while True:
        block = ser.readline().decode('utf-8')
        if block != '':
            break
    return

# called for challenge 2 to tell stm32 to start, and wait for stm to stop before 1st and 2nd block
def fastest_car_handler(ser, client_sock):
    # try to recognise first block image straitaway
    #reply = Snapper.snap_and_identify()
    ser.write('0000'.encode('utf-8'))
    sleep(1)
    ser.write('9000'.encode('utf-8'))
    count = 0
    start = 0
    end = 0
    time_taken = 0
    while count < 2:
        while True:
            block = ser.readline().decode('utf-8')
            if block != '' and count == 0:
                # start timing from moment robot signals it is stopped in front of obstacle 2
                start = time.time()
                break
            # sleep 11 secs to wait for robot to round 1st obstacle before proceeding to take photo
            if count == 1:
                sleep(11)
                break

        #sleep(5)
        print('*********')
        print(block)
        print('*********')
        if count == 0:
            #if reply == None:
            reply = Snapper.snap_and_identify()
            if reply == '39':
                client_sock.send('LEFT'.encode('utf-8'))
                ser.write('1111'.encode('utf-8'))
                sleep(1)
                ser.write('9000'.encode('utf-8'))
            elif reply == '38':
                client_sock.send('RIGHT'.encode('utf-8'))
                ser.write('2222'.encode('utf-8'))
                sleep(1)
                ser.write('9000'.encode('utf-8'))
            else:
                continue
        elif count == 1:
            while True:
                reply = Snapper.snap_and_identify()
                if reply == '39':
                    t1 = threading.Thread(target=obstacle_two_reached, args=(ser,))
                    t1.start()
                    t1.join()
                    end = time.time()
                    time_taken = end - start
                    print(str(time_taken))
                    client_sock.send('LEFT'.encode('utf-8'))
                    if time_taken < 20: 
                        print('sent 3333')
                        ser.write('3333'.encode('utf-8'))
                    elif time_taken > 20 and time_taken < 21:
                        print('sent 7777')
                        ser.write('7777'.encode('utf-8'))
                    else:
                        print('sent 5555')
                        ser.write('5555'.encode('utf-8'))
                    sleep(1)
                    ser.write('9000'.encode('utf-8'))
                    break
                elif reply == '38':
                    t1 = threading.Thread(target=obstacle_two_reached, args=(ser,))
                    t1.start()
                    t1.join()
                    end = time.time()
                    time_taken = end - start
                    print(str(time_taken))
                    client_sock.send('RIGHT'.encode('utf-8'))
                    if time_taken < 20:
                        print('sent 4444')
                        ser.write('4444'.encode('utf-8'))
                    elif time_taken > 20 and time_taken < 21:
                        print('sent 8888')
                        ser.write('8888'.encode('utf-8'))
                    else:
                        print('sent 6666')
                        ser.write('6666'.encode('utf-8'))
                    sleep(1)
                    ser.write('9000'.encode('utf-8'))
                    break
        count = count + 1
        block = ''
    

# called when we want to send command to stm32
def send_command_to_stm32(data, ser):
    ser.write(data)
    # wait 6 seconds for command to register, then send wait command
    ser.readline()
    ser.write('9000'.encode())
    ser.readline()
    print('done')

def main():
    Snapper.snap_and_identify()
    #ser = arduinoserial.SerialPort('/dev/ttyUSB0', 115200)
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    # while True:
    #     # prompt for serial interface used
    #     interface = input("Enter serial interface (E.g. /dev/ttyUSB0): ")
    #     if interface != '':
    #         interface = '/dev/ttyUSB0'
    #         ser = arduinoserial.SerialPort(interface, 115200)
    #         break
    #     else:
    #         c = input('No serial interface specified. Continue? (Y/N): ')
    #         if c == 'Y':
    #             break

    # setup server info
    HOST = "0.0.0.0"
    PORT = 4444

    # restart bluetooth connection process again when disconnected
    while True:
        obstacleCount = ''
        # create hash table to store obstacle list to send to PC (max 8 obstacles)
        obstacleT = HashTable(7)

        #track indoor or outdoor
        outdoor = 0
        #print("Starting RPi camera web server")
        # run start.sh
        #os.system("/home/pi/Desktop/rpi-testing/RPi_Cam_Web_Interface/start.sh")
        #print("RPi camera web server started")

        print("Advertising bluetooth server")
        # execute piscan command
        os.system("sudo hciconfig hci0 piscan")
        # prompt to connect to bluetooth to open RFCOMM interface
        print("Open app and connect to bluetooth server now")
        while True:
            completed = input("Enter Y when connected: ")
            if completed == "Y":
                break

        server_sock = BluetoothSocket(RFCOMM)
        server_sock.bind(("",PORT_ANY))
        server_sock.listen(1)
        port = server_sock.getsockname()[1]
        uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"
        #uuid = "00001101-0000-1000-8000-00805f9b34fb"
        advertise_service( server_sock, "MDP-Server9",
                        service_id = uuid,
                        service_classes = [ uuid, SERIAL_PORT_CLASS ],
                        profiles = [ SERIAL_PORT_PROFILE ],
        #                  protocols = [ OBEX_UUID ]
                        )

        print("Waiting for connection on RFCOMM channel %d" % port)
        print("Connect to bluetooth server again from app")

        client_sock, client_info = server_sock.accept()
        print("Accepted connection from ", client_info)

        try:
            while True:
                # create list of connected sockets
                socket_list = [client_sock]
                # get list of sockets with data waiting to be read
                read_sockets, write_sockets, error_sockets = select.select(socket_list, [], [])
                # iterate through sockets with data to be read and handle them
                for sock in read_sockets:
                    # Android bluetooth socket
                    if sock == client_sock:
                        print('receiving message from Android')
                        data = client_sock.recv(1024)
                        if len(data) == 0: break
                        command = data.decode("utf-8")
                        print("Received {}".format(command))
                        # first command
                        if command == "READYA2RPI":
                            client_sock.send("READYRPI2A".encode("utf-8"))
                        # check for other commands and handle
                        elif "ROBOT" in command:
                            # TODO
                            print("robot position update")
                        elif "ADD" in command:
                            obstacleCount = (command.split('ADD|')[1])[0]
                            if obstacleCount == '1':
                                obstacleT.set_val(obstacleCount, command)
                                print(obstacleT)
                                print()
                                #obstacleL.append(command)
                            else:
                                obstacleT.set_val(obstacleCount, command.split('ADD|')[1])
                                print(obstacleT)
                                print()
                                #obstacleL.append(command.split('ADD|')[1])
                            print("obstacle marking")
                        elif "REMOVE" in command:
                            # remove obstacle 
                            obstacleT.delete_val((command.split('REMOVE|')[1]).split('|')[0])
                            print(obstacleT)
                            print()
                            print("removing obstacle")
                        elif command == "0":
                            command = '0010'
                            send_command_to_stm32(command.encode('utf-8'), ser)
                            print("move forward")
                        elif command == "1":
                            command = '1010'
                            send_command_to_stm32(command.encode('utf-8'), ser)
                            print("move backwards")
                        elif command == "2":
                            command = '2010'
                            send_command_to_stm32(command.encode('utf-8'), ser)
                            print("turning left")
                        elif command == "RIGHT_TURN":
                            command = '3010'
                            send_command_to_stm32(command.encode('utf-8'), ser)
                            print("turning right")
                        elif "START_IMAGE_TASK" in command:
                            # (send obstacle list in queue to PC on conn)
                            outdoor = int(command.split('|')[1])
                            print('outdoor: ' + str(outdoor))
                            if outdoor == 1:
                                print('sending outdoor')
                                #send_command_to_stm32('8888'.encode('utf-8'), ser)
                            else:
                                print('sending indoor')
                                #send_command_to_stm32('7777'.encode('utf-8'), ser)
                            obstacles = ''
                            for o in range(1, 9):
                                if obstacleT.get_val(str(o)):
                                    obstacles = obstacles + obstacleT.get_val(str(o))
                                else:
                                    break
                            print(obstacles)
                            print("starting image task")
                        elif command == "START_FASTEST_TASK":
                            obstacles = ''
                            # send command to stm32 to start moving
                            print("starting fastest car task")
                            fastest_car_handler(ser, client_sock)
                        elif command == "RESET":
                            sys.exit()
        except IOError:
            pass

        print("disconnected")

        client_sock.close()
        server_sock.close()
        print("all done")

if __name__ == '__main__':
    main()
