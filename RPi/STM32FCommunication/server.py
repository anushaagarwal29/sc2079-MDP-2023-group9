'''
    This is the server code that is running on the RPi.
'''
import socket

def main():
    HOST = "0.0.0.0" # listen for connection on any interface
    PORT = 4444 # listen on port 4444

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        while True:
            s.listen()
            print("Listening for connection on port 4444")
            conn, addr = s.accept()
            with conn:
                try:
                    print(f"Connected by {addr}")
                    while True:
                        data = conn.recv(1024)
                        print(data.decode('utf-8'))
                        if not data:
                            print('Connection closed by client')
                            break
                        conn.send("Data transfer successful!".encode())
                except IOError:
                    pass


if __name__ == '__main__':
    main()
