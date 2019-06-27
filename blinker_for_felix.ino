#include <Bounce2.h>

#define NUM_OF_BUTTONS 5
#define BUTTON_1_INDEX 0
#define BUTTON_2_INDEX 1
#define BUTTON_3_INDEX 2
#define BUTTON_4_INDEX 3
#define BUTTON_5_INDEX 4
#define DS_pin 4
#define STCP_pin 3
#define SHCP_pin 2
#define numOfRows(x) (sizeof (x) / sizeof (x[0]))

const uint8_t BUTTON_PINS[NUM_OF_BUTTONS] = {12, 11, 10, 9, 8};
uint8_t previousButtonStates[NUM_OF_BUTTONS] = {HIGH, HIGH, HIGH, HIGH, HIGH};
uint8_t buttonStateChanges[NUM_OF_BUTTONS] = {HIGH, HIGH, HIGH, HIGH, HIGH};
Bounce * buttonDebouncers = new Bounce[NUM_OF_BUTTONS];
uint8_t activeButtonIndex = 99;

unsigned long previousMillis = 0;
const long interval = 300;

int lastAnimatedRowIndex = 0;

int button1_animation[7][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0},
  {0, 1, 0, 1, 1, 0, 1, 0},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {1, 0, 0, 0, 0, 0, 0, 1}
};

int button2_animation[15][8] = {
  {1, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0}
};

int button3_animation[3][8] = {
  {1, 0, 0, 1, 0, 0, 1, 0},
  {0, 1, 0, 0, 1, 0, 0, 1},
  {0, 0, 1, 0, 0, 1, 0, 0}
};

int button4_animation[8][8] = {
  {1, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0}
};

int button5_animation[16][8] = {
  {1, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 0, 0},
  {1, 1, 1, 1, 1, 0, 0, 0},
  {1, 1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

void setup() { 
  for (int i = 0; i < NUM_OF_BUTTONS; i++) {
    buttonDebouncers[i].attach(BUTTON_PINS[i], INPUT_PULLUP);
    buttonDebouncers[i].interval(50);
  }
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
}

void readButtons() {

  for (int i = 0; i < NUM_OF_BUTTONS; i++)  {   
    buttonDebouncers[i].update();
    int currentButtonState = buttonDebouncers[i].read();

    if(previousButtonStates[i] != currentButtonState) {
      if(currentButtonState == LOW) {
        buttonStateChanges[i] = LOW;
      }
      activeButtonIndex = i;
      lastAnimatedRowIndex = 0;
      
    }
    previousButtonStates[i] = currentButtonState;  
  }
  
}

void writeRowToShiftRegister(int animationMatrix[][8], int index) {
  digitalWrite(STCP_pin, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(SHCP_pin, LOW);
    digitalWrite(DS_pin, animationMatrix[index][i]);
    digitalWrite(SHCP_pin, HIGH);
  }
  digitalWrite(STCP_pin, HIGH);
}

void animateRow(int animationMatrix[][8], size_t rowSize, int rowIndex) {
  int rowIndexToAnimate = rowIndex % rowSize;
  writeRowToShiftRegister(animationMatrix, rowIndexToAnimate);
}

void animate(int animationMatrix[][8], size_t rowSize) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    animateRow(animationMatrix, rowSize, lastAnimatedRowIndex++);
  }
}

void loop() {

  readButtons();

  if(activeButtonIndex == BUTTON_1_INDEX) {
    animate(button1_animation, numOfRows(button1_animation));
  } else if(activeButtonIndex == BUTTON_2_INDEX) {
    animate(button2_animation, numOfRows(button2_animation));
  } else if(activeButtonIndex == BUTTON_3_INDEX) {
    animate(button3_animation, numOfRows(button3_animation));
  } else if(activeButtonIndex == BUTTON_4_INDEX) {
    animate(button4_animation, numOfRows(button4_animation));
  } else if(activeButtonIndex == BUTTON_5_INDEX) {
    animate(button5_animation, numOfRows(button5_animation));
  }
  buttonStateChanges[activeButtonIndex] = HIGH;
  
}
