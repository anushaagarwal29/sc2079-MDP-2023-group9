package com.example.ay22_23s2mdpgrp9.bluetooth;


import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;


import com.example.ay22_23s2mdpgrp9.R;

import java.util.ArrayList;
import java.util.List;

public class DeviceListAdapter extends ArrayAdapter<BluetoothDevice> {

    private LayoutInflater layoutInflater;
    private ArrayList<BluetoothDevice> devices;
    private int viewResourceId;

    public DeviceListAdapter(Context context, int tvResourceId, ArrayList<BluetoothDevice> devices) {
        super(context, tvResourceId, devices);
        layoutInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        viewResourceId = tvResourceId;
        this.devices = (ArrayList<BluetoothDevice>) devices;
    }
    @SuppressLint("ViewHolder")
    public View getView(int position, View convertView, ViewGroup Parent)throws SecurityException {
        convertView = layoutInflater.inflate(viewResourceId, null);
        BluetoothDevice device = devices.get(position);
        if (device != null) {
            TextView deviceName = (TextView) convertView.findViewById(R.id.deviceNameTv);
            TextView deviceAddress = (TextView) convertView.findViewById(R.id.deviceAddressTv);
            if (deviceName != null) {
                deviceName.setText(device.getName());
            }
            if (deviceAddress != null) {
                deviceAddress.setText(device.getAddress());
            }
        }
        return convertView;
    }

}
