#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
SoftwareSerial Serial7Segment(7, 8); //RX pin, TX pin

float x = 0.0; // main reading of altitude
float y = 0.0; // saved reading
float z = 0.0; // prediction
float a = 0.0; // temporal storage for height
float limit = 0.2; //altitude step
int count = 0; // main timer
int count1 = 0; // display timer
int lim1 = 9; //
int lim2 = 45;
int sound = 500;
int freq = 500;
int digit = 0;
char tempString[4];
bool lach = true;
float mainf = 0.0;
float second = 0.0;


void setup()
{
  bmp.begin();
  z , y = bmp.readAltitude(100900); // filling y with data for first step
  Serial7Segment.begin(9600); //Talk to the Serial7Segment at 9600 bps
  Serial7Segment.write('v'); //Reset the display - this forces the cursor to return to the beginning of the display
  Serial7Segment.write(0x7A);  // Brightness control command
  Serial7Segment.write((byte) 255);
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
        // vario speed
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
        // vario speed
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


