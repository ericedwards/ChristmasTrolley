//
// Automatic Trolley using L293D with PWM
//

const int MOTOR_PWM = 11;   // Pin 11 (MOTOR_PWM) --> Pin 1 (Enable 1)
const int MOTOR_IN1 = 10;   // Pin 10 (MOTOR_IN1) --> Pin 2 (In 1)
const int MOTOR_IN2 =  9;   // Pin  9 (MOTOR_IN2) --> Pin 7 (In 2)

const int DIRECTION_LEFT = -1;
const int DIRECTION_RIGHT = 1;
const int MIN_SPEED = 0;
const int MAX_SPEED = 255;
const int SPEED_INCREMENT = 20;

int currentSpeed;
int currentDirection;
int targetSpeed;
int targetDirection;

void setup() {
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  resetSpeedAndDirection();
  updateMotor();
  Serial.begin(9600);
}

void loop() {
  int theKey;
  if (Serial.available()) {
    theKey = Serial.read();
    switch (theKey) {
      case 'w':
        targetSpeed += SPEED_INCREMENT;
        if (targetSpeed > MAX_SPEED) targetSpeed = MAX_SPEED;
        Serial.write(theKey);
        break;
      case 's':
        targetSpeed -= SPEED_INCREMENT;
        if (targetSpeed < MIN_SPEED) targetSpeed = MIN_SPEED;
        Serial.write(theKey);
        break;
      case 'a':
        targetDirection = DIRECTION_LEFT;
        Serial.write(theKey);
        break;
      case 'd':
        targetDirection = DIRECTION_RIGHT;
        Serial.write(theKey);
        break;
      case 'z':
        resetSpeedAndDirection();
        Serial.write(theKey);
        break;
      default:
        Serial.write('?');
        break;
    }
  }
  updateSpeedAndDirection();
  updateMotor();
  delay(100);
}

void resetSpeedAndDirection() {
  currentSpeed = 0;
  currentDirection = DIRECTION_RIGHT;
  targetSpeed = 0;
  targetDirection = DIRECTION_RIGHT;
}

void updateSpeedAndDirection() {
  if ((currentDirection != targetDirection) && (currentSpeed > 0)) {
    // need to keep slowing to change directions
    currentSpeed -= 1;
  } else if ((currentDirection != targetDirection) && (currentSpeed == 0)) {
    // can change directions
    currentDirection = targetDirection;
  } else if (currentSpeed < targetSpeed) {
    // need to speed up
    currentSpeed += 1;
  } else if (currentSpeed > targetSpeed) {
    // need to slow down
    currentSpeed -= 1;
  }
}

void updateMotor() {
  if (currentDirection == DIRECTION_RIGHT) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
  } else if (currentDirection == DIRECTION_LEFT) {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  } else {
    currentDirection = DIRECTION_RIGHT;
    currentSpeed = 0;
  }
  analogWrite(MOTOR_PWM, currentSpeed);
}
