# M5Stack_Sokoban
MhageGHさんの倉庫番をM5Stackに移植させていただきました。<br><br>
オリジナルはこちら。<br>
Sokoban on esp32 and ILI9328 <https://github.com/MhageGH/esp32_ILI9328_Sokoban><br>

---
### 必要な物 ###
* [M5Stack](http://www.m5stack.com/ "Title") (Grayで動作確認をしました。)<br>
* Arduino IDE (1.8.5で動作確認をしました。)<br>
* [Arduino core for the ESP32](https://github.com/espressif/arduino-esp32 "Title")
* [M5Stack Library](https://github.com/m5stack/M5Stack.git "Title")

補足 : ArduinoIDEへのM5StackボードのインストールはM5Stackに付属の説明書の通りにやっても上手くいきませんでした。<br>
M5Stack LibraryをインストールすればボードリストにもM5Stackが現れました。<br>

![画像1](images/image1.png)<br><br><br>

### 操作方法 ###

![画像2](images/image2.png)<br>

* Aボタン：左移動<br>
* Bボタン：上移動<br>
* Cボタン：右移動<br>
* AボタンCボタン同時押し：下移動<br>
* BボタンCボタン同時押し：リスタート<br><br>
ボタンの同時押しは少し慣れが必要かもしれません。<br>
２つのボタンを少し長めに押す感じにすると上手くいくと思います。<br><br><br>
