#include "funshield.h"
#include "input.h"

// map of letter glyphs
constexpr byte LETTER_GLYPH[] {
  0b10001000,   // A
  0b10000011,   // b
  0b11000110,   // C
  0b10100001,   // d
  0b10000110,   // E
  0b10001110,   // F
  0b10000010,   // G
  0b10001001,   // H 0b10001011 looks better
  0b11111001,   // I
  0b11100001,   // J
  0b10000101,   // K
  0b11000111,   // L
  0b11001000,   // M
  0b10101011,   // n
  0b10100011,   // o
  0b10001100,   // P
  0b10011000,   // q
  0b10101111,   // r
  0b10010010,   // S
  0b10000111,   // t
  0b11000001,   // U
  0b11100011,   // v
  0b10000001,   // W
  0b10110110,   // ksi 0b10001001 looks better
  0b10010001,   // Y
  0b10100100,   // Z
};
constexpr byte EMPTY_GLYPH = 0b11111111;

constexpr char c_EMPTY_GLYPH = (char)EMPTY_GLYPH;

constexpr int positionsCount = 4;


constexpr unsigned int scrollingInterval = 300;

class Display{

private:

  int active_digit;
  const int display_size;
  char const *message;
  int messageLength;
  char buff[positionsCount];
  
  void displayChar(char ch, byte pos){
    byte glyph = EMPTY_GLYPH;
    if (isAlpha(ch)) {
      glyph = LETTER_GLYPH[ ch - (isUpperCase(ch) ? 'A' : 'a') ];
    }
    
    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
    shiftOut(data_pin, clock_pin, MSBFIRST, 1 << pos);
    digitalWrite(latch_pin, HIGH);
  }

public:

  Display(const int display_size) : active_digit(0), display_size(display_size)
  {}

  void updateBuff(char p) { 
    buff[active_digit] = p;
  }

  void displayBuff(){
    if (active_digit >= display_size) {
      displayChar(EMPTY_GLYPH, active_digit);
      return;
    }
    displayChar(buff[active_digit], active_digit);
    active_digit = (active_digit + 1) % display_size;
  }

  int getPos() {
    return active_digit;
  }
};

Display D(positionsCount);

SerialInputHandler input;

class ShiftHandler{

private:

  char buff[positionsCount] = { c_EMPTY_GLYPH, c_EMPTY_GLYPH, c_EMPTY_GLYPH, c_EMPTY_GLYPH };
  unsigned long long last_change;
  const char *p;

  bool BuffIsEmpty(){
    for (int i = 0; i < positionsCount; ++i)
      if (buff[i] != '\0') return false;
    return true;
  }

  void fillBuff(){
    for (int i = 0; i < positionsCount; ++i)
      buff[i] = c_EMPTY_GLYPH;
  }

  void shiftBuff(){
    for (int i = 0; i < positionsCount - 1; ++i)
      buff[i] = buff[i + 1];
    buff[positionsCount - 1] = *p;
    if (*p) ++p;
  }

public:

  void init (const char* new_message, unsigned long long curr_time){
    last_change = curr_time;
    p = new_message;
  }

  void loadMessage(const char* new_message){
    p = new_message;
    fillBuff();
  }

  bool messageEnded(){
    return (*p == '\0' && BuffIsEmpty());
  }

  void updateBuff(unsigned long long curr_time){
    if (curr_time - last_change >= scrollingInterval){
      shiftBuff();
      last_change += scrollingInterval;
    }
  }

  char getChar(int pos) { 
    if (pos < positionsCount) return buff[pos];
    return c_EMPTY_GLYPH;
  }
};

ShiftHandler S;

void setup() {

  unsigned long long time = millis();
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  input.initialize();
  S.init(input.getMessage(), time);
}

void loop() {

  unsigned long long time = millis();

  if (S.messageEnded())
    S.loadMessage(input.getMessage());

  S.updateBuff(time);
  
  D.updateBuff(S.getChar(D.getPos()));
  D.displayBuff();
  
  input.updateInLoop();
}