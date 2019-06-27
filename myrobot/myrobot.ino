#include <Servo.h>
#include <IRremote.h>
Servo myservo;

#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

#define FORWARD 16736925
#define BACK    16754775
#define LEFT    16720605
#define RIGHT   16761405
#define STOP 16728765

#define RECV_PIN 12
#define ONE 16738455
#define TWO 16750695
#define THREE 16756815
#define OK 16712445
#define U_OK 3622325019


#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
#define carSpeed 150
unsigned long rightDistance = 0, leftDistance = 0, middleDistance = 0;
unsigned long MAX_DIST = 20;

int led = 13;
int execute_counter = 0;

int Echo = A4;
int Trig = A5;

IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long val;
unsigned long preMillis;

int getDistance() {
    digitalWrite(Trig, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    return (int)pulseIn(Echo, HIGH) / 58;
}

void setup(){
  Serial.begin(9600);
  Serial.println("Starting setup");
  pinMode(led, OUTPUT);
  pinMode(2, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENA, HIGH);  
  digitalWrite(ENB, HIGH); 
  myservo.attach(3);
  irrecv.enableIRIn();
  stop();
}

void forward(int carSpeed_in){
  analogWrite(ENA, carSpeed_in);
  analogWrite(ENB, carSpeed_in);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Forward");
}

void back(int carSpeed_in) {
  analogWrite(ENA, carSpeed_in);
  analogWrite(ENB, carSpeed_in);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Back");
}

void left(int carSpeed_in) {
  analogWrite(ENA, carSpeed_in);
  analogWrite(ENB, carSpeed_in);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Left");
}

void right(int carSpeed_in) {
  analogWrite(ENA, carSpeed_in);
  analogWrite(ENB, carSpeed_in);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  Serial.println("Stop!");
}

void flee() {
  Serial.println("        ok");
  while(1) {
    int s = digitalRead(2);
    if (s == 1) {
      Serial.println("I must flee");
      while(1) {
        myservo.write(90);
        delay(500);
        if (irrecv.decode(&results)){  
          preMillis = millis();
          val = results.value;
          delay(500);
          Serial.println(val);
          irrecv.resume();
        }
        if (val == OK) {
          Serial.println("Will return to main loop and not flee anymore");
          stop();
          return;
        }
        middleDistance = getDistance();
        Serial.print("Center Distance: ");
        Serial.println(middleDistance);
        Serial.println(MAX_DIST);
        while (middleDistance > MAX_DIST) {
          if (irrecv.decode(&results)){  
            preMillis = millis();
            val = results.value;
            delay(500);
            Serial.println(val);
            irrecv.resume();
          }
          if (val == OK) {
            Serial.println("Will return to main loop and not flee anymore");
            stop();
            return;
          }
          middleDistance = getDistance();
          Serial.print("New center distance: ");
          Serial.println(middleDistance);
          forward(carSpeed);
        }
        stop();
        delay(500);
        myservo.write(10);
        delay(1000);
        rightDistance = getDistance();
        Serial.print("Right Distance: ");
        Serial.println(rightDistance);
        delay(500);
        myservo.write(90);
        myservo.write(180);
        delay(1000);
        leftDistance = getDistance();
        Serial.print("Left Distance: ");
        Serial.println(leftDistance);
        delay(500);
        myservo.write(90);
        delay(1000);
        if(rightDistance > leftDistance) {
          right(200);
          delay(400);
          stop();
        } 
        else if (rightDistance < leftDistance) {
          left(200);
          delay(400);
          stop();
        } 
        else if((rightDistance <= 20) || (leftDistance <= 20)) {
          back(carSpeed);
          delay(200);
          stop();
        }
      }
    }
  }
}

void track() {
  Serial.println("        ok");
  while(1) {
    if (irrecv.decode(&results)){  
      preMillis = millis();
      val = results.value;
      delay(500);
      Serial.println(val);
      irrecv.resume();
    }
    if (val == OK) {
      Serial.println("Will return to main loop and not follow the line anymore");
      stop();
      return;
    }
    
    if(LT_M){
      forward(carSpeed);
    }
    else if(LT_R) {
      right(carSpeed);
      while(LT_R);
    }
    else if(LT_L) {
      left(carSpeed);
      while(LT_L);
    }
  }
}

void ir_mode() {
  Serial.println("      ok");
  while(1) {
    if (irrecv.decode(&results)){ 
      preMillis = millis();
      val = results.value;
      delay(500);
      Serial.println(val);
      irrecv.resume();
      switch(val){
        case FORWARD: forward(carSpeed);  break;
        case BACK:    back(carSpeed);     break;
        case LEFT:    left(200);     break;
        case RIGHT:   right(200);    break;
        case STOP:    stop();     break;
        case U_OK:
        case OK:      stop(); Serial.println("Return to main loop"); return;     break;
        default:                  break;
      }
    }
  }
}

void loop() {
  if (irrecv.decode(&results)){  
    preMillis = millis();
    val = results.value;
    delay(500);
    Serial.println(val);
    irrecv.resume();
    switch(val) {
      case ONE: Serial.print("Starting flee program..."); flee(); break;
      case TWO: Serial.print("Start line tracking..."); track(); break;
      case THREE: Serial.print("Start IR mode..."); ir_mode(); break;
      default: break;
    }
  }
}
