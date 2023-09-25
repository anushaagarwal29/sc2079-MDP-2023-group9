package com.example.ay22_23s2mdpgrp9.Arena;

import android.content.Context;
import android.widget.TextView;

public class ArenaIndex extends androidx.appcompat.widget.AppCompatTextView {
    String text;
    public ArenaIndex(Context context,String text) {
        super(context);
        this.text = text;
    }
}
