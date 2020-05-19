#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <LiquidCrystal_I2C.h> 
#include <Wire.h> 

#define LED_BLINK       D4
#define SERIAL_SPEED    115200
#define WIFI_SSID       "TEST"
#define WIFI_PASSWORD   "12345678"
#define GET_HTML_DATA_DELAYTIME  600        // each 600 second catch web data
#define GET_HTML_DATA_ERROR_DELAYTIME  60   // if error happened, re-try durning 60 second
#define LCD_COLUMNS     16
#define LCD_ROWS        2
#define LCD_DISPLAY_DURNING_TIME  5 //second
#define CACHE_DATA_NUM   3

/*--- Wifi Setting Parameter ---*/
WiFiClientSecure client;
const String host = "https://news.campaign.yahoo.com.tw/2019-nCoV/index.php";
const char fingerprint[] PROGMEM = "bf 9b e0 0f ab 21 50 18 5b 5b b9 8c af 13 c4 8f 93 e2 a1 b1"; // Expiration date: 2020/10/17, use web browser to view and copy

/*--- Hardware Setting Parameter ---*/
LiquidCrystal_I2C lcd(0x27, LCD_COLUMNS, LCD_ROWS);
int led_status;

/*--- Crawler Setting Parameter ---*/
String data_buffer[CACHE_DATA_NUM];
int count[CACHE_DATA_NUM] = {0};
const int target_count[CACHE_DATA_NUM] = {1, 1, 1};
const String keyword[CACHE_DATA_NUM] = {"<p class=\"current\">", "<span class=\"no\">", "<span class=\"time\">"};
const String keyword_end[CACHE_DATA_NUM] = {"</p>", "</span>", "</span>"};
bool keyword_catch_enable[CACHE_DATA_NUM] = {0};

/*--- Others Parameter ---*/
unsigned long time_tick = 0;
bool time_init = true;

/*---------- Program Execution ----------*/
void setup() {
  initSerialConfig();
  initLedPinConfig();
  initLcdConfig();
  initWifiConfig();
  delay(5000);
}

void loop() {
  while(millis() - time_tick > GET_HTML_DATA_DELAYTIME * 1000 || time_init == true) {
    
    if(led_status == LOW)
      led_status = HIGH;
    else if(led_status == HIGH)
      led_status = LOW;
    digitalWrite(LED_BLINK, led_status);
  
    if(getWebHtmlData((String*)&data_buffer)) {
      Serial.printf("Now MCU Time : %d \n", millis());
      Serial.println("Total :");
      Serial.println(data_buffer[0]);
      Serial.println("Today :");
      Serial.println(data_buffer[1]);
      Serial.println("Time :");
      Serial.println(data_buffer[2]);
      time_tick = millis();
      if(time_init == true)
        time_init = false;
    } else {      
      displayErrorToLcd((String*)&data_buffer);
      delay(GET_HTML_DATA_ERROR_DELAYTIME);
    }
  }
  displayToLcd((String*)&data_buffer);
}

/*---------- Function Prototypes ----------*/

  /*-- Init Function --*/
  
/* Init Serial Baud Rate */
bool initSerialConfig() {
  Serial.printf("Setting Serial configuration ... ");
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Ready \n");
  return true;
}

/* Init LED Pin Setting */
bool initLedPinConfig() {
  Serial.printf("Setting LED configuration ... ");
  led_status = LOW;
  digitalWrite(LED_BLINK, led_status);
  Serial.printf("Ready \n");
  return true;
}

/* Init LCD Pin Setting */
bool initLcdConfig() {
  Serial.printf("Setting LCD configuration ... ");
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello, World!"); 
  Serial.printf("Ready \n");
  return true;
}

/* Init Wifi Setting */
bool initWifiConfig() {
  Serial.printf("Setting Wifi configuration Start \n");
  WiFi.mode(WIFI_STA);
  Serial.printf("Wifi Connecting to %s \n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {  // wait connected
    delay(100);
    Serial.printf(".");
  }
  Serial.printf("\n");
  Serial.printf("Wifi connected! \n");
  Serial.printf("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.printf("\n");
  Serial.printf("Setting Wifi configuration Success \n");
  return true;
}

  /*-- Work Function --*/
bool getWebHtmlData(String *data) {
  client.setFingerprint(fingerprint);
  HTTPClient https;
  
  if ((WiFi.status() == WL_CONNECTED)) {
    Serial.printf("[HTTP] begin... \n");
    Serial.printf("[HTTP] connect to ");
    Serial.print(host);
    Serial.printf("\n");
    if (https.begin(client, host)) {  // HTTP
      int httpCode = https.GET();
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        getCrawlerData(data);
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        *(data) = https.errorToString(httpCode).c_str();
        return false;
      }
      https.end();
    } else {
      Serial.printf("[HTTP] Unable to connect\n");
      *(data) = "Wifi Unconnect";
      return false;
    }
  }
  return true;
}

void getCrawlerData(String *data) {
  memset(count, 0, CACHE_DATA_NUM);
  
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    for(int i = 0; i < CACHE_DATA_NUM; i++) {
      if(count[i] < target_count[i]) {
        if(line.indexOf(keyword[i]) > 0) {          
          if(line.indexOf(keyword_end[i]) > 0) {
            *(data+i) = line.substring(line.indexOf(keyword[i]) + keyword[i].length(), line.indexOf(keyword_end[i]));
            keyword_catch_enable[i] = false;
            count[i]++;
          } else {
            *(data+i) = line.substring(line.indexOf(keyword[i]) + keyword[i].length());
            keyword_catch_enable[i] = true;
          }
        } else if(keyword_catch_enable[i] == true) {
          if(line.indexOf(keyword_end[i]) > 0) {
            *(data+i) = *(data+i) + line.substring(0, line.indexOf(keyword_end[i]));
            keyword_catch_enable[i] = false;
            count[i]++;
          } else {
            *(data+i) = *(data+i) + line;
          }
        }
      }
    }
    if(checkAllCountMatchTarget())
      break;
  }
}

bool checkAllCountMatchTarget() {
  bool result = true;
  for(int i = 0; i < CACHE_DATA_NUM; i++) {
    if(count[i] != target_count[i])
      return false;
  }
  return result;
}

void displayToLcd(String *data) {
  String line[LCD_ROWS];
  
  lcd.clear();
  line[0] = "Taiwan COVID-19";
  lcd.setCursor(0, 0);
  lcd.print(line[0]);
  line[1] = "News";
  lcd.setCursor(LCD_COLUMNS - line[1].length(), 1);
  lcd.print(line[1]);
  delay(LCD_DISPLAY_DURNING_TIME * 1000);
  
  lcd.clear();
  line[0] = "Total Cases :";
  lcd.setCursor(0, 0);
  lcd.print(line[0]);
  line[1] = *(data);
  lcd.setCursor(LCD_COLUMNS - line[1].length(), 1);
  lcd.print(line[1]);
  delay(LCD_DISPLAY_DURNING_TIME * 1000);
  
  lcd.clear();
  line[0] = "Recent Cases :";
  lcd.setCursor(0, 0);
  lcd.print(line[0]);
  line[1] = *(data + 1);
  lcd.setCursor(LCD_COLUMNS - line[1].length(), 1);
  lcd.print(line[1]);
  delay(LCD_DISPLAY_DURNING_TIME * 1000);
  
  lcd.clear();
  line[0] = "Update Time :";
  lcd.setCursor(0, 0);
  lcd.print(line[0]);
  line[1] = *(data + 2);
  replaceDateFormat(&line[1]);
  lcd.setCursor(LCD_COLUMNS - line[1].length(), 1);
  lcd.print(line[1]);
  delay(LCD_DISPLAY_DURNING_TIME * 1000);
  
  lcd.clear();  
  line[0] = "Taiwan";
  lcd.setCursor(0, 0);
  lcd.print(line[0]);
  line[1] = "Can Help!";
  lcd.setCursor(LCD_COLUMNS - line[1].length(), 1);
  lcd.print(line[1]);
  delay(LCD_DISPLAY_DURNING_TIME * 1000);
}

void replaceDateFormat(String *data) {
  // "2020年5月5日12時34分" -> "2020-5-5-12:34"
  String tmp = *data;
  tmp.replace("年", "-");
  tmp.replace("月", "-");
  tmp.replace("日", "-");
  tmp.replace("時", ":");
  tmp.replace("分", "");
  tmp.trim();
  *data = tmp;
}

void displayErrorToLcd(String *data) {
  String tmp = "[Error] " + *data;
  String line[LCD_ROWS];
  
  lcd.clear();
  for(int i = 0; i < LCD_ROWS; i++) {
    lcd.setCursor(0, i);
    line[i] = tmp.substring(LCD_COLUMNS * i, LCD_COLUMNS * (i + 1));
    lcd.print(line[i]);
  }
}
