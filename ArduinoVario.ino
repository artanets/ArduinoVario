/*
Project of arduino variometr. Pressure sensor bmp085.
Bmp085 pins SDA/SCL to arduino SDA/SCL.
4 digit display spurkfun 7-segment.
Display pin RX to Arduino pin 8.

(bmp085 is a very noisy sensor in 2m range. So you can try to find the balance
between sensetivity and false noise by playng with "limit" and "lim2")
*/
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
SoftwareSerial Serial7Segment(7, 8); //RX pin, TX pin

float x = 0.0; // main reading of altitude
float y = 0.0; // saved last reading
float z = 0.0; // prediction for didgital filter
float a = 0.0; // temporal storage for height for filter
float limit = 0.3; //altitude steps for filtering (sensativity of filtration from 0.2 -0.6)*
int count = 0; // main timer 
int count1 = 0; // display timer
int lim1 = 9; // changebl amount of iterations for filter
int lim2 = 45;// constant amount of iterations for filter (can be canged from 10 to ~60)*
int sound = 500; // time of beaping in ms
int freq = 500; // frequency of sound
int digit = 0; // used for display
char tempString[4]; // used for display
bool lach = true; // used to reset frequency, sound and amount of iterations after altitude stop changing
float mainf = 0.0; //main filter funktion
float second = 0.0; // second filter funktion


void setup()
{
  bmp.begin();
  z , y = bmp.readAltitude(100900); // filling z ,y with data for first step 
  //(100900) is a preasure on sea level must be changed due to your weather (can change altitude from 8 to ~30 m)*
  
  Serial7Segment.begin(9600); //Talk to the Serial7Segment at 9600 bps
  Serial7Segment.write('v'); //Reset the display - this forces the cursor to return to the beginning of the display
  Serial7Segment.write(0x7A);  // Brightness control command
  Serial7Segment.write((byte) 255); // Display brightness 100%
}

void loop()
{
  filter(mainf);
  
  if (count == 0) 
  {a = x;}
  
  count++;

  if (lach) 
  {sound = 500; lim1 = lim2;lach = false;}

  if (count >= lim1)
  { 
    lach = true;
    
    if (a <= (x - limit)) // lift
    {
      filter1(second);
      if (a <= (x - limit))
      {       
        // vario sound speed and frequency
        freq = 700;
        if (a <= (x - (limit + 1.0)))
        {sound = 400;lim1 = 8;freq = 800;}
        if (a <= (x - (limit + 1.5)))
        {sound = 300;lim1 = 7;freq = 850;}
        if (a <= (x - (limit + 2.0)))
        {sound = 200;lim1 = 6;freq = 900;}
        if (a <= (x - (limit + 2.5)))
        {sound = 100;lim1 = 5;freq = 950;}
              
         lach = false;
              
         tone(6, freq, sound);        
      }
    }
    if (a >= (x + limit)) //drop
    {
      filter1(second);
      if (a >= (x + limit))
      {
        // vario sound speed and frequency
        freq = 400;
        if (a >= (x + (limit + 1.0)))
        {sound = 400;lim1 = 8;freq = 350;}
        if (a >= (x + (limit + 1.5)))
        {sound = 300;lim1 = 7;freq = 300;}
        if (a >= (x + (limit + 2.0)))
        {sound = 200;lim1 = 6;freq = 250;}
        if (a >= (x + (limit + 2.5)))
        {sound = 100;lim1 = 5;freq = 200;}
              
         lach = false;
              
         tone(6, freq, sound);       
      }
    }
  count = 0;
  }


  count1++;
  if (count1 >= 50)
  {
    digit = round(x);
    sprintf(tempString, "%4d", digit);
    Serial7Segment.print(tempString);
    count1 = 0;
  }
}

// digital filter for barometr bmp085
float filter (float mainf)
{ x = 0;
  x = bmp.readAltitude(100900);
  x = round(((x + ((y + z) * 2) ) / 6) * 10);
  x = x / 10;
  z = x + (y - x);
  y = x;
  x = x * 2;
  return x;
}

float filter1 (float second)
{
  for (int i = 0; i < lim1; i++)
  {
    filter (mainf);
  }
}


