package com.wzq.posixthread;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    private EditText threadsText;
    private EditText iterationText;
    private final int  threadsCnt = 4;
    private final int iterationCnt = 5;
    private TextView logText;

    private PthreadUtil pthreadUtil;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        pthreadUtil = new PthreadUtil(this);

        pthreadUtil.nativeInit();
        threadsText = (EditText) findViewById(R.id.threadsText);
        iterationText = (EditText) findViewById(R.id.iterationText);
        logText = (TextView) findViewById(R.id.logText);
    }

    @Override
    protected void onDestroy() {
        pthreadUtil.nativeFree();
        super.onDestroy();
    }

    public void startExe(View view)
    {

        int id = Integer.parseInt(threadsText.getText().toString().trim());
        if(id <= 0)
            id = threadsCnt;
        int itertation = Integer.parseInt(iterationText.getText().toString().trim());
        if(itertation <= 0)
            itertation = iterationCnt;
        pthreadUtil.posixThreads(id, itertation);
    }


    public void onNativeMessage(final String message)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                logText.append(message);
                logText.append("\n");
            }
        });
    }


    public void startClear(View view)
    {
        logText.setText("");
    }
}
