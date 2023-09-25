package com.example.ay22_23s2mdpgrp9.bluetooth;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import android.app.ProgressDialog;
import android.widget.Toast;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.UUID;

@SuppressLint("MissingPermission")
public class BluetoothConnectionService {
    private static final String TAG = "BluetoothConnectionService";
    private static final String APP_NAME = "MDP_GROUP9";
    private static final UUID MY_UUID_INSECURE =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private final BluetoothAdapter bluetoothAdapter;
    private Context context;
    private AcceptThread acceptThread;
    private ConnectThread connectThread;

    protected ConnectedThread connectedThread;
    private BluetoothDevice connectedDevice;
    private UUID deviceUUID;
    ProgressDialog mProgressDialog;


    public BluetoothConnectionService(Context context) {
        this.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        this.context = context;
        startAcceptThread();// start bluetooth server
    }
    /**
     * This thread runs while listening for incoming connections. It behaves
     * like a server-side client. It runs until a connection is accepted
     * (or until cancelled).
     */
    //waits for a connection using another thread instead of the thread that created the fragment
    private class AcceptThread extends Thread {
        private final BluetoothServerSocket bluetoothServerSocket;

        public AcceptThread() {
            BluetoothServerSocket tmp = null;
            try {
                //server serving with this UUID
                tmp = bluetoothAdapter.listenUsingInsecureRfcommWithServiceRecord(APP_NAME, MY_UUID_INSECURE);
                Log.d(TAG, "AcceptThread: Setting up Server using: " + MY_UUID_INSECURE);
            } catch (IOException e) {
                Log.e(TAG, "AcceptThread: Socket's server failed", e);
            }
            bluetoothServerSocket = tmp;
        }
        // run when AcceptThread instance is created
        public void run() {
            Log.d(TAG, "run: AcceptThread Running");
            BluetoothSocket socket = null;
            try {
                // This is a blocking call and will only return on a
                // successful connection or an exception
                Log.d(TAG, "run: RFCOM server socket start....");
                socket = bluetoothServerSocket.accept();
                Log.d(TAG,"run: RFCOM Server accepted connection");
            } catch (IOException e) {
                Log.e(TAG, "Accept Thread: IOException: " + e.getMessage());
            }
            if (socket != null) {
                connectedDevice = socket.getRemoteDevice();
                connected(socket, connectedDevice);
            }
            Log.i(TAG, "End AcceptThread");

        }
        // just to close the bt server socket running on an accept thread instance
        public void cancel() {
            try {
                Log.d(TAG, "cancel: Cancelling AcceptThread");
                bluetoothServerSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "cancel: Close of AcceptThread ServerSocket failed" + e.getMessage());
            }
        }
    }
    /**
     * This thread runs while attempting to make an outgoing connection
     * with a device. It runs straight through; the connection either
     * succeeds or fails.
     */
    //connect as a client
     // both bt device will be running the accept thread server until another device connect with
        // connect thread
    private class ConnectThread extends Thread {
        private BluetoothSocket socket;

        public ConnectThread(BluetoothDevice device, UUID uuid) {
            Log.d(TAG, "ConnectThread: started.");
            connectedDevice = device;
            deviceUUID = uuid;
        }
        // run when connectThread instance is created

        public void run() {
            BluetoothSocket tmp = null;
            Log.i(TAG, "Run ConnectThread");
            //Get a BluetoothSocket for a connection with the
            //given BluetoothDevice
            try {
                Log.d(TAG, "ConnectThread: Trying to create InsecureRfcommSocket using UUID");
                tmp = connectedDevice.createRfcommSocketToServiceRecord(deviceUUID);
            } catch (IOException e) {
                Log.e(TAG, "ConnectThread: Could not create InsecureRFcommSocket " + e.getMessage());
            }
            socket = tmp;
            // cancel because discovering takes a lot of RAM!
            bluetoothAdapter.cancelDiscovery();
            try {
                //this is a blocking call and will only return on a
                // successful connection or an exception
                socket.connect();
                Log.d(TAG, "run: ConnectThread connected");
                connected(socket, connectedDevice);
            } catch (IOException e) {
                // close the socket
                try {
                    socket.close();
                    Log.d(TAG, "run : Closed Socket.");
                } catch (IOException ex) {
                    Log.e(TAG, "ConnectThread: run :Unable to close connection in socket " + ex.getMessage());
                }
                Log.d(TAG, "run: ConnectThread : Could not connect to UUID " + MY_UUID_INSECURE);
            }
        }

        public void cancel() {
            try {
                Log.d(TAG, "cancel: Closing Client Socket.");
                socket.close();
            } catch (IOException e) {
                Log.e(TAG, "cancel: Close() of socket in ConnectThread failed" + e.getMessage());
            }
        }
    }
    /**
     * Start the chat service. Specifically start AcceptThread to begin a
     * session in listening (server) mode. Called by the Activity onResume()
     */
    public synchronized void startAcceptThread() {
        //remove connectThread instance if exist
        Log.d(TAG, "start");
        if (connectThread != null) {
            connectThread.cancel();
            connectThread = null;
        }
        if (acceptThread == null) {
            acceptThread = new AcceptThread();
            //initiate accept thread
            acceptThread.start();
        }
    }
    /**
     AcceptThread starts and sits waiting for a connection.
     Then ConnectThread starts and attempts to make a connection with the other devices AcceptThread.
     **/

    public void startConnectThread(BluetoothDevice device, UUID uuid) {
        Log.d(TAG, "startClient: Started.");

        //init progress dialog
        mProgressDialog = ProgressDialog.show(this.context, "Connecting Bluetooth"
                , "Please Wait...", true);

        connectThread = new ConnectThread(device, uuid);
        connectThread.start();
    }

    /**
     Finally the ConnectedThread which is responsible for maintaining the BTConnection, Sending the data, and
     receiving incoming data through input/output streams respectively.
     **/
    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInputStream;
        private final OutputStream mmOutputStream;

        public ConnectedThread(BluetoothSocket socket) {
            Log.d(TAG, "ConnectedThread: Starting.");

            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            //dismiss the progress dialog when connection is established
            try {
                mProgressDialog.dismiss();
            } catch (NullPointerException e) {
               Log.e(TAG,"dialog dismiss error" + e.getMessage());
            }
            try {
                tmpIn = mmSocket.getInputStream();
                tmpOut = mmSocket.getOutputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }

            mmInputStream = tmpIn;
            mmOutputStream = tmpOut;
        }
        // run when connected thread instance is created
        public void run(){
            byte[] buffer = new byte[1024];  // buffer store for the stream

            int bytes; // bytes returned from read()

            // Keep listening to the InputStream until an exception occurs
            while (true) {
                // Read from the InputStream
                try {
                    bytes = mmInputStream.read(buffer);
                    String incomingMessage = new String(buffer, 0, bytes);
                    System.out.println(incomingMessage);
                    Log.d(TAG, "InputStream: " + incomingMessage);
                    // Broadcast Incoming Message
                    Intent incomingMsgIntent = new Intent("IncomingMsg");
                    incomingMsgIntent.putExtra("receivingMsg", incomingMessage);
                    LocalBroadcastManager.getInstance(context).sendBroadcast(incomingMsgIntent);

                } catch (IOException e) {
                    Log.e(TAG, "write: Error reading Input Stream. " + e.getMessage() );
                    break;
                }
            }
        }
        //Call this from the main activity to send data to the remote device
        public void write(byte[] bytes) {
            String text = new String(bytes, Charset.defaultCharset());
            Log.d(TAG, "write: Writing to outputstream: " + text);
            try {
                mmOutputStream.write(bytes);
            } catch (IOException e) {
                Log.e(TAG, "write: Error writing to output stream. " + e.getMessage() );
            }
        }

        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG,"Connect Thread cancel: There is no connection established");
            }
        }


    }
    private void connected(BluetoothSocket mmSocket,BluetoothDevice mmDevice){
        Log.d(TAG, "connected: Starting.");

        // Start the thread to manage the connection and perform transmissions
        connectedThread = new ConnectedThread(mmSocket);
        connectedThread.start();
        // Broadcast connection status
        Intent connectionStatus = new Intent("connectionStatus");
        connectionStatus.putExtra("btConnected", true);
        connectionStatus.putExtra("btConnectedDevice",mmDevice);
        LocalBroadcastManager.getInstance(context).sendBroadcast(connectionStatus);
    }
    /**
     * Write to the ConnectedThread in an unsynchronized manner
     *
     * @param out The bytes to write
     * @see ConnectedThread#write(byte[])
     */
    public void write(byte[] out) {
        // Create temporary object
        ConnectedThread r;

        // Synchronize a copy of the ConnectedThread
        Log.d(TAG, "write: Write Called.");
        //perform the write
        connectedThread.write(out);
    }
}
