#include <DFRobotDFPlayerMini.h>    // MP3 모듈 라이브러리
#include <SoftwareSerial.h>         // 소프트웨어 시리얼 라이브러리
#include <Wire.h>                   // I2C 통신 라이브러리
#include <Adafruit_TCS34725.h>      // TCS34725 RGB 센서 라이브러리
#include <Adafruit_GFX.h>           // 그래픽 라이브러리
#include <MCUFRIEND_kbv.h>          // 3.5인치 TFT LCD 라이브러리

// 핀 설정
const int buttonPin = 53; 
SoftwareSerial mp3_serial(34, 35);  // MP3 모듈 RX, TX 핀 지정
DFRobotDFPlayerMini mp3;            // MP3 객체 생성
Adafruit_TCS34725 tcs[6];           // TCS34725 RGB 센서 배열 (6개)
MCUFRIEND_kbv tft;                  // TFT LCD 객체 생성

// LCD 핀 설정
const int SD_SCK = 13;
const int SD_D0 = 12;
const int SD_DI = 11;
const int SD_SS = 10;
const int LCD_D0 = 8;
const int LCD_D1 = 9;
const int LCD_D2 = 2;
const int LCD_D3 = 3;
const int LCD_D4 = 4;
const int LCD_D5 = 5;
const int LCD_D6 = 6;
const int LCD_D7 = 7;
const int LCD_RD = A0;
const int LCD_WR = A1;
const int LCD_RS = A2;
const int LCD_CS = A3;
const int LCD_RST = A4;
const int F_CS = A5;

// 모터 핀 설정
int motorPins[6][3] = {
  {38, 39, A6},  // 모터1
  {40, 41, A7},  // 모터2
  {42, 43, A8},  // 모터3
  {44, 45, A9},  // 모터4
  {46, 47, A10}, // 모터5
  {48, 49, A11}  // 모터6
};

// 점수 설정
int scores[6] = {5, 10, 15, 20, 25, 30}; // 각 출구의 점수
int player1Score = 0;                    // Player 1 총점
int player2Score = 0;                    // Player 2 총점

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // 버튼 핀을 입력으로 설정하고 풀업 저항 활성화
  
  // 버튼 대기 루프: 버튼을 누를 때까지 대기
  while (digitalRead(buttonPin) == HIGH) {
    Serial.println("Press the button to start setup");
    delay(100);
  }
  Serial.begin(9600); // 시리얼 통신 초기화
  mp3_serial.begin(9600); // MP3 모듈 통신
  mp3.begin(mp3_serial);   // MP3 모듈 초기화
  delay(1000);
  
  mp3.volume(20); // 볼륨 설정
  mp3.loop(1);    // 첫 번째 mp3파일 무한 재생

  // TFT LCD 초기화
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);  // 가로 방향
  tft.fillScreen(TFT_BLACK); // 화면 전체를 검은색으로 채움

  // TCA9548A I2C 멀티플렉서 초기화 (SDA: 20, SCL: 21 사용)
  Wire.begin();  // i2c 통신 초기화

  // RGB 센서 초기화
  for (int i = 0; i < 6; i++) {
    int port = (i == 2) ? 5 : i;  // 센서 인덱스 2는 포트 6번 사용
    selectMuxPort(port);
    tcs[i] = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X); // 감도 16배 설정
    if (tcs[i].begin()) {
      Serial.print("RGB 센서 ");
      Serial.print(i);
      Serial.println(" 초기화 성공");
    } else {
      Serial.print("RGB 센서 ");
      Serial.print(i);
      Serial.println(" 초기화 실패");
    }
  } 

  // 모터 핀 초기화
  for (int i = 0; i < 6; i++) {
    pinMode(motorPins[i][0], OUTPUT);
    pinMode(motorPins[i][1], OUTPUT);
    pinMode(motorPins[i][2], OUTPUT);
    activateObstacle(i);  // 모터를 계속 회전시킴
  }
  displayScores();  // 초기 점수 화면 표시
}

void loop() {
  static unsigned long gameStartTime = millis();
  const unsigned long gameDuration = 60000; // 60초 게임 지속

  if (millis() - gameStartTime >= gameDuration) {
    displayGameOver();

    // 버튼을 누르면 게임 재시작
    while (digitalRead(buttonPin) == HIGH);
    delay(500);
    player1Score = 0;
    player2Score = 0;
    displayScores();
    gameStartTime = millis();
  }

  for (int i = 0; i < 5; i++) {
    int port = (i == 2) ? 5 : i;
    selectMuxPort(port);
    uint16_t r, g, b, c;
    tcs[i].getRawData(&r, &g, &b, &c);

    Serial.print("센서 ");
    Serial.print(i);
    Serial.print(" - R: ");
    Serial.print(r);
    Serial.print(" G: ");
    Serial.print(g);
    Serial.print(" B: ");
    Serial.print(b);
    Serial.print(" C: ");
    Serial.println(c);

    if (detectBall(r, g, b, i)) {
      displayScores(); // 점수 갱신 후 표시
    }
  }
  delay(1000); // 각 센서 측정 후 1초 지연
}

// RGB 센서 멀티플렉서 포트 선택
void selectMuxPort(int port) { 
  Wire.beginTransmission(0x70); // TCA9548A의 기본 I2C 주소
  Wire.write(1 << port);  // 포트 선택
  Wire.endTransmission();
}

// 공을 감지하는 함수 (주황색과 흰색 범위에 따른 점수 추가)
bool detectBall(uint16_t r, uint16_t g, uint16_t b, int sensorIndex) {
  int score = scores[sensorIndex];
  
  // 주황색 공 감지 (PLAYER 1)
  if  (r > 1500 && g < 1000 && b < 800){
    player1Score += score;
    return true;
  }
  
  // 흰색 공 감지 (PLAYER 2)
  if (r > 2000 && g > 2000 && b > 2000){
    player2Score += score;
    return true;
  }
  
  return false;
}

// 각 플레이어의 총점을 TFT LCD에 출력하는 함수
void displayScores() {
  tft.fillRect(0, 0, 320, 80, TFT_BLACK); // 점수 출력 영역 지우기
  tft.setTextSize(4);
  tft.setCursor(20, 20);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print(player1Score);
  tft.print("POINT");

  tft.setCursor(20, 50);
  tft.print("PLAYER 2: ");
  tft.print(player2Score);
  tft.print("POINT");
}

// 장애물 모터를 계속 작동시키는 함수
void activateObstacle(int motorIndex) {
  digitalWrite(motorPins[motorIndex][0], HIGH);
  digitalWrite(motorPins[motorIndex][1], LOW);
  analogWrite(motorPins[motorIndex][2], 200);  // PWM 속도 설정 (200으로 계속 회전)
}

// 게임 종료 후 결과를 표시하는 함수
void displayGameOver() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(4);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setCursor(60, 100);
  tft.print("Game Over!");
  
  tft.setTextSize(3);
  tft.setCursor(50, 150);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print("Final Score");

  tft.setCursor(50, 200);
  tft.print("P1: ");
  tft.print(player1Score);

  tft.setCursor(50, 250);
  tft.print("P2: ");
  tft.print(player2Score);
}


