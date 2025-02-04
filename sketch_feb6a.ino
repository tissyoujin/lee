#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
Liquidcrystal_I2C 1cd(0x27, 16, 2);

int sensorPin = AD;//센서의 아날로그 출력 핀
int buzzer = 11;//부저 피
int toneDuration = 500;//부저 소리 지속 시간

int redPin = 9;//빨강 LED 피
int greenPin = 7; // 초록 LED 핀 
int bluepin = 8; // 파랑 LED 핀
int switchPin = 2; // 스위치 핀 (인터럽트 핀)
volatile int switch_state;
int freAnt = 262;//도
int freLow = 294; //레
int freMid = 330;//미
int freigh = 349;// 파
int freover = 392;//솔

int thresholdoml = 0;
int threshold100ml = 150;//100ml에 해당하는 센서 값 임계값
int threshold300ml = 350;//300ml에 해당하는 센서 값 임계값 
int threshold500ml = 500;//500ml에 해당하는 센서 값 임계값
int overover = 650;
 
bool reached100ml = false;
bool reached300ml = false;
bool reached500ml = false;
bool reachedoverover = false;
bool waterstarted = false; // 물 측정이 시작되었는지 여부

void setup () {
pinMode (buzzer, OUTPUT);//부저 핀 출력 설정
digitalWrite (buzzer, LOW);//초기 상태 설정

pinMode (redPin, OUTPUT); 
pinMode (greenPin, OUTPUT);
pinMode (bluePin, OUTPUT);
pinMode(switchPin, INPUT _PULLUP; // 스위치 핀

attachInterrupt(digitalpinTointerrupt(switchpin), mjmjmj, CHANGE); // 인터럽트 설정

lcd.init ();//LCD 초기화
lcd.backlight);//LCD 백라이트 켜기
lcd.setcursor (0, 0);//LCD 커서 위치 설정
lcd.print("startHanda");//시작한다 출력
 
 delay(1000);
}

void loop () {
int sensorvalue = analogRead(sensorpin); // 센서 값 읽기
//개미눈물 만큼
if (sensorValue > thresholdoml && threshold100ml > sensorValue && !waterstarted) {
  tone (buzzer, freant, toneDuration);
  lcd. clear(); // LCD 초기화 
  delay(100); // 지연지연
  lcd.setCursor (0, 0);
  lcd.print("ant tears");
  waterstarted = true;
}
// 100ml
if (threshold300ml > sensorValue && sensorValue ›= threshold100ml && !reached100ml) {
  tone (buzzer, freLow, toneDuration);
  lcd.clear();
  delay(100);
  lcd.setcursor(O, 0);
  lcd.print("Reached 100ml");
  reached100ml = true;
}
// 300ml
if (threshold500ml > sensorValue && sensorValue ›= threshold300ml && !reached300ml) { 
  tone (buzzer, freMid, toneDuration);
  lcd.clear());
  delay (100);
  lcd.setCursor (0, 0);
  lcd.print("Reached 300ml");
  reached300ml = true;
}

//500ml
if (overover > sensorValue && sensorValue ›= threshold500ml && !reached500ml) { tone (buzzer, freHigh, toneDuration);
  lcd. clear(); // LCD 초기화
  delay (100);
  lcd.setcursor (O, 0);
  lcd.print("Reached 500ml ");
  reached500ml = true;
}
// 500이상
if (sensorValue ›= overover && !reachedoverover) { 
  tone (buzzer, freover, toneDuration);
  lcd.clear ; // LCD초기화
  delay (100);
  lcd.setCursor (0, 0);
  lcd.print (”danger”); // 넘쳐흘러
  reachedoverOver = true;
}
delay (1000);
}

void setColor (int red, int green, int blue) {
  analogWrite (redPin, red); //빨강
  analogwrite(greenpin, green); // 초록 
  analogwrite(bluepin, blue); // 파랑
}

void mimimi() {
  switch_state = digitalRead(switchPin);

  if (switch state == HIGH) {
    setColor (0, 0, 0); // LED 꺼짐
  }
  else if (switch_state == LOW) {

    if (reached100ml == true && Ireached300ml && !reached500ml) {
      setcolor (0, 255, 0); // 초록색
    }
    else if (reached300ml == true && !reached500ml && !reachedoverover) {
      setcolor(255, 228, 0); //노란색
    }
    else if (reached500ml == true && !reachedoverover) {
      setcolor (0, 0, 255); // 파란색
    }
    else if (reachedoverover == true){
      setcolor(255, 0,0 );//빨강색
    }
    else {
      setcolor(255, 255, 255); // 흰색 개미눈물만큼
    }
  }
}













