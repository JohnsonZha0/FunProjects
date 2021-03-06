// ----------------------------------------------------------------------------
// elecKeyboard.ino
//
 // Created 2015-11-04
 // By seesea <seesea2517#gmail#com>
 //
 // 模拟电子琴
 // 七个按键为 1 2 3 4 5 6 7
 // 加两个按键控制升降一个八度
 // ----------------------------------------------------------------------------
 
 #include "pitches.h"
 
 #define BASS 0  // 低音
 #define ALTO 1  // 中音
 #define ALTA 2  // 高音
 
 byte keyOttavaBassa = 2;                    // 低八度变换按键
 byte keyOttavaAlta  = 3;                    // 高八度变换按键
 byte keyboard[] = { 4, 5, 6, 7, 8, 9, 10 }; // 七个音阶键
 byte pinBuzzer  = 12;                       // 蜂鸣器
 byte pinLED     = 13;                       // 灯光显示
 
 char keyNum = sizeof(keyboard) / sizeof(keyboard[0]);
 
 unsigned int notes[][7] = {
         { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4 },  // 低音
         { NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5 },  // 中音
         { NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6 },  // 高音-------
     };
 
 void setup()
 {
     pinMode(keyOttavaBassa, INPUT_PULLUP);
     pinMode(keyOttavaAlta,  INPUT_PULLUP);
     for (char i = 0; i < keyNum; ++i)
     {
         pinMode(keyboard[i], INPUT_PULLUP);
     }
 
     pinMode(pinLED, OUTPUT);
 }
 
 void loop()
 {
     int key = getKeyDown();
 
     if (key < 0)
     {
         noTone(pinBuzzer);
         digitalWrite(pinLED, LOW);
         return;
     }
 
     tone(pinBuzzer, notes[getToneLevel()][key]);
     digitalWrite(pinLED, HIGH);
 }
 
 // 根据音调按键来获得当前的音调高度
 char getToneLevel()
 {
     if (digitalRead(keyOttavaBassa) == LOW)
         return BASS;
 
     if (digitalRead(keyOttavaAlta) == LOW)
         return ALTA;
 
     return ALTO;
 }
 
 // 获取按下的音阶按键
 // 以 0 - 6 为音阶 1 - 7
 // 没有按下的话返回 -1
 char getKeyDown()
 {
     for (char i = 0; i < keyNum; ++i)
     {
         if (digitalRead(keyboard[i]) == LOW)
             return i;
     }
 
     return -1;
 }
