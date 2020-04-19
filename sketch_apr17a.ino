#include <ESP8266WiFi.h>
#include <Servo.h>

Servo myservo,myservo2;

// 設定無線基地台SSID跟密碼
const char* ssid     = "SSID";     //改成您的SSID 
const char* password = "PASSWORD";   //改成您的密碼

// 設定 web server port number 80
WiFiServer server(80);

// 儲存 HTTP request 的變數
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

void setup() {
  Serial.begin(115200);
  // 將輸出變數初始化
  myservo.attach(D1);
  myservo.write(90);
  myservo2.attach(D2);
  myservo2.write(90);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // 使用SSID 跟 password 連線基地台
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // 使用COM Port 列出取得的IP address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop(){
  WiFiClient client = server.available();   // 等待 clients 連線

  if (client) {                             // 假使新的用戶端連線
    Serial.println("New Client.");          // 從序列 Port印出訊息內容
    String currentLine = "";                // 清空這行的內容 
    while (client.connected()) {            // 當 client繼續連線持續執行迴圈
      if (client.available()) {             // 假使從 client 有讀到字元
        char c = client.read();             // 讀取這個字元
        Serial.write(c);                    // 印出這個字元在串列視窗
        header += c;
        if (c == '\n') {                    // 假使是換行符號

          // 假使目前的一行是空白且有兩個新行，就結束 client HTTP 的要求
          if (currentLine.length() == 0) {
            // HTTP 表頭開始時，會有回應碼 response code (如： HTTP/1.1 200 OK)
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // 依情況調整馬達角度
            if (header.indexOf("GET /5/on") >= 0) 
            {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(LED_BUILTIN, LOW);
              myservo.write(0);
              delay(100);
              myservo.write(90);
              digitalWrite(LED_BUILTIN, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) 
              {
                 Serial.println("GPIO 5 off");
                 output5State = "off";
                 digitalWrite(LED_BUILTIN, LOW);
                 myservo.write(180);
                 delay(100);
                 myservo.write(90);
                 digitalWrite(LED_BUILTIN, HIGH);
               } else if (header.indexOf("GET /4/on") >= 0) 
                 {
                   Serial.println("GPIO 4 on");
                   output4State = "on";
                   digitalWrite(LED_BUILTIN, LOW);
                   myservo2.write(0);
                   delay(100);
                   myservo2.write(90);
                   digitalWrite(LED_BUILTIN, HIGH);
                 } else if (header.indexOf("GET /4/off") >= 0) 
                   {
                     Serial.println("GPIO 4 off");
                     output4State = "off";
                     digitalWrite(LED_BUILTIN, LOW);
                     myservo2.write(180);
                     delay(100);
                     myservo2.write(90);
                     digitalWrite(LED_BUILTIN, HIGH);
                   }
            
            // 顯示 HTML 網頁
            client.println("<html>");
            client.println("<head>");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // 設定 on/off 按鈕的CSS
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // 網頁表頭
            client.println("<body><h1>ESP8266 Web Server</html>");
            
            // 顯示按鈕1 按鈕的狀態是 ON/OFF  
            client.println("<p>Light_Rear - State " + output5State + "</p>");

            // 按鈕假使狀態是 off, 就要顯示 ON        
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // 顯示現在按鈕2 按鈕的狀態是 ON/OFF  
            client.println("<p>Light_Close " + output4State + "</p>");

            // 按鈕假使狀態是 off, 就要顯示 ON      
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button/a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button/a></p>");
            }
            client.println("</body></html>");                           
            
            // 使用空白行結束 HTTP回應
            client.println();
           
            break;
          } else {   // 假使有新的一行, 清除目前這一行
            currentLine = "";
          }
        } else if (c != '\r') {  // 讀取到的不是換行符號 
          currentLine += c;      // 增加一個字元在本行最後
        }
      }
    }
    // 清除表頭變數
    header = "";
    // 關閉連線 connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
