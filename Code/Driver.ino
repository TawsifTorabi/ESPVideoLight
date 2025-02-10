#define LED1_PIN D7  // Optocoupler for LED Set 1
#define LED2_PIN D6  // Optocoupler for LED Set 2

#define SWITCH1_PIN D1  // Slide switch position 1 (Pulled Up)
#define SWITCH2_PIN D2  // Slide switch position 2 (Pulled Up)

#define INDICATOR1_PIN D3  // Indicator LED for Set 1
#define INDICATOR2_PIN D4  // Indicator LED for Set 2

#define BUTTON_INC D8  // Button to increase brightness
#define BUTTON_DEC D5  // Button to decrease brightness

int maxBrightness = 40;
int brightness1 = maxBrightness;
int brightness2 = maxBrightness;
const int brightnessStep = 1;

// Button state tracking
unsigned long lastIncPressTime = 0;
unsigned long lastDecPressTime = 0;
unsigned long holdStartTime = 0;
bool incHeld = false;
bool decHeld = false;
const int debounceDelay = 50;  // 50ms debounce time
const int holdDelay = 500;  // Hold time before auto-increment starts
const int repeatRate = 100; // Speed of continuous brightness change

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);

  pinMode(INDICATOR1_PIN, OUTPUT);
  pinMode(INDICATOR2_PIN, OUTPUT);

  pinMode(BUTTON_INC, INPUT);
  pinMode(BUTTON_DEC, INPUT);

  analogWriteFreq(10000);  // Set PWM frequency to 10 kHz

  Serial.begin(115200);
}

void loop() {
  bool switch1 = digitalRead(SWITCH1_PIN) == LOW;
  bool switch2 = digitalRead(SWITCH2_PIN) == LOW;
  bool buttonIncState = digitalRead(BUTTON_INC) == HIGH;
  bool buttonDecState = digitalRead(BUTTON_DEC) == HIGH;
  unsigned long currentTime = millis();

  if (switch1 && !switch2) {  // Control LED Set 1
    handleBrightness(buttonIncState, buttonDecState, brightness1, currentTime);
    analogWrite(LED1_PIN, brightness1);
    analogWrite(LED2_PIN, brightness2);
    digitalWrite(INDICATOR1_PIN, HIGH);
    digitalWrite(INDICATOR2_PIN, LOW);
  } 
  else if (!switch1 && switch2) {  // Control LED Set 2
    handleBrightness(buttonIncState, buttonDecState, brightness2, currentTime);
    analogWrite(LED1_PIN, brightness1);
    analogWrite(LED2_PIN, brightness2);
    digitalWrite(INDICATOR1_PIN, LOW);
    digitalWrite(INDICATOR2_PIN, HIGH);
  } 
  else {
    analogWrite(LED1_PIN, brightness1);
    analogWrite(LED2_PIN, brightness2);
    digitalWrite(INDICATOR1_PIN, LOW);
    digitalWrite(INDICATOR2_PIN, LOW);
  }

  Serial.print("Set 1 Brightness: ");
  Serial.print(brightness1);
  Serial.print(" | Set 2 Brightness: ");
  Serial.println(brightness2);

  delay(20);  // Small delay for loop execution
}

void handleBrightness(bool buttonInc, bool buttonDec, int &brightness, unsigned long currentTime) {
  if (buttonInc) {
    if (!incHeld) {
      if (currentTime - lastIncPressTime > debounceDelay) {
        brightness = min(brightness + brightnessStep, maxBrightness);
        lastIncPressTime = currentTime;
        holdStartTime = currentTime;
        incHeld = true;
      }
    } else if (currentTime - holdStartTime > holdDelay) {
      if (currentTime - lastIncPressTime > repeatRate) {
        brightness = min(brightness + brightnessStep, maxBrightness);
        lastIncPressTime = currentTime;
      }
    }
  } else {
    incHeld = false;
  }

  if (buttonDec) {
    if (!decHeld) {
      if (currentTime - lastDecPressTime > debounceDelay) {
        brightness = max(brightness - brightnessStep, 0);
        lastDecPressTime = currentTime;
        holdStartTime = currentTime;
        decHeld = true;
      }
    } else if (currentTime - holdStartTime > holdDelay) {
      if (currentTime - lastDecPressTime > repeatRate) {
        brightness = max(brightness - brightnessStep, 0);
        lastDecPressTime = currentTime;
      }
    }
  } else {
    decHeld = false;
  }
}
