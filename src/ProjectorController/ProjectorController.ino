// Operating mode - recording or normal use
// #define OPERATING_MODE_RECORDING

#define IR_SEND_PIN 3
#define IR_RECEIVE_PIN 10

#include <IRremote.hpp>

const int buttonPin = 2;
const int ledPin = 13;
const int repeats = 5;

int photocellPin = 0;
int photocellReading;
int photocellBackgroundReading = 0;

// Last recorded state
// 0 - off
// 1 - on
bool currentState = 0;

void blinkLed() {
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
}

void blinkLed(uint8_t n) {
  for (uint8_t i=0; i<n; i++) {
    blinkLed();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  IrSender.begin(DISABLE_LED_FEEDBACK, 0);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.print(F("at pin "));
  Serial.println(IR_RECEIVE_PIN);
}

void loop() {
  #if defined(OPERATING_MODE_RECORDING)
  // The code in this #if block is based on SimpleReceiver by Armin Joachimsmeyer et al., licensed under the MIT License.
  // https://github.com/Arduino-IRremote/Arduino-IRremote/blob/1c84ee96680eadc0563659babb913915df477171/examples/SimpleReceiver/SimpleReceiver.ino#L1
  //
  // MIT License
  //
  // (c) Copyright 2009 Ken Shirriff http://www.righto.com
  // (c) Copyright 2016 Rafi Khan
  // (c) Copyright 2020-2022 Armin Joachimsmeyer et al.
  //
  //     http://www.opensource.org/licenses/mit-license.php
  // 
  // Permission is hereby granted, free of charge, to any person obtaining a copy
  // of this software and associated documentation files (the "Software"), to deal
  // in the Software without restriction, including without limitation the rights
  // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  // copies of the Software, and to permit persons to whom the Software is
  // furnished to do so, subject to the following conditions:
  // 
  // The above copyright notice and this permission notice shall be included in all
  // copies or substantial portions of the Software.
  // 
  // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  // SOFTWARE.

  /*
  * Check if received data is available and if yes, try to decode it.
  * Decoded result is in the IrReceiver.decodedIRData structure.
  *
  * E.g. command is in IrReceiver.decodedIRData.command
  * address is in command is in IrReceiver.decodedIRData.address
  * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
  */
  if (IrReceiver.decode()) {

    /*
    * Print a summary of received data
    */
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
      // We have an unknown protocol here, print extended info
      IrReceiver.printIRResultRawFormatted(&Serial, true);

      IrReceiver.resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
    } else {
      IrReceiver.resume(); // Early enable receiving of the next IR frame

      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.printIRSendUsage(&Serial);
    }
    Serial.println();

    /*
    * Finally, check the received data and perform actions according to the received command
    */
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      Serial.println(F("Repeat received. Here you can repeat the same action as before."));
    } else {
      if (IrReceiver.decodedIRData.command == 0x10) {
        Serial.println(F("Received command 0x10."));
        // do something
      } else if (IrReceiver.decodedIRData.command == 0x11) {
        Serial.println(F("Received command 0x11."));
        // do something else
      }
    }
  }

  #else  // Normal loop operation

  delay(500);

  bool buttonState = digitalRead(buttonPin);
  if (buttonState == currentState) {
    return;
  }

  if (buttonState == LOW) {
    // OFF Sequence - presses POWER and confirms the device shutdown dialog
    // POWER, OK

    currentState = 0;

    blinkLed(2);
    Serial.println("Turning OFF...");
    
    // POWER
    blinkLed();
    IrSender.sendNEC(0x281, 0x1, repeats);
    delay(500);
    
    // OK
    blinkLed();
    IrSender.sendNEC(0x281, 0x5, repeats);
    delay(500);

    blinkLed(5);
    Serial.println("Sequence sent");
  }

  if (buttonState == HIGH) {
    // ON sequence - turns ON the device and selects the HDMI input device
    // POWER, (wait for the device to turn on), (if long boot) HOME, UP, DOWN, OK (else) nothing

    currentState = 1;
    blinkLed(2);
    
    Serial.println("Turning ON...");
    Serial.println("Calibrating sensor...");

    for (int i=0; i<5; i++) {
      photocellReading = analogRead(photocellPin);
      photocellBackgroundReading = ((photocellBackgroundReading*i) + photocellReading)/(i+1);
      delay(100);
    }

    Serial.println("Sensor calibrated");
    Serial.print("Background reading: ");
    Serial.println(photocellBackgroundReading);
    
    // POWER
    blinkLed();
    IrSender.sendNEC(0x281, 0x1, repeats);

    int powerOnDelay = 0;
    int photocellThreshold = photocellBackgroundReading+300;
    bool lightDetected = false;
    
    for (int i=0; i<40; i++) {  // we sleep 500ms per loop, so this waits up to 20 seconds
      delay(500);
      powerOnDelay += 500;
      photocellReading = analogRead(photocellPin);

      Serial.print("Photocell reading: ");
      Serial.print(photocellReading);
      Serial.print(" / ");
      Serial.println(photocellThreshold);

      if (photocellReading > photocellThreshold) {
        Serial.println("Threshold passed");
        lightDetected = true;
        break;
      }
    }

    if (!lightDetected) {
      blinkLed(2);
      delay(500);
      blinkLed(2);
      delay(500);
      blinkLed(2);
      Serial.println("Error, light not detected.");
    } else {
      if (powerOnDelay > 10000) {
        Serial.println("Long boot detected. Sending long sequence.");
        
        // HOME
        blinkLed();
        IrSender.sendNEC(0x281, 0x9, repeats);
        delay(1000);
        
        // UP
        blinkLed();
        IrSender.sendNEC(0x281, 0x3, repeats);
        delay(500);
        
        // DOWN
        blinkLed();
        IrSender.sendNEC(0x281, 0x7, repeats);
        delay(500);

        // OK
        blinkLed();
        IrSender.sendNEC(0x281, 0x5, repeats);
        delay(500);
      } else {
        Serial.println("Short boot detected. Doing nothing.");
      }
    }
  
    blinkLed(5);
    Serial.println("Sequence sent.");
  }
  #endif
}
