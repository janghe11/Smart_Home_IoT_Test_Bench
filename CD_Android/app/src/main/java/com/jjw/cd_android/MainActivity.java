package com.jjw.cd_android;

import android.os.Bundle;
import android.os.StrictMode;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;


public class MainActivity extends AppCompatActivity {

    String PHPandClientId, PHPandModeGet, PHPavrCommGet;

    // 데이터를 보기위한 TextView
    TextView txtView, txtResult, boilerValue;
    // 결과값 전역변수 저장
    String myResult;
    // 보일러 온도 저장 변수
    String temperature;
    // 현재 서버에 저장된 데이터 저장 변수
    String phpData = "";
    // json객체 변수선언
    JSONObject temp;
    // json파싱을 통한 avr_data 저장변수
    String jsonData = "";

    TextView txtState;
    ToggleButton gasValve, doorLock, boilerControl;
    ImageView gasOpenImg, gasCloseImg, doorLockImg, doorUnlockImg, txt_door, txt_gas, txt_boiler;
    SeekBar boilerBar;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        txtState = (TextView) findViewById(R.id.txtState);
        boilerValue = (TextView) findViewById(R.id.boilerValue);

        gasValve = (ToggleButton) findViewById(R.id.gasValve);
        doorLock = (ToggleButton) findViewById(R.id.doorLock);
        boilerControl = (ToggleButton) findViewById(R.id.boilerControl);

        gasOpenImg = (ImageView) findViewById(R.id.gasOpenImg);
        gasCloseImg = (ImageView) findViewById(R.id.gasCloseImg);
        doorLockImg = (ImageView) findViewById(R.id.doorLockImg);
        doorUnlockImg = (ImageView) findViewById(R.id.doorUnlockImg);
        txt_door = (ImageView) findViewById(R.id.txt_door);
        txt_gas = (ImageView) findViewById(R.id.txt_gas);
        txt_boiler = (ImageView) findViewById(R.id.txt_boiler);

        boilerBar = (SeekBar) findViewById(R.id.boilerBar);

        StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder().detectDiskReads().detectDiskWrites().detectNetwork().penaltyLog().build());

        doorLock.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (doorLock.isChecked()) {// 도어락 열림
                    PHPandClientId = "capstonemp12d160422r002";
                    PHPandModeGet = "2";
                    PHPavrCommGet = "u";
                    try {
                        HttpPostData();
                        if(jsonData.equals("u")){
                            doorLockImg.setVisibility(View.INVISIBLE);
                            doorUnlockImg.setVisibility(View.VISIBLE);
                            txt_door.setVisibility(View.VISIBLE);
                            Toast.makeText(getApplicationContext(), "문이 열렸습니다. 확인해 주세요.", Toast.LENGTH_SHORT).show();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                } else {// 도어락 닫힘
                    PHPandClientId = "capstonemp12d160422r002";
                    PHPandModeGet = "2";
                    PHPavrCommGet = "l";
                    try {
                        HttpPostData();
                        if(jsonData.equals("l")){
                            doorLockImg.setVisibility(View.VISIBLE);
                            doorUnlockImg.setVisibility(View.INVISIBLE);
                            txt_door.setVisibility(View.INVISIBLE);
                            Toast.makeText(getApplicationContext(),"문이 안전하게 닫혔습니다.",Toast.LENGTH_SHORT).show();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        });

        gasValve.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (gasValve.isChecked()) {// 가스밸브 오픈
                    PHPandClientId = "capstonemp12d160422r002";
                    PHPandModeGet = "2";
                    PHPavrCommGet = "v";
                    gasCloseImg.setVisibility(View.INVISIBLE);
                    gasOpenImg.setVisibility(View.VISIBLE);
                    try {
                        HttpPostData();
                        if(jsonData.equals("v")){
                            gasCloseImg.setVisibility(View.INVISIBLE);
                            gasOpenImg.setVisibility(View.VISIBLE);
                            txt_gas.setVisibility(View.VISIBLE);
                            Toast.makeText(getApplicationContext(),"가스밸브가 열려있습니다. 외출 시 차단시켜주세요.",Toast.LENGTH_SHORT).show();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                } else {// 가스밸브 차단
                    PHPandClientId = "capstonemp12d160422r002";
                    PHPandModeGet = "2";
                    PHPavrCommGet = "g";
                    try {
                        HttpPostData();
                        if(jsonData.equals("g")){
                            gasOpenImg.setVisibility(View.INVISIBLE);
                            gasCloseImg.setVisibility(View.VISIBLE);
                            txt_gas.setVisibility(View.INVISIBLE);
                            Toast.makeText(getApplicationContext(),"가스밸브가 안전하게 차단되었습니다.",Toast.LENGTH_SHORT).show();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        });

        boilerControl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (boilerControl.isChecked()) {// 보일러 ON
                    PHPandClientId = "capstonemp12d160422r002";
                    PHPandModeGet = "2";
                    PHPavrCommGet = "0";
                    boilerValue.setText(String.format(" 현재 온도 : 18°"));
                    txt_boiler.setVisibility(View.VISIBLE);
                    boilerValue.setVisibility(View.VISIBLE);
                    Toast.makeText(getApplicationContext(),"보일러 전원이 ON 되었습니다.",Toast.LENGTH_SHORT).show();
                    try {
                        HttpPostData();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                } else {// 보일러 OFF
                    PHPandClientId = "capstonemp12d160422r002";
                    PHPandModeGet = "2";
                    PHPavrCommGet = "o";
                    txt_boiler.setVisibility(View.INVISIBLE);
                    boilerValue.setVisibility(View.INVISIBLE);
                    Toast.makeText(getApplicationContext(),"보일러 전원이 OFF 되었습니다.",Toast.LENGTH_SHORT).show();
                    try {
                        HttpPostData();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        });

        // 보일러 온도 조절 (18도 ~ 33도)
        boilerBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                boilerValue.setText(String.format(" 현재 온도 : %d°", progress + 18));

                temperature = Integer.toString(progress);

                if (progress==10) {
                    temperature = "a";
                } else if (progress==11) {
                    temperature = "b";
                } else if (progress==12) {
                    temperature = "c";
                } else if (progress==13) {
                    temperature = "d";
                } else if (progress==14) {
                    temperature = "e";
                } else if (progress==15) {
                    temperature = "f";
                }

                PHPandClientId = "capstonemp12d160422r002";
                PHPandModeGet = "2";
                PHPavrCommGet = temperature;

                try {
                    HttpPostData();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
    //------------------------------
    //   Http Post로 주고 받기
    //------------------------------
    public void HttpPostData() throws IOException {
        try {
            URL control_url = new URL("http://localip/control.php"); // URL 설정
            HttpURLConnection http = (HttpURLConnection) control_url.openConnection(); // url을 연결한 객체 생성 접속

            http.setDefaultUseCaches(false); // 캐싱데이터 디폴트 값 설정
            http.setDoInput(true); // 서버에서 읽기 모드 지정
            http.setDoOutput(true); // 서버로 쓰기 모드 지정
            http.setRequestMethod("POST"); // 전송 방식은 POST
            http.setRequestProperty("content-type", "application/x-www-form-urlencoded"); // 전달 방식 설정

            StringBuffer buffer = new StringBuffer();
            buffer.append("andClientId").append("=").append(PHPandClientId).append("&"); // php 변수에 값 대입
            buffer.append("andModeSet").append("=").append(PHPandModeGet).append("&"); // php 변수 앞에 '$' 붙이지 않는다
            buffer.append("avrCommSet").append("=").append(PHPavrCommGet); // 변수 구분은 '&' 사용

            PrintWriter writer = new PrintWriter(new OutputStreamWriter(http.getOutputStream()));
            writer.write(buffer.toString());
            writer.flush();

            BufferedReader reader = new BufferedReader(new InputStreamReader(http.getInputStream()));
            StringBuilder builder = new StringBuilder();
            String line;

            while ((line = reader.readLine()) != null) {
                builder.append(line);
            }

            myResult = builder.toString(); // 전송결과를 전역 변수에 저장

            try {
                JSONArray pi = new JSONArray(myResult);


                temp = pi.getJSONObject(0);
                jsonData = temp.getString("avr_data");
                txtState.setText(" 현재 Home 상태 : ");
                txtState.append(temp.getString("rpi_time")); // 최근 보낸 데이터의 시간 출력


            } catch (JSONException e) {
                e.printStackTrace();
            }

        } catch (MalformedURLException e) {

        } catch (IOException e) {
            //
        } // try
    } // HttpPostData
}