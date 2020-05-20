# ESP8266 Convid19 Web Crawler

[![YouTube影片](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/Video_AddPlayIcon.jpg?raw=true)](https://youtu.be/YavU43Bsjqs)

影片連結: https://youtu.be/YavU43Bsjqs

---

## 1. 前言

2020年新冠病毒(Convid-16)疫情影響, 各社群對疫情的討論與創作熱絡, 恰巧某天在maker社群看見相關創作計畫 - [Taiwan can help ! + Taiwanduino](https://www.facebook.com/shijie.wei.7/posts/3080508575314757)  [[共筆筆記]](https://hackmd.io/-hK9HRlVSUSNKXRL9mUYyA?both&fbclid=IwAR1VDwCz9Ti68y5FFf8GJZLS9UgtpFDAbtoobG91PqfVfnf1hFv08fXGEn0), 這專案就這麼跟風修改做出來了。

有鑑於前個專案在電路製作上需下功夫, 這次盡量以簡單的開發板、模組, 用杜邦線串接為主

整體花費上, 硬體材料費約300以內搞定, 不包含程式環境架設的話, 製作時間1天內可搞定, 算是簡單好上手的專案。~~因人而異說不定~~

>## 專案難度: ★★☆☆☆

---

## 2. 所需材料

#### 必要
+ WeMos D1 mini開發板
+ IIC/I2C 1602 LCD模組
+ micro USB傳輸線
+ 杜邦線、排針數個
+ 紙模型
#### 非必要
+ LCD 1602 透明 壓克力 支架

---

## 3. 模型處理

### (1) 紙模型製作

此專案採用的紙模型為[Tonn Hsu 許彤 #ㄧTonn防疫美勞教室 #第一彈_阿中部長](https://www.facebook.com/tonnhsu/posts/534963517124004/), 要以簡單的模型來呈現疫情當下, CDC連日坐鎮公布最新的案例狀況。

基本的製作過程可參考創作者連結中的說明, 這邊說明幾點紙模型的簡單技巧。 建議在摺紙的過程中, 在折痕的凹陷面, 用美工刀刀背畫痕, 以求破壞紙纖維讓折線更工整漂亮。 如果害怕紙模型受潮軟化, 可在選紙時採用具基本防水性能的紙材, 以及在完成的作品噴塗保護漆。

因為需在模型內放置WeMos D1 mini開發板, 在側面記得要開孔以求micro USB頭可插入做充電傳輸。

![紙模型](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/90065485_534959493791073_6525902218690822144_o.jpg?raw=true)
>圖1. 紙模型, 圖片、模型來源 -
>**Tonn Hsu 許彤**   
>https://www.facebook.com/tonnhsu/

---

## 4. 電路製作

### (1) 規劃配線

此專題以簡化到只需接線設計好的模組, 確認程式設定和燒入無誤即可完成, 花錢買的完整模組就是這麼方便~

這邊先打個預防針, 這類套用模組的專題, 新手照步驟最容易發生的問題在幾點: 線路接錯、劣質杜邦線斷路、買錯模組和模組本身有問題。 建議在操作網路上的別人設計好的懶人專案, 還是需要了解程式內容以及驗證模組電路的可用性。

![電路圖](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/wemos_i2c_to_lcd_module_layout.PNG?raw=true)
>圖2. 電路圖, 圖像來源 -
>**Fritzing**   

這邊對模組可能要注意的地方提醒幾點:

1. LCD模組I2C接線是否正確, I2C除了電源和接地外, 還存在時脈線(SCL)和數據線(SDA), 模組時脈線對開發板時脈線、模組數據線對開發板數據線, 部分網路說法有開發板spec的數據時脈線應對調, 這和開發板製作有關, 也可作為問題發生的其中一個線路對調測試。
   
2. I2C的硬體限制, 線材長度不可過長。
   
3. LCD模組I2C地址設定, 本專案程式設定上是0x27, 但這方面設定和模組本身有關, 如模組設定位址有變, 應以模組I2C為主。
   
4. LCD模組燈光設定, 本專案採用的1602 LCD模組存在旋鈕式可調電阻可調整文字顯示明暗, 如調整錯誤, 將看無文字顯示, 下圖為筆者所調整的旋鈕角度。

![LCD Hardware Setting](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/lcd1602_setting.jpg?raw=true)
>圖3. 1602 LCD I2C 模組可調電阻設定

---

## 5. 程式燒錄

### (1) 環境建置

ESP8266的相關介紹在之前的專案已說明, 此次用的WeMos D1 mini開發板是將ESP8266的ESP-12晶片引出接腳, 並配置通訊、電源模組方便使用者插入USB即可操作編譯, 因體積小且價錢便宜, 在專案中是ESP8266的熱門開發板, 硬要說缺點就引出的接腳數少, 如不在意體積, 需要接腳數多的開發板, 可參考NodeMCU開發板。

此專案是採用ESP8266 to Arduino IDE, 將ESP8266當MCU編譯操作, 而預設的AT Command操作。需使用到第三方資源對燒錄的環境建置。

相關建置的步驟可參考網路上對 ESP8266 to Arduino IDE編譯的相關教學。詳細步驟這裡不多做敘述, 請自行參考附上的網路資源。

1.  ESP8266 to Arduino IDE編譯

+ [使用Arduino IDE 開發ESP8266 應用(一) : 環境設定與韌體上傳 - 小狐狸事務所](http://yhhuang1966.blogspot.com/2017/09/arduino-ide-esp8266.html)  
+ [[ ESP8266 & NodeMCU ] 在Arduino IDE 設定NodeMCU - Makee.io Blog](https://oranwind.org/-esp8266-nodemcu-zai-arduino-ide-she-ding-nodemcu/)
+ [Installing - ESP8266 Arduino Core documentation - Read the Docs](https://arduino-esp8266.readthedocs.io/en/latest/installing.html)
+ [Install the ESP8266 Board in Arduino IDE in less than 1 minute (Windows, Mac OS X, and Linux)](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
  
### (2) 燒錄

本專案在燒錄前, 可更改部分程式碼做自訂設定

>更改連線的 Wifi SSID
>
>```C
>#define WIFI_SSID      "TEST"
>```

>更改連線的 Wifi 登入密碼
>
>```C
>#define WIFI_PASSWORD  "12345678"
>```

>更改資料更新的間格時間(秒)
>
>```C
>#define GET_HTML_DATA_DELAYTIME  600
>```

>更改HTTPS憑證指紋
>
>```C
>const char fingerprint[] PROGMEM = "bf 9b e0 0f ab 21 50 18 5b 5b b9 8c af 13 c4 8f 93 e2 a1 b1";
>```

1. ESP8266操作程式碼燒錄，在download完本專案後, 點開"ESP8266_Convid19_WebCrawler.ino", 並執行燒錄。

補充一點, 由於此燒錄方式是在本機端重新做編譯, 請確認Arduino是否有更新ESP8266的新版本開發板套件, 以及預載用到的函式庫。開發板套件安裝路徑為「工具->開發板->開發板管理員」, 函式庫安裝路徑為「草稿碼->匯入程式庫->管理程式庫」。

```C
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClientSecureBearSSL.h>
    #include <LiquidCrystal_I2C.h> 
```

本專案採用的開發板套件版本為
+ esp8266 by ESP8266 Community version 2.7.0
  
本專案採用的library版本為
+ ESP8266WiFi Built-In by Ivan Grokhotkov version 1.0.0
+ ESP8266HTTPClient Built-In by Markus Sattler version 1.2.0
+ LiquidCrystal by Frank de Brabander version 1.1.2

相關細節設定如下圖所示。

![Upload Setting](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/upload_setting.jpg?raw=true)
>圖4. 燒錄開發板設定

---

## 6. 上電展示

完成硬體製作和程式燒錄後, 即可接上USB 5V供電確認專案完成, 上電後ESP8266會自動連線自設定的Wifi SSID, 並連線至疫情網頁, 抓取確診人數資料。
這邊就此專案程式部分提醒幾點注意的地方:

1. 此專案是以爬蟲技術去抓取[Yahoo疫情懶人包](https://news.campaign.yahoo.com.tw/2019-nCoV/index.php)的資料, 如Yahoo端更改網頁寫法或關閉頁面, 此專案將無法成功擷取資料, 如疫情仍嚴峻的當下發生類似狀況, 請聯繫作者更新或自行修改程式碼。
   
2. 程式設定預設為每10分鐘連線至網路抓取資料, 避免過於頻繁的請求資料會被認為是網路惡意攻擊, 也因此更新的數據非即時性, 存在資料更新的延時。

3. 因HTTPS網路協定, 連線所需的憑證指紋具有時效性, 需定期更新修改, 透過瀏覽器(Chrome)取得憑證指紋的方法如下圖。
   
![Get Fingerprint Step 1](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/GetFingerprintStep1.png?raw=true)
>圖5. 取得憑證指紋步驟一, 以瀏覽器(Google Chrome)連線進入要取得憑證的https網頁, 點擊網址旁的鎖頭符號(https網頁圖示為鎖頭, https網頁圖示為解鎖頭)-> 點擊憑證

![Get Fingerprint Step 2](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/GetFingerprintStep2.PNG?raw=true)
>圖6. 取得憑證指紋步驟二, 點擊跳出視窗的詳細資料-> 選擇憑證指紋-> 複製內容到程式碼內
  
4. 因網路的連線架設和品質各地不同, 如出現網路連線異常相關的錯誤, 請自行檢查網路設定。

![Final Project](https://github.com/NEXTJun/ESP8266_Convid19_WebCrawler/blob/master/src_md/picture/main.jpg?raw=true)
>圖7. 成品展示