#include <Wire.h>
#include "SSD1306Wire.h"
//2020.2.19
#include "images.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>                
#include <Time.h>
#include <Timezone.h>
#include "NTP.h"


int json_state;//天气返回编码
int json_state2;//温度返回值
String json_temp;

const char* AP_SSID ="GayHome-Guest";         // 你的WiFi账号
const char* AP_PSK = "xx666309";        // 你的WiFi密码
const char *host = "ip-api.com"; //欲访问的域名

// 北京时间时区
#define STD_TIMEZONE_OFFSET +8    // Standard Time offset (-7 is mountain time)

TimeChangeRule mySTD = {"", First,  Sun, Jan, 0, STD_TIMEZONE_OFFSET * 60};
Timezone myTZ(mySTD, mySTD);

WiFiClient client;                       //创建一个网络对象

String key = "SFYwl_WGlDEUncVhp";   //心知天气的秘钥，可以自己去注册也可以用我的
String weizhi = "zhuhai";        //这里写你的地址

//液晶部分
SSD1306Wire display(0x3c, 5, 4);   //SDA SCL
typedef void (*Demo)(void);


/*====================================画天气图标函数===============================*/
void drawzhongyu() {

  display.drawXbm(drawposition, 0, WiFi_Logo_width, WiFi_Logo_height, zhongyu);

}
void drawdayu() {

  display.drawXbm(drawposition, 0, WiFi_Logo_width, WiFi_Logo_height, dayu);

}
void drawxiaoyu() {

  display.drawXbm(drawposition, 0, WiFi_Logo_width, WiFi_Logo_height, xiaoyu);

}
void drawqingtian() {

  display.drawXbm(drawposition, 0, WiFi_Logo_width, WiFi_Logo_height, qingtian);

}
void drawduoyun() {

  display.drawXbm(drawposition, 0, WiFi_Logo_width, WiFi_Logo_height, duoyun);

}

void drawzhu() {

  display.drawXbm(32, 0, 64, 64, WiFi_Logo_bits);

}
void drawseconds_iconclear() {

  display.drawXbm(0, 40, 24, 24, icon_clear);

}
Demo demos[] = {drawqingtian, drawduoyun, drawxiaoyu, drawzhongyu, drawdayu, drawzhu, drawseconds_iconclear}; //6


/*========================画图部分结束，接下来是数据处理部分=====================*/



void wifi_start_connect()              //连接WIFI
{
  WiFi.mode(WIFI_STA);                 //设置esp8266 工作模式
  Serial.println("Connecting to ");    //写几句提示
  Serial.println(AP_SSID);
  WiFi.begin(AP_SSID, AP_PSK);         //连接wifi
  WiFi.setAutoConnect(true);
  while (WiFi.status() != WL_CONNECTED) //这个函数是wifi连接状态，返回wifi链接状态
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected IP is");
  Serial.println(WiFi.localIP());
}



void parseUserData(String content)
{
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 210;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(content);

  JsonObject& results_0 = root["results"][0];


  JsonObject& results_0_now = results_0["now"];
  //const char* results_0_now_text = results_0_now["text"];  // 天气情况
  int results_0_now_code = results_0_now["code"];  //天气情况识别码
  const char* results_0_now_temperature = results_0_now["temperature"];//气温
  

  

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);

  json_temp = results_0_now_temperature;   
   
  json_state = results_0_now_code;
  
}

//调用api函数
void apicmd()
{
 
  if (client.connect("api.seniverse.com", 80) == 1)    //服务连接
  {
    client.print("GET /v3/weather/now.json?key=");
    client.print(key);
    client.print("&location=");
    client.print(weizhi);
    client.print("&language=zh-Hans&unit=c HTTP/1.1\r\n");

    client.print("Host:api.seniverse.com\r\n");
    client.print("Accept-Language:zh-cn\r\n");
    client.print("Connection:close\r\n\r\n");                 //向心知天气的服务器发送请求。


    
    if (client.find("\r\n\r\n") == 1)                         //跳过返回的json数据头，
    {
      String json_from_server = client.readStringUntil('\n'); //读取返回的JSON数据
      Serial.println(json_from_server);                      //打印json数据
      parseUserData(json_from_server);                       //调用josn解析函数，并传参

    }
  }
  else
  {
    Serial.println("服务器连接失败");
    delay(5000);
  }

  client.stop();                                            //关闭HTTP

}

String Middle_minutes;
String Middle_hours;
//时间获取函数
void updateDisplay(void) {

  TimeChangeRule *tcr;        // Pointer to the time change rule

  // Read the current UTC time from the NTP provider
  time_t utc = now();

  // Convert to local time taking DST into consideration
  time_t localTime = myTZ.toLocal(utc, &tcr);

  // Map time to pixel positions
  //int weekdays =   weekday(localTime);
  String days    =  (String) day(localTime);
  String months  = (String) month(localTime);
 // String years   = (String) year(localTime);
 // String seconds = (String) second(localTime);
  String minutes = (String) minute(localTime);
  String hours   = (String) hour(localTime); 

/*串口输出时间
  Serial.println("");
  Serial.print("Current local time:");
  Serial.print(days);
  Serial.print("/");
  Serial.print(months);
  Serial.print("/");
  Serial.print(years);
  Serial.print(" - ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print(" - ");
  Serial.print(dayStr(weekdays));
  Serial.println("");
*/


  if (Middle_minutes != minutes)
  {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    Serial.println("调用天气");
    apicmd();
    
    display.drawString(75, 40, json_temp);
    display.drawString(100, 40, "°C");

    display.drawString(10, 5, hours);
    display.drawString(38, 3, ":");
    display.drawString(45, 5, minutes);
    
    判断返回输出图标
    if (json_state == 0)
    {
      demos[0]();
      
    }
    else if (json_state > 0  && json_state < 4)
    {
      demos[0]();
      
    }
    else if (json_state > 3  && json_state < 10)
    {
      demos[1]();
      
    }
    else if (json_state > 9  && json_state < 14)
    {
      demos[2]();
      
    }
    else if (json_state == 14)
    {
      demos[3]();
      
    }
    else if (json_state > 14  && json_state < 19)
    {
      demos[4]();

    }

    display.drawString(0, 40, months);
    display.drawString(27, 40, "/");
    display.drawString(33, 40, days);
    
    
    
    Middle_minutes = minutes;
  }

    display.display();
}


void setup() {
  
  Serial.begin(115200);  
  display.init();
  display.flipScreenVertically();//反向显示
  display.clear();
  demos[5]();
  display.display();
   
  initNTP(AP_SSID, AP_PSK);
 
  
  client.setTimeout(5000);//设置服务器连接超时时间
  

}
time_t previousSecond = 0;
void loop() {


  if (timeStatus() != timeNotSet) {
    if (second() != previousSecond) {
      previousSecond = second();
      updateDisplay();

    }
  }
  delay(1000);


}
