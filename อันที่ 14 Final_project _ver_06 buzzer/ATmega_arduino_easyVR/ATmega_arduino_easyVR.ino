  #include "Arduino.h"
#if !defined(SERIAL_PORT_MONITOR)
#endif

#if defined(SERIAL_PORT_USBVIRTUAL)
  // Shield Jumper on HW (for Leonardo and Due)
  #define port SERIAL_PORT_HARDWARE
  #define pcSerial SERIAL_PORT_USBVIRTUAL
#else

  #include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

  // Shield Jumper on SW (using pins 12/13 or 8/9 as RX/TX)
  #include "SoftwareSerial.h"

  #include "EasyVR.h"
SoftwareSerial easyvrPort(12, 13); // RX, TX สำหรับ EasyVR
EasyVR easyvr(easyvrPort);

  #define pcSerial SERIAL_PORT_MONITOR
#endif
#pragma once   

enum Groups {
  GROUP_0 = 0,
  GROUP_1 = 1,
};

enum Group0 {
  SIRI = 0,
};

enum Group1 {
  G1_ON  = 0,
  G1_OFF = 1,
  G2_ON  = 2,
  G2_OFF = 3,
  G3_ON  = 4,
  G3_OFF = 5,
  S1_ON  = 6,
  S2_ON  = 7,
  S3_ON  = 8
};

int8_t group, idx;

float melody[] = {
  659.25, 587.33, 370.00, 415.30, 
  554.37, 466.16, 261.63, 329.63, 
  440.00
};

int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};

const int buzzer = 8;

String receivedDataESP = "";
///////////////////////////// END include /////////////////////////////////

void setup() {
  Serial.begin(115200);
  easyvrPort.begin(115200);

  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  // dfPlayerPort.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  lcd.clear();
  lcd.print("Siri Test");

int mode = easyvr.bridgeRequested(pcSerial);
  switch (mode)
  {
    case EasyVR::BRIDGE_NONE:
      easyvrPort.begin(9600);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge not started!"));
      Serial.println("*** case EasyVR::BRIDGE_NONE: ***");
      break;

    case EasyVR::BRIDGE_NORMAL:
      easyvrPort.begin(9600);
      easyvr.bridgeLoop(pcSerial);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge connection aborted!"));
      Serial.println("*** case EasyVR::BRIDGE_NORMAL: ***");
      break;

    case EasyVR::BRIDGE_BOOT:
      easyvrPort.begin(115200);
      easyvr.bridgeLoop(pcSerial);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge connection aborted!"));
      Serial.println("*** case EasyVR::BRIDGE_BOOT: ***");
      break;
  }

while (!easyvr.detect()) {
    lcd.clear();
    lcd.print("NO EasyVR");
    Serial.println("EasyVR not detected!");
    delay(1000);
  }

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER;

  int size = sizeof(durations) / sizeof(int);
  // เล่นโน้ตในลูป
  for (int note = 0; note < size; note++) {
    // คำนวณระยะเวลาโน้ต
    int duration = 1000 / durations[note];
    
    // เล่นเสียงที่ความถี่จากอาเรย์ melody[] และระยะเวลา
    tone(buzzer, melody[note]);
    delay(duration);  // เล่นเสียงตามระยะเวลาที่คำนวณ
    noTone(buzzer);  // หยุดเสียง

    // หน่วงเวลาเล็กน้อยระหว่างโน้ต
    delay(50);
  }
}
///////////////////////////// END setup /////////////////////////////////

void action() {
  switch (group) {
    case GROUP_0:
      if (idx == SIRI) {
        group = GROUP_1;
        lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่
        lcd.setCursor(0, 0);
        lcd.print("Siri Hi");
        tone(buzzer,  1046.50   ); // Send 1KHz sound signal...
        delay(200);  
        noTone(buzzer);
        tone(buzzer,  523.25  );
        delay(200);  
        noTone(buzzer);
        tone(buzzer, 1174.66 );
        delay(200);  
        noTone(buzzer);
        tone(buzzer, 698.46 );
        noTone(buzzer);
      }
      break; 

    case GROUP_1:
       lcd.clear();
       if (idx == G1_ON) {
        Serial.println("G1_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 01");
        lcd.setCursor(0, 1);
        lcd.print("....");

      } else if (idx == G1_OFF) {
        Serial.println("G1_OFF");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 01");
        lcd.setCursor(0, 1);
        lcd.print("....");

      } else if (idx == G2_ON) {
        Serial.println("G2_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 02");
        lcd.setCursor(0, 1);
        lcd.print("....");

      } else if (idx == G2_OFF) {
        Serial.println("G2_OFF");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 02");
        lcd.setCursor(0, 1);
         lcd.print("....");
 
      } else if (idx == S1_ON) {
        Serial.println("S1_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Sensor");
        lcd.setCursor(0, 1);
        lcd.print("DS18B20...");

      } else if (idx == S2_ON) {
        Serial.println("S2_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Sensor");
        lcd.setCursor(0, 1);
        lcd.print("DHT11....");

      } 
      break; 
  }
}
///////////////////////////// END action /////////////////////////////////

void loop() {
  // Serial.println("Now in loop...");
  
  easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)
  easyvr.recognizeCommand(group); // Recognition of a built-in word.

  do
  {
     // สามารถทำการประมวลผลอื่นๆ ระหว่างรอคำสั่ง
  }
  while (!easyvr.hasFinished()); // Polls the status of ongoing recognition
  easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

  easyvr.setMicDistance(3); // Set mic operating distance. 1 nearest - 3 farthest.
  easyvr.setLevel(2); // Set strictness level for recognition of custom commands.
  easyvr.setKnob(2); // Set confidence threshold for recognition of built-in words.

  idx = easyvr.getWord(); // Get recognized word index from built-in sets or custom grammars.
  if (idx >= 0)
  {
    return;
  } // end if.

  idx = easyvr.getCommand(); 

 if (idx >= 0)
  {
    // print debug message
    uint8_t train = 0;
    char name[32];
    if (easyvr.dumpCommand(group, idx, name, train)) // Retrieves the name and training data of a custom command.
    {
    } // end if.

    else
      easyvr.playSound(0, EasyVR::VOL_FULL); // Play bell sound.

    // perform some action
    action(); // Turn LED on or off.
    
  }

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    lcd.clear();
    if (inChar == '\n') {
      receivedDataESP.trim();
      lcd.setCursor(0, 0);

       if (receivedDataESP.startsWith("DHT11")) {
        lcd.setCursor(0, 0);
        lcd.print("DHT11");

        lcd.setCursor(0, 1);
        lcd.print("T:" + receivedDataESP.substring(22, 27) + "\xDF""C");
        lcd.print(" H:" + receivedDataESP.substring(44, 46 ) + "%"); 

      } else if (receivedDataESP.startsWith("DS18B20")) {
        lcd.setCursor(0, 0);
        lcd.print("DS18B20");

        lcd.setCursor(0, 1);
        lcd.print("T:" + receivedDataESP.substring(24, 29) + "\xDF""C"); 
       

      } else if (receivedDataESP.startsWith("M1_ON")) {
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 01");
        lcd.setCursor(0, 1);
        lcd.print("ON");
        
        tone(buzzer, 200); 
          delay(200);
          noTone(buzzer);  
        tone(buzzer, 500); 
          delay(200);
          noTone(buzzer); 
        tone(buzzer, 1000);
          delay(200);
          noTone(buzzer); 

      } else if (receivedDataESP.startsWith("M1_OFF")) {
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 01");
        lcd.setCursor(0, 1);
        lcd.print("OFF");

        tone(buzzer, 1000); 
          delay(200);  
          noTone(buzzer);
          delay(200);  
        tone(buzzer, 1000);
          delay(200);  
          noTone(buzzer);

      } else if (receivedDataESP.startsWith("M2_ON")) {
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 02");
        lcd.setCursor(0, 1);
        lcd.print("ON");

        tone(buzzer, 200); 
          delay(200);
          noTone(buzzer);  
        tone(buzzer, 500); 
          delay(200);
          noTone(buzzer); 
        tone(buzzer, 1000);
          delay(200);
          noTone(buzzer); 
      
      } else if (receivedDataESP.startsWith("M2_OFF")){
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 02");
        lcd.setCursor(0, 1);
        lcd.print("OFF");

        tone(buzzer, 1000); // Send 1KHz sound signal...
          delay(200);  
          noTone(buzzer);
          delay(200);  
        tone(buzzer, 1000);
          delay(200);  
          noTone(buzzer);
      
      } else if (receivedDataESP.startsWith("TimeOut")){
        lcd.setCursor(0, 0);
        lcd.print("Siri TimeOut");

        tone(buzzer, 1000); // Send 1KHz sound signal...
          delay(3000);  
          noTone(buzzer);

          lcd.clear();
          if (group == GROUP_0) {
              lcd.setCursor(0, 0);
              lcd.print("Siri Test");

          } else if (group == GROUP_1) {
              lcd.setCursor(0, 0);
              lcd.print("Siri Hi");
          }
      }

      receivedDataESP = "";

    } else {
      receivedDataESP += inChar;
    }
  }
  delay(500);
}

///////////////////////////// END loop /////////////////////////////////

