'''
    This is the client script to connect between the PC running algo to the RPi.
'''
import socket
from ImageRec.imageRec import Snapper
import time

def main():
    HOST = "192.168.9.9" # connect to server running on RPi via WiFi AP
    # HOST = "127.0.0.1" # if testing with the server code running locally host is localhost 
    PORT = 4444


    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,PORT))
        time.sleep(3)
        counter=13
        while True:
            try:
                # data = input('Enter data to send to server: ')
                # if not data:
                #     print('Connection to server closed')
                #     break
                # s.send(data.encode())
                # res = s.recv(1024)
                # print(res.decode('utf-8'))

                # res = s.recv(1024)
                # if (res == 'stop'):
                # reply = 'i'
                # s.send(reply.encode())
                reply = Snapper.snap_and_identify()
                if (reply and reply != 'Bullseye'):
                    toSend = 'i'
                    s.send(toSend.encode())
                    print('sending stop')
                time.sleep(counter)
                counter=counter+3

                # |--aaaaa-------aaaaaaa-------aaaaaaa-------aaaaaaa-------aaaaaaa
                # |---s-------------s-------------s-------------s-------------s
            except IOError:
                pass


if __name__ == '__main__':
    main()
