#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// ==========================================
// LCD
// ==========================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==========================================
// KEYPAD 4x4
// ==========================================
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

// ==========================================
// MODE
// ==========================================
#define CALCULATOR 0
#define SNAKE 1

int mode = CALCULATOR;

// ==========================================
// CALCULATOR VARIABLES
// ==========================================
String num1 = "";
String num2 = "";

char operation = '\0';

bool secondNumber = false;

// ==========================================
// SNAKE VARIABLES
// ==========================================
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

int direction = RIGHT;

int snakeX[32];
int snakeY[32];

int snakeLength = 3;

int foodX;
int foodY;

int score = 0;

bool snakeGameOver = false;

unsigned long lastMoveTime = 0;

int gameSpeed = 300;

// ==========================================
// CUSTOM LCD CHARACTERS
// ==========================================
byte snakeChar[8] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000
};

byte foodChar[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

// ==========================================
// SETUP
// ==========================================
void setup() {

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, snakeChar);
  lcd.createChar(1, foodChar);

  randomSeed(analogRead(A0));

  showCalculator();
}

// ==========================================
// LOOP
// ==========================================
void loop() {

  char key = keypad.getKey();

  // ไม่มีการกดปุ่ม
  if (!key) {

    // ถ้าอยู่ในเกม Snake ให้เกมเดินต่อ
    if (mode == SNAKE) {
      updateSnake();
    }

    return;
  }

  // ========================================
  // เปลี่ยนโหมด
  // ========================================

  if (key == '*') {

    if (mode == CALCULATOR) {

      // Calculator -> Snake
      mode = SNAKE;

      startSnake();

    } else {

      // Snake -> Calculator
      mode = CALCULATOR;

      resetCalculator();
      showCalculator();
    }

    return;
  }

  // ========================================
  // CALCULATOR MODE
  // ========================================

  if (mode == CALCULATOR) {

    calculatorInput(key);

    return;
  }

  // ========================================
  // SNAKE MODE
  // ========================================

  if (mode == SNAKE) {

    snakeInput(key);

    updateSnake();
  }
}


// ==================================================
//                    CALCULATOR
// ==================================================

void showCalculator() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Calculator");

  lcd.setCursor(0, 1);
  lcd.print("0");
}


// ------------------------------------------
// รับปุ่มเครื่องคิดเลข
// ------------------------------------------

void calculatorInput(char key) {

  // -----------------------------
  // ตัวเลข
  // -----------------------------

  if (key >= '0' && key <= '9') {

    if (!secondNumber) {

      num1 += key;

    } else {

      num2 += key;
    }

    showCalculatorNumbers();

    return;
  }


  // -----------------------------
  // เครื่องหมาย
  // -----------------------------

  if (key == 'A' ||
      key == 'B' ||
      key == 'C' ||
      key == 'D') {

    if (num1 != "") {

      operation = key;

      secondNumber = true;

      showCalculatorNumbers();
    }

    return;
  }


  // -----------------------------
  // เท่ากับ
  // -----------------------------

  if (key == '#') {

    calculate();

    return;
  }
}


// ------------------------------------------
// แสดงเลขบน LCD
// ------------------------------------------

void showCalculatorNumbers() {

  lcd.clear();

  lcd.setCursor(0, 0);

  if (num1 == "") {
    lcd.print("0");
  } else {
    lcd.print(num1);
  }

  if (operation != '\0') {

    lcd.print(" ");
    lcd.print(getOperator());
  }

  lcd.setCursor(0, 1);

  if (secondNumber) {
    lcd.print(num2);
  }
}


// ------------------------------------------
// แปลง A B C D
// ------------------------------------------

char getOperator() {

  if (operation == 'A') {
    return '+';
  }

  if (operation == 'B') {
    return '-';
  }

  if (operation == 'C') {
    return '*';
  }

  if (operation == 'D') {
    return '/';
  }

  return ' ';
}


// ------------------------------------------
// คำนวณ
// ------------------------------------------

void calculate() {

  if (num1 == "" ||
      num2 == "" ||
      operation == '\0') {

    return;
  }

  long n1 = num1.toInt();
  long n2 = num2.toInt();

  long result = 0;

  // บวก
  if (operation == 'A') {

    result = n1 + n2;
  }

  // ลบ
  else if (operation == 'B') {

    result = n1 - n2;
  }

  // คูณ
  else if (operation == 'C') {

    result = n1 * n2;
  }

  // หาร
  else if (operation == 'D') {

    if (n2 == 0) {

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("ERROR");

      lcd.setCursor(0, 1);
      lcd.print("Divide by zero");

      delay(1500);

      resetCalculator();
      showCalculator();

      return;
    }

    result = n1 / n2;
  }


  // แสดงผล
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Answer:");

  lcd.setCursor(0, 1);
  lcd.print(result);


  // เตรียมใช้ผลลัพธ์ต่อ
  num1 = String(result);

  num2 = "";

  operation = '\0';

  secondNumber = false;
}


// ------------------------------------------
// รีเซ็ตเครื่องคิดเลข
// ------------------------------------------

void resetCalculator() {

  num1 = "";

  num2 = "";

  operation = '\0';

  secondNumber = false;
}


// ==================================================
//                       SNAKE
// ==================================================

// ------------------------------------------
// เริ่มเกม
// ------------------------------------------

void startSnake() {

  snakeLength = 3;

  direction = RIGHT;

  score = 0;

  snakeGameOver = false;

  // ตำแหน่งเริ่มต้น
  snakeX[0] = 3;
  snakeY[0] = 0;

  snakeX[1] = 2;
  snakeY[1] = 0;

  snakeX[2] = 1;
  snakeY[2] = 0;

  spawnFood();

  lastMoveTime = millis();

  lcd.clear();

  drawSnake();
}


// ------------------------------------------
// รับปุ่มควบคุม Snake
// ------------------------------------------

void snakeInput(char key) {

  if (snakeGameOver) {

    // กด # เริ่มใหม่
    if (key == '#') {

      startSnake();
    }

    return;
  }


  // ขึ้น
  if (key == '2') {

    if (direction != DOWN) {
      direction = UP;
    }
  }


  // ลง
  else if (key == '8') {

    if (direction != UP) {
      direction = DOWN;
    }
  }


  // ซ้าย
  else if (key == '4') {

    if (direction != RIGHT) {
      direction = LEFT;
    }
  }


  // ขวา
  else if (key == '6') {

    if (direction != LEFT) {
      direction = RIGHT;
    }
  }
}


// ------------------------------------------
// อัปเดตเกม
// ------------------------------------------

void updateSnake() {

  if (snakeGameOver) {
    return;
  }

  if (millis() - lastMoveTime >= gameSpeed) {

    lastMoveTime = millis();

    moveSnake();
  }
}


// ------------------------------------------
// เคลื่อนงู
// ------------------------------------------

void moveSnake() {

  int newX = snakeX[0];

  int newY = snakeY[0];


  // คำนวณตำแหน่งใหม่

  if (direction == UP) {
    newY--;
  }

  else if (direction == DOWN) {
    newY++;
  }

  else if (direction == LEFT) {
    newX--;
  }

  else if (direction == RIGHT) {
    newX++;
  }


  // --------------------------------
  // ชนขอบ
  // --------------------------------

  if (newX < 0 ||
      newX >= 16 ||
      newY < 0 ||
      newY >= 2) {

    gameOverSnake();

    return;
  }


  // --------------------------------
  // ชนตัวเอง
  // --------------------------------

  for (int i = 0; i < snakeLength; i++) {

    if (snakeX[i] == newX &&
        snakeY[i] == newY) {

      gameOverSnake();

      return;
    }
  }


  // --------------------------------
  // ขยับตัวงู
  // --------------------------------

  for (int i = snakeLength - 1; i > 0; i--) {

    snakeX[i] = snakeX[i - 1];

    snakeY[i] = snakeY[i - 1];
  }


  snakeX[0] = newX;

  snakeY[0] = newY;


  // --------------------------------
  // กินอาหาร
  // --------------------------------

  if (newX == foodX &&
      newY == foodY) {

    if (snakeLength < 32) {

      snakeLength++;
    }

    score += 10;

    // เพิ่มความเร็ว
    if (gameSpeed > 100) {

      gameSpeed -= 10;
    }

    spawnFood();
  }


  drawSnake();
}


// ------------------------------------------
// สร้างอาหาร
// ------------------------------------------

void spawnFood() {

  bool valid = false;

  while (!valid) {

    foodX = random(0, 16);

    foodY = random(0, 2);

    valid = true;


    // ตรวจว่าอาหารไม่ทับงู
    for (int i = 0; i < snakeLength; i++) {

      if (snakeX[i] == foodX &&
          snakeY[i] == foodY) {

        valid = false;

        break;
      }
    }
  }
}


// ------------------------------------------
// วาด Snake
// ------------------------------------------

void drawSnake() {

  lcd.clear();


  // อาหาร
  lcd.setCursor(foodX, foodY);

  lcd.write(byte(1));


  // งู
  for (int i = 0; i < snakeLength; i++) {

    lcd.setCursor(snakeX[i], snakeY[i]);

    lcd.write(byte(0));
  }
}


// ------------------------------------------
// Game Over
// ------------------------------------------

void gameOverSnake() {

  snakeGameOver = true;

  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("GAME OVER");

  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print(score);

  delay(1000);

  lcd.clear();

  lcd.setCursor(2, 0);
  lcd.print("GAME OVER");

  lcd.setCursor(0, 1);
  lcd.print("# Restart");
}