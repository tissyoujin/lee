#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int A = 0;
int B = 0;
int sw1 = 6;
int sw2 = 7;
int start = 8;
int led = 13;

bool gameRunning = false;
bool ledOn = false;
unsigned long ledOnTime;
unsigned long randomDelay;
unsigned long ledDuration; // LED가 켜져 있는 시간

bool lastButtonState1 = HIGH; // 이전 sw1 상태
bool lastButtonState2 = HIGH; // 이전 sw2 상태

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);
  pinMode(start, INPUT_PULLUP);
  
  randomSeed(analogRead(0));
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Press Start");
}

void loop() {
  if (digitalRead(start) == LOW && !gameRunning) {
    startGame();
  }

  if (gameRunning) {
    unsigned long currentTime = millis();

    if (!ledOn && currentTime - ledOnTime > randomDelay) {
      digitalWrite(led, HIGH);
      ledOn = true;
      ledOnTime = currentTime; // LED 켜진 시간 기록
      ledDuration = random(500, 500); // LED가 켜져 있는 시간 (1초 ~ 3초 랜덤)
    }

    if (ledOn && currentTime - ledOnTime > ledDuration) {
      endRound();
    }

    if (ledOn) {
      // 버튼 상태 변동 확인 및 디바운싱
      bool buttonState1 = digitalRead(sw1);
      bool buttonState2 = digitalRead(sw2);

      if (buttonState1 != lastButtonState1) {
        if (buttonState1 == LOW && millis() - lastDebounceTime1 > debounceDelay) {
          if (buttonState2 != LOW) { // sw2가 동시에 눌리지 않았을 때만 A 점수 증가
            A++;
            lcd.setCursor(8, 0);
            lcd.print(A);
          }
          lastDebounceTime1 = millis();
        }
        lastButtonState1 = buttonState1;
        endRound();
      }

      if (buttonState2 != lastButtonState2) {
        if (buttonState2 == LOW && millis() - lastDebounceTime2 > debounceDelay) {
          if (buttonState1 != LOW) { // sw1가 동시에 눌리지 않았을 때만 B 점수 증가
            B++;
            lcd.setCursor(8, 1);
            lcd.print(B);
          }
          lastDebounceTime2 = millis();
        }
        lastButtonState2 = buttonState2;
        endRound();
      }
    } else { // LED가 꺼져 있을 때의 상태 확인
      bool buttonState1 = digitalRead(sw1);
      bool buttonState2 = digitalRead(sw2);

      if (buttonState1 != lastButtonState1) {
        if (buttonState1 == LOW && millis() - lastDebounceTime1 > debounceDelay) {
          if (A > 0) A--; // 점수가 0 이상일 때만 감소
          lcd.setCursor(8, 0);
          lcd.print(A);
          lastDebounceTime1 = millis();
        }
        lastButtonState1 = buttonState1;
      }

      if (buttonState2 != lastButtonState2) {
        if (buttonState2 == LOW && millis() - lastDebounceTime2 > debounceDelay) {
          if (B > 0) B--; // 점수가 0 이상일 때만 감소
          lcd.setCursor(8, 1);
          lcd.print(B);
          lastDebounceTime2 = millis();
        }
        lastButtonState2 = buttonState2;
      }
    }

    if (A == 5 || B == 5) {
      displayWinner();
      gameRunning = false;  // 게임을 종료합니다.
    }
  }
}

void startGame() {
  gameRunning = true;
  A = 0;
  B = 0;
  ledOn = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A_SCORE=0");
  lcd.setCursor(0, 1);
  lcd.print("B_SCORE=0");
  randomDelay = random(500, 5000);
  ledOnTime = millis();
}

void endRound() {
  digitalWrite(led, LOW);
  ledOn = false;
  randomDelay = random(500, 5000);
  ledOnTime = millis();
}

void displayWinner() {
  lcd.clear();
  if (A == 5) {
    lcd.setCursor(0, 0);
    lcd.print("A Wins!");
  } else if (B == 5) {
    lcd.setCursor(0, 0);
    lcd.print("B Wins!");
  }
  lcd.setCursor(0, 1);
  lcd.print("Press Start");
}