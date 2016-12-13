#include <Time.h>
#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>


#define PIN 0
#define button 1

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);

//Colors
uint32_t off, hour_color, minute_color, second_color = strip.Color(0, 0, 0);

//Time
int h,m,s,timer = 0;

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.setBrightness(1);
  strip.show();
  pinMode(button, INPUT);
  setTime(7,9,0,12,12,2016); //hr,min,sec,day,month,year
}

void loop() {
  // put your main code here, to run repeatedly:
  h = hour()%12; //Makes 24 hour clock into 12 hour clock
  m = minute()/5; //Only 12 LEDS on clock so we round down to nearest 5th
  s = second()/5;

  set_color(); //Sets color of LED, checks to see if any clock hands overlap

  strip.setPixelColor(h, hour_color);
  strip.setPixelColor(m, minute_color);
  strip.setPixelColor(s, second_color);

///////////////////////////////////////////////////////////////////
//Detect when button is pressed and for how long
///////////////////////////////////////////////////////////////////
  int val = digitalRead(button);
  while(val == HIGH){
    val = digitalRead(button);
    timer++; //Timer is in deciseconds
    delay(100); //
  }
  if(timer > 20){ //If button is held, then set the time
    set_time();
  }if(timer > 1){ //If button is pressed, then display the time
    show_color();
  }
  timer = 0; //Reset timer
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
  strip.setPixelColor(h, off); //Clear hour
  strip.setPixelColor(m, off); //Clear minute
  strip.setPixelColor(s, off); //Clear second
  strip.show(); //Makes sure to turn off prevoius LED for each hand of the clock
}

/////////////////////////////////////////////////////////////
//Checking to see if clock hands overlap
//If they do set the led color to a mix of both hands color
//////////////////////////////////////////////////////////////
void set_color(){ 
  //initial value
  hour_color = strip.Color(255, 0, 0); //Hour hand is red
  minute_color = strip.Color(0, 0, 255); //Minute hand is blue
  second_color = strip.Color(0, 255, 0);  //Second hand is green
  if(h == m && h == s){ //Are all the hands overlapping?
    second_color = strip.Color(255, 255, 255); // white
  }else if(h == s){ //Are the hour and second hand overlapping?
    second_color = strip.Color(255, 255, 0); //yellowish
  }else if(m == s){ //Are the minutes and second hand overlapping?
    second_color = strip.Color(0, 255, 255);//Green_blue?
  }else if(h == m){ //Are the hour and minute hand overlapping?
    minute_color = strip.Color(255, 0, 255); //Purple
  }
}

void show_color(){
  strip.show();
  delay(3000); //3 seconds so user can have time to read clock
}

void set_time(){
  //Clear the leds
  strip.setPixelColor(h, off);
  strip.setPixelColor(m, off);
  strip.setPixelColor(s, off);
  strip.show();
  //set hour
  int i = 0;
  while(true){ //Loops through each hour, user presses the push button when current hour is lit
    strip.setPixelColor(i, hour_color);
    strip.show();
    delay(1000); //Stays on each LED/Hour for 1 second
    int val = digitalRead(button);
    if(val == 1){
      h = i; //Sets hour 
      i = 0;
      break; 
    }
    strip.setPixelColor(i, off);
    strip.show();
    i++;
    if(i == 12){
      i = 0;
    }
  }
  //set minute
  while(true){ //Loops through each minute, user presses the push button when current minute is lit
    strip.setPixelColor(h, hour_color); //Dispalys hour for users information
    strip.setPixelColor(i, minute_color); 
    strip.show();
    delay(1000);
    int val = digitalRead(button);
    if(val == 1){
      m = 5*i;//only 12 leds so we round
      i = 0;
      break;
    }
    strip.setPixelColor(i, off);
    strip.show();
    i++;
    if(i == 12){
      i = 0;
    }
  }
  setTime(h,m,0,12,12,2016); //Set the new time
}
