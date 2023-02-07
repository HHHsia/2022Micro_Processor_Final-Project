#define FL_PWM_PIN 2
#define FL_IN1_PIN 4
#define FL_IN2_PIN 16
#define RL_PWM_PIN 17
#define RL_IN1_PIN 21
#define RL_IN2_PIN 22
#define FR_PWM_PIN 14
#define FR_IN1_PIN 27
#define FR_IN2_PIN 26
#define RR_PWM_PIN 25
#define RR_IN1_PIN 33
#define RR_IN2_PIN 32

#define FR_CHANNEL 1
#define FL_CHANNEL 2
#define RR_CHANNEL 3
#define RL_CHANNEL 4


int outputPower = 0;
int turnPower = 0;
int forwardStatus = 1;
int feq = 5000;
int dutyCycle = 210;
int continueOutput = false;
int stoped = true;

void stopCar() {
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  ledcWrite(4, 0);
  stoped = true;
}

void enPower() {
  int p1 = map(outputPower, 0, 1023, 0, 255);
  ledcWrite(1, p1);
  ledcWrite(2, p1);
  ledcWrite(3, p1);
  ledcWrite(4, p1);
  stoped = false;
}

void forward() {
  forwardStatus = 1;
  digitalWrite(FR_IN1_PIN, HIGH);
  digitalWrite(FR_IN2_PIN, LOW);
  digitalWrite(FL_IN1_PIN, HIGH);
  digitalWrite(FL_IN2_PIN, LOW);
  digitalWrite(RR_IN1_PIN, HIGH);
  digitalWrite(RR_IN2_PIN, LOW);
  digitalWrite(RL_IN1_PIN, HIGH);
  digitalWrite(RL_IN2_PIN, LOW);
}
void shutdownall() {
  //forwardStatus = 1;
  digitalWrite(FR_IN1_PIN, LOW);
  digitalWrite(FR_IN2_PIN, LOW);
  digitalWrite(FL_IN1_PIN, LOW);
  digitalWrite(FL_IN2_PIN, LOW);
  digitalWrite(RR_IN1_PIN, LOW);
  digitalWrite(RR_IN2_PIN, LOW);
  digitalWrite(RL_IN1_PIN, LOW);
  digitalWrite(RL_IN2_PIN, LOW);
}

void goBack() {
  forwardStatus = 0;
  digitalWrite(FR_IN1_PIN, LOW);
  digitalWrite(FR_IN2_PIN, HIGH);
  digitalWrite(FL_IN1_PIN, LOW);
  digitalWrite(FL_IN2_PIN, HIGH);
  digitalWrite(RR_IN1_PIN, LOW);
  digitalWrite(RR_IN2_PIN, HIGH);
  digitalWrite(RL_IN1_PIN, LOW);
  digitalWrite(RL_IN2_PIN, HIGH);
}

void spinRight() {
  digitalWrite(FR_IN1_PIN, LOW);
  digitalWrite(FR_IN2_PIN, HIGH);
  digitalWrite(RR_IN1_PIN, LOW);
  digitalWrite(RR_IN2_PIN, HIGH);
}

void spinLeft() {
  digitalWrite(FL_IN1_PIN, LOW);
  digitalWrite(FL_IN2_PIN, HIGH);
  digitalWrite(RL_IN1_PIN, LOW);
  digitalWrite(RL_IN2_PIN, HIGH);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  ledcAttachPin(FR_PWM_PIN, FR_CHANNEL); // assign RGB led pins to channels
  ledcAttachPin(FL_PWM_PIN, FL_CHANNEL);
  ledcAttachPin(RR_PWM_PIN, RR_CHANNEL);
  ledcAttachPin(RL_PWM_PIN, RL_CHANNEL);
  ledcSetup(FR_CHANNEL, feq, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(FL_CHANNEL, feq, 8);
  ledcSetup(RR_CHANNEL, feq, 8);
  ledcSetup(RL_CHANNEL, feq, 8);
  pinMode(FR_IN1_PIN, OUTPUT);
  pinMode(FR_IN2_PIN, OUTPUT);
  pinMode(FL_IN1_PIN, OUTPUT);
  pinMode(FL_IN2_PIN, OUTPUT);
  pinMode(RR_IN1_PIN, OUTPUT);
  pinMode(RR_IN2_PIN, OUTPUT);
  pinMode(RL_IN1_PIN, OUTPUT);
  pinMode(RL_IN2_PIN, OUTPUT);
}

void loop()
{
  Serial.printf("loop start\n");
  foward();
  delay(3000);
  
  spinright();
  delay(3000);

  backward();
  delay(3000);
  
  spinleft();
  Serial.print("end of loop\n\n");

  delay(10000);

}
void foward() {
  Serial.print("forward\n");
  continueOutput = true;
  forward();
  enPower();
  delay(500);
  continueOutput = false;
  stopCar();
  shutdownall();
  delay(500);
}
void backward() {
  Serial.print("goback\n");
  continueOutput = true;
  goBack();
  enPower();
  delay(500);
  continueOutput = false;
  stopCar();
  shutdownall();
  delay(500);

}
void spinright() {
  Serial.println("spin right\n");
  forward(); //先回到前進狀態
  spinRight();
  enPower();
  delay(500);
  stopCar();
  shutdownall();
  delay(500);
}
void spinleft() {
  Serial.println("\nspin left\n");
  forward(); //先回到前進狀態
  spinLeft();
  enPower();
  delay(500);
  stopCar();
  shutdownall();
  delay(500);
  
}

/*
  //set output power level
  void fn3() {
  //outputPower = 1;
  //Serial.print("set power = ");
  //Serial.println(outputPower);
  if(continueOutput){
    enPower();
  }
  }

  //設定轉彎力道
  void fn1() {
  turnPower = 1;
  Serial.print("set turn power = ");
  Serial.println(turnPower);
  }
*/
/*
  void turnright() {
  int x ;
  Serial.println("=== turn right ===");
  if(x == 1){
    goRight();
    right();
  }
  else{
    if(forwardStatus==1){
      forward();
    }
    else
    {
      goBack();
    }
    if(continueOutput){
      enPower();
    }
    else
    {
      stopCar();
    }
  }
  }

  void turnleft() {
  //int x = 1;
  Serial.println("=== turn left ===");
  if(x == 1){
    goLeft();
    left();
  }else{
    if(forwardStatus==1){
      forward();
    }
    else
    {
      goBack();
    }
    if(continueOutput){
      enPower();
    }
    else
    {
      stopCar();
    }
  }
  }
*/
/*
void goRight() {
  //如果是前進狀態，停右前輪逆轉右後輪
  //如果是倒退狀態，停右後輪逆轉右前輪
  if (forwardStatus == 1) {
    digitalWrite(FR_IN1_PIN, HIGH);
    digitalWrite(FR_IN2_PIN, LOW);
    digitalWrite(FL_IN1_PIN, HIGH);
    digitalWrite(FL_IN2_PIN, LOW);
    digitalWrite(RR_IN1_PIN, LOW);
    digitalWrite(RR_IN2_PIN, HIGH);
    digitalWrite(RL_IN1_PIN, HIGH);
    digitalWrite(RL_IN2_PIN, LOW);
  } else {
    digitalWrite(FR_IN1_PIN, HIGH);
    digitalWrite(FR_IN2_PIN, LOW);
    digitalWrite(FL_IN1_PIN, LOW);
    digitalWrite(FL_IN2_PIN, HIGH);
    digitalWrite(RR_IN1_PIN, LOW);
    digitalWrite(RR_IN2_PIN, HIGH);
    digitalWrite(RL_IN1_PIN, LOW);
    digitalWrite(RL_IN2_PIN, HIGH);
  }
}

void goLeft() {
  //如果是前進狀態，停左前輪逆轉左後輪
  //如果是倒退狀態，停左後輪逆轉左前輪
  if (forwardStatus == 1) {
    digitalWrite(FR_IN1_PIN, HIGH);
    digitalWrite(FR_IN2_PIN, LOW);
    digitalWrite(FL_IN1_PIN, HIGH);
    digitalWrite(FL_IN2_PIN, LOW);
    digitalWrite(RR_IN1_PIN, HIGH);
    digitalWrite(RR_IN2_PIN, LOW);
    digitalWrite(RL_IN1_PIN, LOW);
    digitalWrite(RL_IN2_PIN, HIGH);
  } else {
    digitalWrite(FR_IN1_PIN, LOW);
    digitalWrite(FR_IN2_PIN, HIGH);
    digitalWrite(FL_IN1_PIN, HIGH);
    digitalWrite(FL_IN2_PIN, LOW);
    digitalWrite(RR_IN1_PIN, LOW);
    digitalWrite(RR_IN2_PIN, HIGH);
    digitalWrite(RL_IN1_PIN, LOW);
    digitalWrite(RL_IN2_PIN, HIGH);
  }
}*/
/*
 * 
void right() {
  int p1 = map(outputPower, 0, 1023, 0, 255);
  int p2 = map(turnPower, 0, 1023, 0, 255);
  if (forwardStatus == 1) {
    ledcWrite(FR_CHANNEL, 0);
    ledcWrite(RR_CHANNEL, p2);
    ledcWrite(FL_CHANNEL, p1);
    ledcWrite(RL_CHANNEL, p1);
  } else {
    ledcWrite(FR_CHANNEL, p2);
    ledcWrite(RR_CHANNEL, 0);
    ledcWrite(FL_CHANNEL, p1);
    ledcWrite(RL_CHANNEL, p1);
  }
  //如果是前進狀態，停右前輪
  //如果是倒退狀態，停右後輪
  // if(forwardStatus==1){
  //   ledcWrite(FR_CHANNEL,0);
  //   ledcWrite(FL_CHANNEL,p2);
  // }else{
  //   ledcWrite(FR_CHANNEL,p1);
  //   ledcWrite(FL_CHANNEL,p1);
  // }
  // if(forwardStatus==1){
  //   ledcWrite(RR_CHANNEL,p1);
  //   ledcWrite(RL_CHANNEL,p1);
  // }else{
  //   ledcWrite(RR_CHANNEL,0);
  //   ledcWrite(RL_CHANNEL,p2);
  // }
}

void left() {
  int p1 = map(outputPower, 0, 1023, 0, 255);
  int p2 = map(turnPower, 0, 1023, 0, 255);
  if (forwardStatus == 1) {
    ledcWrite(FR_CHANNEL, p1);
    ledcWrite(RR_CHANNEL, p1);
    ledcWrite(FL_CHANNEL, 0);
    ledcWrite(RL_CHANNEL, p2);
  } else {
    ledcWrite(FR_CHANNEL, p1);
    ledcWrite(RR_CHANNEL, p1);
    ledcWrite(FL_CHANNEL, p2);
    ledcWrite(RL_CHANNEL, 0);
  }

  //如果是前進狀態，停左前輪
  //如果是倒退狀態，停左後輪
  // if(forwardStatus==1){
  //   ledcWrite(FL_CHANNEL,0);
  //   ledcWrite(RL_CHANNEL,p2);
  // }else{
  //   ledcWrite(FL_CHANNEL,p1);
  //   ledcWrite(RL_CHANNEL,p1);
  // }
  // if(forwardStatus==1){
  //   ledcWrite(RR_CHANNEL,p1);
  //   ledcWrite(FR_CHANNEL,p1);
  // }else{
  //   ledcWrite(FL_CHANNEL,p2);
  //   ledcWrite(RL_CHANNEL,0);
  // }
}*/
