#include <Arduino.h>
#include <Button2.h>
#include <RTClib.h>

#define PLUS_BTN_PIN D6
#define MINUS_BTN_PIN D5
#define SCL_PIN D1
#define SDA_PIN D2
#define LONGCLICK_MS 250
const TimeSpan ONE_MINUTE = TimeSpan(60);
const TimeSpan FIFTEEN_MINUTES = TimeSpan(60 * 15);
const DateTime CLOCK_EPOCH = DateTime(SECONDS_FROM_1970_TO_2000);
RTC_DS3231 rtc;
Button2 plusButton;
Button2 minusButton;
bool hasPrintedTime = false;

void printCurrentTime() {
  DateTime now = rtc.now();
  Serial.printf("Time is %02d:%02d\n", now.hour(), now.minute());
}

void handlePlusButtonClick(Button2 &plusButton) {
  if (plusButton.wasPressedFor() < plusButton.getLongClickTime()) {
    DateTime currentSetTime = rtc.now();
    rtc.adjust(currentSetTime + ONE_MINUTE);
    printCurrentTime();
  }
}

void handlePlusButtonLongClickDetected(Button2 &plusButton) {
  DateTime currentSetTime = rtc.now();
  uint8_t hours = currentSetTime.hour();
  uint8_t minutes = currentSetTime.minute();

  if (minutes >= 0 && minutes < 15) {
    minutes = 15;
  } else if (minutes >= 15 && minutes < 30) {
    minutes = 30;
  } else if (minutes >= 30 && minutes < 45) {
    minutes = 45;
  } else {
    if (hours >= 23) {
      hours = 0;
    } else {
      hours += 1;
    }
    minutes = 0;
  }
  rtc.adjust(DateTime(currentSetTime.year(), currentSetTime.month(),
                      currentSetTime.day(), hours, minutes, 0));
  printCurrentTime();
}

void handleMinusButtonClick(Button2 &minusButton) {
  if (minusButton.wasPressedFor() < minusButton.getLongClickTime()) {
    DateTime currentSetTime = rtc.now();
    rtc.adjust(currentSetTime - ONE_MINUTE);

    printCurrentTime();
  }
}
void handleMinusButtonLongClickDetected(Button2 &plusButton) {
  DateTime currentSetTime = rtc.now();
  uint8_t hours = currentSetTime.hour();
  uint8_t minutes = currentSetTime.minute();
  if (minutes <= 59 && minutes > 45) {
    minutes = 45;
  } else if (minutes <= 45 && minutes > 30) {
    minutes = 30;
  } else if (minutes <= 30 && minutes > 15) {
    minutes = 15;
  } else if (minutes <= 15 && minutes > 0) {
    minutes = 0;
  } else {
    if (hours == 0) {
      hours = 23;
    } else {
      hours -= 1;
    }
    minutes = 45;
  }
  rtc.adjust(DateTime(currentSetTime.year(), currentSetTime.month(),
                      currentSetTime.day(), hours, minutes, 0));
  printCurrentTime();
}
void setup() {
  Serial.begin(9600);
  Serial.println("starting to watch");
  plusButton.begin(PLUS_BTN_PIN);
  plusButton.setTapHandler(handlePlusButtonClick);
  plusButton.setLongClickDetectedHandler(handlePlusButtonLongClickDetected);
  plusButton.setLongClickDetectedRetriggerable(true);
  plusButton.setLongClickTime(LONGCLICK_MS);

  minusButton.begin(MINUS_BTN_PIN);
  minusButton.setTapHandler(handleMinusButtonClick);
  minusButton.setLongClickDetectedHandler(handleMinusButtonLongClickDetected);
  minusButton.setLongClickDetectedRetriggerable(true);
  minusButton.setLongClickTime(LONGCLICK_MS);

  rtc.begin();
  rtc.adjust(CLOCK_EPOCH);

  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  plusButton.loop();
  minusButton.loop();
  digitalWrite(LED_BUILTIN, LOW);
  if (millis() % 5000 == 0) {
    if (!hasPrintedTime) {

      printCurrentTime();
      hasPrintedTime = true;
    }
  } else {
    hasPrintedTime = false;
  }
}
