//#define DEBUG_PRINT

// Serial pins
#define RTS_PIN 9
#define CTS_PIN 10

// Amiga pins
#define XA A1   //  SubD Pin 4
#define XB A0   //  SubD Pin 2
#define YA A2   //  SubD Pin 1
#define YB A3   //  SubD Pin 3
// If any axis is reversed then swap A/B
#define LB 14   //  SubD Pin 6
#define RB 15   //  SubD Pin 9

// See code below for explanation of this.
// TL;DR set this as low as you can without experiencing
// erratic cursor motion with fast mouse movements
#define DELAY_US 151

uint8_t _dataPacket[3];
uint8_t _dataPosition = 0;

uint8_t _currentStateX = 0;
uint8_t _currentStateY = 0;

void setup() {
#ifdef DEBUG_PRINT
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
#endif

  pinMode(RTS_PIN, OUTPUT);
  digitalWrite(RTS_PIN, LOW);
  pinMode(CTS_PIN, INPUT);
  Serial1.begin(1200, SERIAL_7N1);

  pinMode(XA, OUTPUT);
  pinMode(XB, OUTPUT);
  pinMode(YA, OUTPUT);
  pinMode(YB, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RB, OUTPUT);
  digitalWrite(LB, HIGH);
  digitalWrite(RB, HIGH);

#ifdef DEBUG_PRINT
  Serial.println("READY.");
#endif
}

void writeGray(uint8_t state, uint8_t pinA, uint8_t pinB) {
  uint8_t gray = state ^ (state >> 1); // Convert binary to gray code

  digitalWrite(pinA, (gray & 0x1));
  digitalWrite(pinB, (gray & 0x2));
}

void move(int8_t x, int8_t y) {
  bool x_neg = false;
  if (x < 0) x_neg = true;
  x = abs(x);
  bool y_neg = false;
  if (y < 0) y_neg = true;
  y = abs(y);

  while ((x > 0) || (y > 0)) {
    if (x > 0) {
      if (x_neg) {
        _currentStateX = (_currentStateX - 1) & 0x3;
      } else {
        _currentStateX = (_currentStateX + 1) & 0x3;        
      }
      writeGray(_currentStateX, XA, XB);
      x--;
    }
    if (y > 0) {
      if (y_neg) {
        _currentStateY = (_currentStateY - 1) & 0x3;
      } else {
        _currentStateY = (_currentStateY + 1) & 0x3;        
      }
      writeGray(_currentStateY, YA, YB);
      y--;
    }

    // We need to put a delay here, we can't strobe the X/Y pins
    // too fast or the amiga may miss parts of the grey code.
    // If this happens the mouse will skip and go backwards.
    // The way gray code works is if any bit flip is missed it is
    // impossible to know which direction the mouse moved.
    
    delayMicroseconds(DELAY_US);
    // Set this as low as you can without getting erratic cursor movements
    // when moving the mouse quickly.
    
    // If the delay is removed, then the mouse will be much more responsive
    // but will also skip and go backwards with fast movements
    // Maybe this is dependent on the noise on the X/Y lines?
  }
}

void parsePacket(uint8_t* data) {
  if (data[0] & 0x20) {
#ifdef DEBUG_PRINT
    Serial.println("Left button pressed");
#endif
    digitalWrite(LB, LOW);
  } else {
    digitalWrite(LB, HIGH);
  }
  if (data[0] & 0x10) {
#ifdef DEBUG_PRINT
    Serial.println("Right button pressed");
#endif
    digitalWrite(RB, LOW);
  } else {
    digitalWrite(RB, HIGH);
  }
  int8_t x = ((data[0] & 0x3) << 6) | (data[1] & 0x3F);
  int8_t y = ((data[0] & 0xC) << 4) | (data[2] & 0x3F);
#ifdef DEBUG_PRINT
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(", Y: ");
  Serial.println(y);
#endif
  move(x, y);
}

void loop() {
  if (Serial1.available()) {
    int c = Serial1.read();
    if (c & 0x40) { // Only first byte has highest bit set
      _dataPosition = 0;
    }
    if (_dataPosition < 3) {
      _dataPacket[_dataPosition] = (uint8_t)c;
      _dataPosition++;
      if (_dataPosition == 3) {
        parsePacket(_dataPacket);
      }
    }
  }
}
