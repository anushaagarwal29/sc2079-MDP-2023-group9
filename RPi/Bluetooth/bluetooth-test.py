#!/usr/bin
'''
    Before running this script, open another SSH session and run bluetoothctl
    After that execute the command "sudo hciconfig hci0 piscan" to advertise
    the bluetooth server.
    Finally, execute this script with "sudo python3 bluetooth-test.py"
'''
from bluetooth import *
server_sock = BluetoothSocket(RFCOMM)
server_sock.bind(("",PORT_ANY))
server_sock.listen(1)
port = server_sock.getsockname()[1]
uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"
advertise_service( server_sock, "MDP-Server9",
                   service_id = uuid,
                   service_classes = [ uuid, SERIAL_PORT_CLASS ],
                   profiles = [ SERIAL_PORT_PROFILE ],
#                  protocols = [ OBEX_UUID ]
                 )

print("Waiting for connection on RFCOMM channel %d" % port)

client_sock, client_info = server_sock.accept()
print("Accepted connection from ", client_info)

try:
    while True:
        print("In while loop...")
        data = client_sock.recv(1024)
        if len(data) == 0: break
#        print("Received [%s]" % data)
        print("Received {}".format(data.decode("utf-8")))
        client_sock.send("message from pi!")
except IOError:
    pass

print("disconnected")

client_sock.close()
server_sock.close()
print("all done")

