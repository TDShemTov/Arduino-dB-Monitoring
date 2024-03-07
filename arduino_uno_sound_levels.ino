#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <math.h>

// Defining pins and what ports they are hooked up to
#define SCLK_PIN 13
#define DIN_PIN 11
#define CS_PIN   10
#define DC_PIN   7
#define RST_PIN  8

// Color definitions
#define BLACK   0x0000
#define WHITE   0xFFFF
#define BLUE    0x001F
#define RED     0xF800
#define YELLOW  0xFFE0
#define GREEN   0x07E0
#define LIGHTGREEN    0x97F7

// Create an Adafruit SSD1351 object
Adafruit_SSD1351 display = Adafruit_SSD1351(CS_PIN, DC_PIN, DIN_PIN, SCLK_PIN, RST_PIN);


//MAX4466 hooked to A0 pin
const int micPin = A0;

const float ref_voltage = 3.3; // 3.3 volts

//basic values for continuous graph implementation
static int yval[6] = {80, 80, 80, 80, 80, 80};
static int currPixel = 80;
static int prev_y = 80;
static int prev_x = 0;

// Function to convert voltage to decibels
double voltageToDecibels(float voltage) {
  return 20.0 * log10(voltage / ref_voltage);
}

void setup() {
  display.begin();  // start the display
  display.fillScreen(LIGHTGREEN);
  Serial.begin(9600);
  //boundaries for graph (startx, starty, endx, endy, color)
  display.drawLine(0, 60, 127, 60, BLACK);
  display.drawLine(0, 90, 127, 90, BLACK);
  //for message announcement
  display.drawRect(5, 37, 115, 20, BLACK);
  resetScreen();
}
//This function resets specific parts on the screen that needs to be resetted
void resetScreen() {
  //reset for gradient
  display.fillRect(6, 97, 117, 22, WHITE);
  //reset for graph
  display.fillRect(0, 61, 128, 29, WHITE);
  //black bars for gradient
  int x = 4;
  for (int i = 0; i < 16; i++) {
    display.drawRect(x + 1, 95 + 1, 14, 24, BLACK);
    x += 7;
  }
  //for message on screen
  display.fillRect(6, 38, 113, 18, WHITE);
}
//based on dB, the length of the gradient meter changes(higher dB->longer meter)
int findIter(double dB) {
  int iter = 0;
  if (dB >= 0 && dB < 10.5) {
    iter = 5;
  }
  else if (dB >= 10.5 && dB < 17.5) {
    iter = 10;
  }
  else {
    iter = 16;
  }

  return iter;
}

//stylize the graph to be gradient, from green to yellow to red
void gradient(int iter) {
  //start from green
  uint16_t color = GREEN;
  int x = 4;
  for (int i = 0; i < iter; i++) {
    //based on # of iterations, fill the real-time meter with more colors, color is manipulated based on 16-bit color masking
    display.fillRect(x + 2, 97, 12, 22, color);
    uint16_t Green = (color & GREEN) >> 5; //0x3F, shifting and extracting green from rgb assuming everything is 00000 111111 00000 -> 00000 00000 111111
    uint16_t Red = (color & RED) >> 11; //shifting and extracting red from rgb assuming everything is 11111 000000 00000 -> 00000 000000 11111
    uint16_t Blue = 0;

    //color of gradient meter goes from green->yellow->red based on # of iterations
    //0 to 31, maximize red
    if (Red + 4 <= 31) {
      Red += 4;
    }
    else {
      Red = 31;
    }
    //minimize green
    if (Green - 4 >= 0) {
      Green -= 4;
    }
    else {
      Green = 0;
    }
    //color is changed depending on iteration, shifting back the bits to their place using bit-wise OR
    color = (Green << 5) | (Red << 11) | Blue;
    x = x + 7;
  }
}

//text display on the screen based on position,color,size
void printText(double dB) {

  display.setTextSize(2);
  if (dB >= 0 && dB < 10.5) {
    display.setCursor(19, 40); // sets position of text
    display.setTextColor(GREEN);
    display.println("I sleep");
  }
  else if (dB >= 10.5 && dB < 17.5) {
    display.setCursor(10, 40);
    display.setTextColor(YELLOW);
    display.println("Too close");
  }
  else {
    display.setTextSize(2);
    display.setCursor(8, 40);
    display.setTextColor(RED);
    display.println("I'M AWAKE");
  }
}
void drawGraph(double dB) {
  //topleft_x, topleft_y, bottomright_x, bottomright_y, color;
  yval[5] = currPixel - dB;
  //keeping values in bound so graph can be visually represented clearly
  if (yval[5] <= 60) {
    yval[5l] = 60;
  }
  else if (yval[5] >= 90) {
    yval[5] = 90;
  }

  //tracking previous coordinates and inserting new coordinates, draw the new point of graph
  // OLED has 127 pixels, 125+5, iteration 1: x axis=2, iteration 2: x axis = 27 .... until x axis =127
  for (int xval = 0; xval < 6; xval++) {
    display.drawLine(prev_x, prev_y, 2 + (xval * 25), yval[xval], BLACK);
    prev_x = 2 + (xval * 25);
    prev_y = yval[xval];
  }
  //reset prev_vars and currPixel for next point calculation
  prev_x = 0;
  prev_y = 80;
  currPixel = 90;
  //shift all elements to the left to create space for new point on graph(makes graph shift left)
  for (int i = 0; i < 5; i++) {
    yval[i] = yval[i + 1];
  }
}
void loop() {

  // Read analog voltage from MAX4466
  int sensorValue = analogRead(micPin);


  // converting to voltage
  double voltage = analogRead(A0) * (ref_voltage / 1023.0);

  //absolute value of dB
  double dB = abs(voltageToDecibels(voltage));

  //reset the screen for visual clarity
  resetScreen();
  drawGraph(dB);
  printText(dB);

  //before drawing gradient meter, figure out level of dB to decide # of iterations. higher iterations longer meter
  int iter = findIter(dB);
  gradient(iter);

  //loop() repeats every 2 seconds
  delay(2000);
}
