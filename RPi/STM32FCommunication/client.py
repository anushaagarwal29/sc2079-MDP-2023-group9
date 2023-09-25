'''
    This is the client script to connect between the PC running algo to the RPi.
'''
import socket
import time

def main():
    HOST = "192.168.9.9" # connect to server running on RPi via WiFi AP
    #HOST = "127.0.0.1" # if testing with the server code running locally host is localhost 
    PORT = 4444

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,PORT))
        while True:
            try:
                data = input("Enter data to send: ")
                if not data:
                    print('Connection to server closed')
                    break
                s.send(data.encode())
                res = s.recv(1024)
                print(res.decode('utf-8'))
            except IOError:
                pass


if __name__ == '__main__':
    main()
