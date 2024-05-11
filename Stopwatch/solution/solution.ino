#include "funshield.h"

constexpr int button_pin[] = {button1_pin, button2_pin, button3_pin};
constexpr int interval_button_error = 5;
constexpr int pow_ten[] = { 1000, 100, 10, 1 };
constexpr int dot_bitmask = 0b01111111;
constexpr int time_precision_loss = 100;

class Button{
  
private:

  int pin;
  unsigned long long last_change;
  bool prev_state;
  
public:

  void init(unsigned long long time, int inpt_pin){
    last_change = time;
    prev_state = false;
    pin = inpt_pin;
  }

  bool was_pressed(unsigned long long time){
    bool curr_state = digitalRead(pin);

    if (time - last_change > interval_button_error && prev_state != curr_state){
      prev_state = curr_state;
      last_change = time;
      if (curr_state == ON) return true;
    }

    return false;
  }
};

enum StopwatchState{ stopped, running, lapped };

class Clock{

private:

  unsigned long long time_to_display;
  unsigned long long last_time_change;
  unsigned long long time_lapped;

  StopwatchState state;

public:

  void init(unsigned long long curr_time){
    time_to_display = 0;
    last_time_change = curr_time;
    state = stopped;
  }

  bool update_time(unsigned long long curr_time){
    if (state != stopped) time_to_display += (curr_time - last_time_change);
    last_time_change = curr_time;

    return (state == running);
  }

  void reset_time(unsigned long long curr_time){
    if (state == stopped) init(curr_time);
  }

  void start_stop(){ 
    if (state == lapped) return;
    state = (state == running) ? stopped : running;
  }
  
  void lap_resume(){ 
    if (state == stopped) return;
    state = (state == running) ? lapped : running;
    time_lapped = time_to_display;
  }

  unsigned long long get_time(){ 
    return (state == lapped) ? time_lapped : time_to_display;
  }
};

class Display{

private:

  int active_digit;
  int digit_to_show;
  const int number_of_digits;
  const int dot_pos;

  void displayDigit(){
    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, digit_to_show);
    shiftOut(data_pin, clock_pin, MSBFIRST, digit_muxpos[active_digit]);
    digitalWrite(latch_pin, HIGH);
  }

public:

  Display(int active_digit, const int number_of_digits) : active_digit(active_digit), number_of_digits(number_of_digits), dot_pos(2)
  {  }

  void processNumber(unsigned long long number){

    digit_to_show = (number / pow_ten[active_digit]) % 10;
    if (number < (unsigned)pow_ten[active_digit] && active_digit < dot_pos) digit_to_show = 10;
    digit_to_show = digits[digit_to_show];
    if (active_digit == dot_pos) digit_to_show &= dot_bitmask;
  }

  void display(){

    displayDigit();
    active_digit = (active_digit + 1) % number_of_digits;
  }
};

constexpr int number_of_buttons = 3;
Button buttons[number_of_buttons];
Display D(0, 4);
Clock Stopwatch;

void setup() {
  
  unsigned long long time = millis();

  for (int i = 0; i < number_of_buttons; ++i){
    pinMode(button_pin[i], INPUT);
    buttons[i].init(time, button_pin[i]);
  }
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  Stopwatch.init(time);
}

void loop() {
  
  unsigned long long time = millis();

  if (buttons[0].was_pressed(time)) Stopwatch.start_stop();
  if (buttons[1].was_pressed(time)) Stopwatch.lap_resume();
  if (buttons[2].was_pressed(time)) Stopwatch.reset_time(time);

  if (Stopwatch.update_time(time))
    D.processNumber(Stopwatch.get_time() / time_precision_loss); // now, I calculate the digit_to_show first
  D.display();
  // and only than I display processed digit
}