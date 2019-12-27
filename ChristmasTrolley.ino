//
// Automatic Trolley using L293D with PWM
//

const int MOTOR_PWM = 11;   // Pin 11 (MOTOR_PWM) --> Pin 1 (Enable 1)
const int MOTOR_IN1 = 10;   // Pin 10 (MOTOR_IN1) --> Pin 2 (In 1)
const int MOTOR_IN2 =  9;   // Pin  9 (MOTOR_IN2) --> Pin 7 (In 2)
const int SENSOR_RIGHT = 8; // Pin  8
const int SENSOR_LEFT = 7;  // Pin  7

const int DIRECTION_LEFT = -1;
const int DIRECTION_RIGHT = 1;
const int MIN_SPEED = 0;
const int MAX_SPEED = 255;
const int SPEED_INCREMENT = 48;
const int AUTO_SPEED = 192;

const int STATE_STATION_LEFT = 0;
const int STATE_RUN_RIGHT = 1;
const int STATE_STATION_RIGHT = 2;
const int STATE_RUN_LEFT = 3;

int currentSpeed;
int currentDirection;
int targetSpeed;
int targetDirection;
int sensorRight;
int sensorLeft;
int rightValue;
int rightCount;
int leftValue;
int leftCount;
int currentState;
int stateCount;

void setup() {
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(SENSOR_RIGHT, INPUT);
  pinMode(SENSOR_LEFT, INPUT);
  resetStateMachine();
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
  debounceSensors();
  runStateMachine();
  updateSpeedAndDirection();
  updateMotor();
  delay(50);
}

void resetStateMachine() {
  currentState = 0;
  stateCount = 0;
}

void runStateMachine() {
  ++stateCount;
  switch (currentState) {
    case STATE_STATION_LEFT:
      if (stateCount > 100) {
        Serial.write('1');
        targetDirection = DIRECTION_RIGHT;
        targetSpeed = AUTO_SPEED;
        currentState = STATE_RUN_RIGHT;
        stateCount = 0;
      }
      break;
    case STATE_RUN_RIGHT:
      if (sensorRight == LOW) {
        Serial.write('2');
        targetDirection = DIRECTION_RIGHT;
        targetSpeed = 0;
        currentState = STATE_STATION_RIGHT;
        stateCount = 0;
      }
      break;
    case STATE_STATION_RIGHT:
      if (stateCount > 100) {
        Serial.write('3');
        targetDirection = DIRECTION_LEFT;
        targetSpeed = AUTO_SPEED;
        currentState = STATE_RUN_LEFT;
        stateCount = 0;
      }
      break;
    case STATE_RUN_LEFT:
      if (sensorLeft == LOW) {
        Serial.write('0');
        targetDirection = DIRECTION_LEFT;
        targetSpeed = 0;
        currentState = STATE_STATION_LEFT;
        stateCount = 0;
      }
      break;
  }
}

void resetSensors() {
  sensorRight = 0;
  sensorLeft = 0;
  rightValue = 0;
  rightCount = 0;
  leftValue = 0;
  leftCount = 0;
}

void debounceSensors() {
  int rightX = digitalRead(SENSOR_RIGHT);
  if (rightX == rightValue) {
    if (rightCount < 255) {
      rightCount++;
    }
    if (rightCount == 2) {
      sensorRight = rightValue;
      Serial.write('R');
    }
  } else {
    rightValue = rightX;
    rightCount = 0;
  }
  int leftX = digitalRead(SENSOR_LEFT);
  if (leftX == leftValue) {
    if (leftCount < 255) {
      leftCount++;
    }
    if (leftCount == 2) {
      sensorLeft = leftValue;
      Serial.write('L');
    }
  } else {
    leftValue = leftX;
    leftCount = 0;
  }
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
    currentSpeed -= 4;
  } else if ((currentDirection != targetDirection) && (currentSpeed == 0)) {
    // can change directions
    currentDirection = targetDirection;
  } else if (currentSpeed < targetSpeed) {
    // need to speed up
    currentSpeed += 4;
    if (currentSpeed > 255) currentSpeed = 255;
  } else if (currentSpeed > targetSpeed) {
    // need to slow down
    currentSpeed -= 4;
    if (currentSpeed < 0) currentSpeed = 0;
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
