package com.example.ay22_23s2mdpgrp9.bluetooth;

import static android.app.Activity.RESULT_CANCELED;
import static android.app.Activity.RESULT_OK;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.Location;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.SpannableStringBuilder;
import android.text.method.ScrollingMovementMethod;
import android.text.style.BackgroundColorSpan;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.ay22_23s2mdpgrp9.MainActivity;
import com.example.ay22_23s2mdpgrp9.R;

import java.io.IOException;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;


public class BluetoothFragment extends Fragment {

    private Button searchBtn;

    private Button discoverableBtn;

    private Button onOffBtn;

    private TextView chatTv;

    private EditText messageEt;

    private Button sendMessageBtn;

//    private BluetoothConnectionService bluetoothConnectionService;
    private static final UUID MY_UUID_INSECURE =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
//    private BluetoothDevice connectedDevice;
    private BluetoothAdapter btAdapter;

    private ArrayList<BluetoothDevice> btPairedDevices = new ArrayList<>();

    private ListView pairedDevicesLv;

    private ListView availableDevicesLv;

    private ArrayList<BluetoothDevice> btAvailableDevice = new ArrayList<>();

    private DeviceListAdapter btAvailableDeviceAdapter;

    private DeviceListAdapter btPairedDeviceAdapter;

//    private boolean hasBtConnectedDevice = false;

    private static final String TAG = "BluetoothFragment";

    private static final int BT_ENABLE_REQUEST_CODE = 8;

    private boolean b1Registered = false;

    private boolean b2Registered = false;

    private boolean b3registered = false;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }


    //broadcast receiver for ACTION_STATE_CHANGED
    private final BroadcastReceiver mBroadcastReceiver1 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (btAdapter.ACTION_STATE_CHANGED.equals(action)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, btAdapter.ERROR);
                switch (state) {
                    case BluetoothAdapter.STATE_OFF:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: STATE OFF");
                        break;
                    case BluetoothAdapter.STATE_TURNING_OFF:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: STATE TURNING OFF");
                        break;
                    case BluetoothAdapter.STATE_ON:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: STATE ON");
                        break;
                    case BluetoothAdapter.STATE_TURNING_ON:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: STATE TURNING ON");
                        break;

                }
            }
        }
    };
    // broadcast receiver for ACTION_SCAN_MODE_CHANGED
    private final BroadcastReceiver mBroadcastReceiver2 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (btAdapter.ACTION_SCAN_MODE_CHANGED.equals(action)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_SCAN_MODE, btAdapter.ERROR);
                switch (state) {
                    case BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE:
                        Log.d(TAG, "ACTION_SCAN_MODE_CHANGED RECEIVER: Discoverability Enabled");
                        break;
                    case BluetoothAdapter.SCAN_MODE_CONNECTABLE:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: Discoverability Disabled " +
                                "Able to receive connection");
                        break;
                    case BluetoothAdapter.SCAN_MODE_NONE:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: Discoverability Disabled " +
                                "Not able to receive connection");
                        break;
                    case BluetoothAdapter.STATE_CONNECTING:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: Connecting...");
                        break;
                    case BluetoothAdapter.STATE_CONNECTED:
                        Log.d(TAG, "ACTION_STATE_CHANGE RECEIVER: Connected");
                        break;

                }
            }
        }
    };
    // broadcast Receiver for ACTION_FOUND
    private final BroadcastReceiver mBroadcastReceiver3 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) throws SecurityException {
            final String action = intent.getAction();
            Log.d(TAG, "ACTION_FOUND receiver : Looking for unpaired devices");
            if (action.equals(BluetoothDevice.ACTION_FOUND)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if(!btPairedDevices.contains(device) && !btAvailableDevice.contains(device))
                    btAvailableDevice.add(device);
                Log.d(TAG, "ACTION_FOUND receiver: " + device.getName() + ": " + device.getAddress());
                btAvailableDeviceAdapter = new DeviceListAdapter(context, R.layout.device_adapter_view, btAvailableDevice);
                availableDevicesLv.setAdapter(btAvailableDeviceAdapter);
            }
        }
    };

    // broadcast receiver for ACTION_BOND_STATE_CHANGED
    private final BroadcastReceiver mBroadcastReceiver4 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) throws SecurityException {
            final String action = intent.getAction();
            if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                //3 CASES
                switch (device.getBondState()) {
                    //bonded already
                    case BluetoothDevice.BOND_BONDED:
                        Log.d(TAG, "ACTION_BOND_STATE_CHANGED : Bonded");
                        btAvailableDevice.remove(device);
                        btAvailableDeviceAdapter = new DeviceListAdapter(getContext(),R.layout.device_adapter_view,btAvailableDevice);
                        availableDevicesLv.setAdapter(btAvailableDeviceAdapter);

                        btPairedDevices.add(device);
                        btPairedDeviceAdapter = new DeviceListAdapter(getContext(),R.layout.device_adapter_view,btPairedDevices);
                        pairedDevicesLv.setAdapter(btPairedDeviceAdapter);
                        break;
                    // establishing a bond
                    case BluetoothDevice.BOND_BONDING:
                        Log.d(TAG, "ACTION_BOND_STATE_CHANGED : Establishing a bond");
                        break;
                    // breaking a bond
                    case BluetoothDevice.BOND_NONE:
                        Log.d(TAG, "ACTION_BOND_STATE_CHANGED : Breaking a bond");
                        break;
                }

            }
        }
    };
    private final BroadcastReceiver mBroadcastReceiver5 = new BroadcastReceiver() {
        @SuppressLint("MissingPermission")
        @Override
        public void onReceive(Context context, Intent intent)  {
            Log.d(TAG, "Connection status receiver : Bluetooth Device Connected");
            MainActivity.hasBtConnectedDevice = intent.getBooleanExtra("btConnected",false);
            MainActivity.connectedDevice = intent.getParcelableExtra("btConnectedDevice");
                if(MainActivity.connectedDevice.getName()!=null){
                    Toast.makeText(context,"Connected to:"+MainActivity.connectedDevice.getName(),Toast.LENGTH_SHORT).show();
                }
                else{
                    Toast.makeText(context,"Connected to:"+MainActivity.connectedDevice.getAddress(),Toast.LENGTH_SHORT).show();
                }
        }
    };
    private final BroadcastReceiver broadcastReceiverIncomingMsg = new BroadcastReceiver() {
        @SuppressLint("MissingPermission")
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "Receiver Incoming message");
            String header = MainActivity.connectedDevice.getAddress();
            String msg = intent.getStringExtra("receivingMsg");
            if(MainActivity.connectedDevice.getName()!=null){
                    header = MainActivity.connectedDevice.getName();
            }
            chatTv.append(header + ": " + msg + "\n");
            MainActivity.serialChat = new StringBuilder().append(chatTv.getText());
            chatTv.setText(MainActivity.serialChat);
        }
    };

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_bluetooth, container, false);
        searchBtn = (Button) view.findViewById(R.id.searchBtn);
        discoverableBtn = (Button) view.findViewById(R.id.discoverableBtn);
        onOffBtn = (Button) view.findViewById(R.id.onOffBtn);
        sendMessageBtn = (Button) view.findViewById(R.id.sendMsgBtn);
        chatTv = (TextView) view.findViewById(R.id.chatTv);
        chatTv.setMovementMethod(new ScrollingMovementMethod());
        chatTv.setText(MainActivity.serialChat);
        messageEt = (EditText) view.findViewById(R.id.messageEt);
        pairedDevicesLv = (ListView) view.findViewById(R.id.pairedDevicesLv);
        availableDevicesLv = (ListView) view.findViewById(R.id.availableDevicesLv);
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        IntentFilter bondFilter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        getActivity().registerReceiver(mBroadcastReceiver4, bondFilter);

        //Register Broadcast Receiver for connection status
        LocalBroadcastManager.getInstance(getContext()).registerReceiver(mBroadcastReceiver5, new IntentFilter("connectionStatus"));
        // Register Broadcast Receiver for incoming message
        // suppose to put this when you established a connection
        LocalBroadcastManager.getInstance(getContext()).registerReceiver(broadcastReceiverIncomingMsg, new IntentFilter("IncomingMsg"));
        availableDevicesLv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                if (ActivityCompat.checkSelfPermission(getActivity(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                    getActivity().requestPermissions(new String[]{Manifest.permission.BLUETOOTH_SCAN}, BT_ENABLE_REQUEST_CODE); //Any number
                }
                btAdapter.cancelDiscovery();
                String deviceName = btAvailableDevice.get(i).getName();
                String deviceAddress = btAvailableDevice.get(i).getAddress();
                Log.d(TAG,"OnItemClick : " + deviceName + ": " + deviceAddress);
                if(Build.VERSION.SDK_INT > Build.VERSION_CODES.JELLY_BEAN_MR2){
                    Log.d(TAG, "Trying to pair with " + deviceName + ": " + deviceAddress);
                    btAvailableDevice.get(i).createBond();

                }
            }
        });
        pairedDevicesLv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                if (ActivityCompat.checkSelfPermission(getActivity(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                    getActivity().requestPermissions(new String[]{Manifest.permission.BLUETOOTH_SCAN}, BT_ENABLE_REQUEST_CODE); //Any number
                }
                btAdapter.cancelDiscovery();
                MainActivity.connectedDevice = btPairedDevices.get(i);
                MainActivity.globalBluetoothService = new BluetoothConnectionService(getActivity());
                startConnection();
            }
        });
        implementListeners();
        return view;
    }
    private void startBtConnection(BluetoothDevice device,UUID uuid){
        Log.d(TAG,"startBtConnection: Initializing RFCOM Bluetooth Connection");
        MainActivity.globalBluetoothService.startConnectThread(device,uuid);
    }
    public void startConnection(){
        startBtConnection(MainActivity.connectedDevice,MY_UUID_INSECURE);
    }


    private void implementListeners() {
        onOffBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggleOnOff();
            }
        });
        discoverableBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                enableDiscoverable();
            }
        });
        searchBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                searchAvailableDevices();
            }
        });
        sendMessageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(!btAdapter.isEnabled() || !MainActivity.hasBtConnectedDevice || MainActivity.globalBluetoothService==null){
                    Toast.makeText(getContext(),"Connection not established",Toast.LENGTH_LONG).show();
                    return;
                }
                    String text = messageEt.getText().toString().trim();
                    byte[] bytes = text.getBytes(Charset.defaultCharset());
                    MainActivity.globalBluetoothService.write(bytes);
                    messageEt.setText("");
                    chatTv.append("This Device: " + text + "\n");
                    MainActivity.serialChat = new StringBuilder().append(chatTv.getText());
                    chatTv.setText(MainActivity.serialChat);
            }
        });
    }

    private void toggleOnOff() {
        if (this.btAdapter == null) {
            Log.d(TAG, "toggleOnOff: Does not have BT functionality");
        } else if (!btAdapter.isEnabled()) {
            //enable bt
            Intent enableBTIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivity(enableBTIntent);
            IntentFilter bTIntent = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
            getActivity().registerReceiver(mBroadcastReceiver1, bTIntent);
            Log.d(TAG,"#############################################################");
            b1Registered = true;
        } else if (btAdapter.isEnabled()) {
            if (ActivityCompat.checkSelfPermission(getActivity(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                getActivity().requestPermissions(new String[]{
                        Manifest.permission.BLUETOOTH_CONNECT}, BT_ENABLE_REQUEST_CODE); //Any number
            }
            //turn off blue tooth
            btAdapter.disable();
            clearListView();
            if(MainActivity.hasBtConnectedDevice){
                MainActivity.hasBtConnectedDevice = false;
            }
            MainActivity.connectedDevice = null;
            MainActivity.globalBluetoothService=null;
            Toast.makeText(getContext(), "Bluetooth turning off", Toast.LENGTH_SHORT).show();
            //register broadcast receiver for action_state_changed
            IntentFilter bTIntent = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
            getActivity().registerReceiver(mBroadcastReceiver1, bTIntent);
            b1Registered = true;
        }
    }

    private void enableDiscoverable() {
        Log.d(TAG, "enableDiscoverable: Making device discoverable for 300 seconds");
        Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
        discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
        startActivity(discoverableIntent);
        IntentFilter bTIntent = new IntentFilter(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED);
        getActivity().registerReceiver(mBroadcastReceiver2, bTIntent);
        b2Registered = true;
        MainActivity.globalBluetoothService = new BluetoothConnectionService(getActivity());

    }

    private void searchAvailableDevices() {
        Log.d(TAG, "searchAvailableDevices: looking for unpaired devices");
        int permissionCheck = getActivity().checkSelfPermission("Manifest.permission.ACCESS_FINE_LOCATION");
        permissionCheck += getActivity().checkSelfPermission("Manifest.permission.ACCESS_COARSE_LOCATION");
        if (permissionCheck != 0)
            getActivity().requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, 1001);
        if(!btAdapter.isEnabled()){
            Toast.makeText(getContext(),"Bluetooth is off. Please turn on bluetooth",Toast.LENGTH_LONG).show();
        }
        //location is required to search for available devices
        else if(!locationCheck()){
            buildAlertMessageNoGps();
        }
        else {
            if (btAdapter.isDiscovering()) {
                btAdapter.cancelDiscovery();
                Log.d(TAG, "searchAvailableDevices : stop searching for unpaired devices");
                btAdapter.startDiscovery();
            } else if (!btAdapter.isDiscovering()) {
                btAdapter.startDiscovery();
            }
            updatePairedDevices();
            IntentFilter searchIntentFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            getActivity().registerReceiver(mBroadcastReceiver3, searchIntentFilter);
            b3registered = true;
        }
    }
    @SuppressLint("MissingPermission")
    private void updatePairedDevices(){
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        Set<BluetoothDevice> pairedDevices = btAdapter.getBondedDevices();
        if (!pairedDevices.isEmpty()) {
            this.btPairedDevices.clear();
            this.btPairedDevices.addAll(pairedDevices);
            btPairedDeviceAdapter = new DeviceListAdapter(getContext(), R.layout.device_adapter_view,btPairedDevices);
            pairedDevicesLv.setAdapter(btPairedDeviceAdapter);
        }
    }
    private void clearListView(){
        this.btPairedDevices.clear();
        this.btAvailableDevice.clear();
        btPairedDeviceAdapter = new DeviceListAdapter(getContext(), R.layout.device_adapter_view,btPairedDevices);
        pairedDevicesLv.setAdapter(btPairedDeviceAdapter);
        btAvailableDeviceAdapter = new DeviceListAdapter(getContext(), R.layout.device_adapter_view,btAvailableDevice);
        availableDevicesLv.setAdapter(btAvailableDeviceAdapter);
    }

    public boolean locationCheck() {
        final LocationManager manager = (LocationManager) getActivity().getSystemService(Context.LOCATION_SERVICE);
        return manager.isProviderEnabled(LocationManager.GPS_PROVIDER);
    }

    private void buildAlertMessageNoGps() {
        final AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setMessage("Your GPS seems to be disabled, do you want to enable it?")
                .setCancelable(false)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(final DialogInterface dialog, final int id) {
                        startActivity(new Intent(android.provider.Settings.ACTION_LOCATION_SOURCE_SETTINGS));
                    }
                })
                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                    public void onClick(final DialogInterface dialog, final int id) {
                        dialog.cancel();
                    }
                });
        final AlertDialog alert = builder.create();
        alert.show();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if(b1Registered) getActivity().unregisterReceiver(mBroadcastReceiver1);
        if(b2Registered) getActivity().unregisterReceiver(mBroadcastReceiver2);
        if(b3registered) getActivity().unregisterReceiver(mBroadcastReceiver3);
        getActivity().unregisterReceiver(mBroadcastReceiver4);
    }



}