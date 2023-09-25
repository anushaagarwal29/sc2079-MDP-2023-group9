package com.example.ay22_23s2mdpgrp9.FastestCarFragment;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.fragment.app.Fragment;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.example.ay22_23s2mdpgrp9.MainActivity;
import com.example.ay22_23s2mdpgrp9.R;
import com.example.ay22_23s2mdpgrp9.constant.Constant;

import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

public class FastestCarFragment extends Fragment {
    private String TAG ="FastestCarFragment";

    private TextView imageID1TV;
    private TextView imageID2TV;
    private TextView timingTV;
    private ImageView direction1IV;
    private ImageView direction2IV;
    private Button resetBtn;
    private Button startBtn;
    private Button stopBtn;
    private int count;
    private int milliseconds =0;
    private boolean running;
    private boolean wasRunning;
    private Map<String,Integer> directionMapping = new HashMap<String,Integer>() {{
        put("RIGHT",38);
        put("LEFT",39);
    }};


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        if (savedInstanceState != null) {

            // Get the previous state of the stopwatch
            // if the activity has been
            // destroyed and recreated.
            milliseconds
                    = savedInstanceState
                    .getInt("milliseconds");
            running
                    = savedInstanceState
                    .getBoolean("running");
            wasRunning
                    = savedInstanceState
                    .getBoolean("wasRunning");
        }
        runTimer();
        View view = inflater.inflate(R.layout.fragment_fastestcar, container, false);
        LocalBroadcastManager.getInstance(getContext()).registerReceiver(broadcastReceiverIncomingMsg, new IntentFilter("IncomingMsg"));
        imageID1TV = view.findViewById(R.id.imageID1Tv);
        imageID2TV = view.findViewById(R.id.imageID2Tv);
        timingTV = view.findViewById(R.id.timingTV);
        direction1IV = view.findViewById(R.id.direction1IV);
        direction2IV = view.findViewById(R.id.direction2IV);
        resetBtn = view.findViewById(R.id.resetBTN);
        startBtn = view.findViewById(R.id.startBTN);
        stopBtn = view .findViewById(R.id.stopBTN);
        count = 0;
        startBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                sendMessageToRPI("START_FASTEST_TASK");
                running = true;
            }
        });
        resetBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                running = false;
                milliseconds = 0;
                direction1IV.setImageResource(R.drawable.border_detected);
                direction2IV.setImageResource(R.drawable.border_detected);
                imageID1TV.setText("Image ID 1:#");
                imageID2TV.setText("Image ID 2:#");

            }
        });
        stopBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                running = false;
            }
        });


        return view;
    }
    @Override
    public void onSaveInstanceState(
            Bundle savedInstanceState)
    {
        savedInstanceState
                .putInt("milliseconds", milliseconds);
        savedInstanceState
                .putBoolean("running", running);
        savedInstanceState
                .putBoolean("wasRunning", wasRunning);
    }
    @Override
    public void onPause()
    {
        super.onPause();
        wasRunning = running;
        running = false;
    }
    @Override
    public void onResume()
    {
        super.onResume();
        if (wasRunning) {
            running = true;
        }
    }
    private final BroadcastReceiver broadcastReceiverIncomingMsg = new BroadcastReceiver() {
        @SuppressLint("MissingPermission")
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "Receiver Incoming message");
            String msg = intent.getStringExtra("receivingMsg");
            Log.d(TAG,"Incoming Msg: " + msg);
            if(count < 1){
                count++;
                if(msg.equalsIgnoreCase("LEFT")){
                        imageID1TV.setText("Image ID 1:" + directionMapping.get("LEFT"));
                        direction1IV.setImageResource(Constant.imageMapping.get(directionMapping.get("LEFT")));
                }
                else{
                        imageID1TV.setText("Image ID 1:" + directionMapping.get("RIGHT"));
                        direction1IV.setImageResource(Constant.imageMapping.get(directionMapping.get("RIGHT")));
                }
            }else{
                if(msg.equalsIgnoreCase("LEFT")){
                    imageID2TV.setText("Image ID 2:" + directionMapping.get("LEFT"));
                    direction2IV.setImageResource(Constant.imageMapping.get(directionMapping.get("LEFT")));
                }
                else{
                    imageID2TV.setText("Image ID 2:" + directionMapping.get("RIGHT"));
                    direction2IV.setImageResource(Constant.imageMapping.get(directionMapping.get("RIGHT")));
                }
            }
        }
    };
    private void sendMessageToRPI(String text){
        if(!MainActivity.hasBtConnectedDevice || MainActivity.globalBluetoothService ==null){
            Toast.makeText(getContext(), "Connection not establish", Toast.LENGTH_SHORT).show();
            return;
        }
        MainActivity.serialChat = new StringBuilder(MainActivity.serialChat.append("This Device:" + text + '\n'));
        byte[] bytes = text.getBytes(Charset.defaultCharset());
        MainActivity.globalBluetoothService.write(bytes);
    }
    private void runTimer()
    {

        // Creates a new Handler
        final Handler handler
                = new Handler();

        // Call the post() method,
        // passing in a new Runnable.
        // The post() method processes
        // code without a delay,
        // so the code in the Runnable
        // will run almost immediately.
        handler.post(new Runnable() {
            @Override

            public void run()
            {
                int milisecs = (milliseconds/10) %100;
                int secs = (milliseconds/1000)%60;
                int minute = (milliseconds/1000/60)%60;

                // Format the seconds into hours, minutes,
                // and seconds.
                String time
                        = String
                        .format(Locale.getDefault(),
                                "%02d:%02d:%02d",
                                minute,secs, milisecs);

                // Set the text view text.
                timingTV.setText(time);

                // If running is true, increment the
                // seconds variable.
                if (running) {
                    milliseconds+=330;
                }

                // Post the code again
                // with a delay of 1 second.
                handler.postDelayed(this, 330);
            }
        });
    }

}
