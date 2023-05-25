#include "Sensors.h" 

unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

float read_pH() 
{
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(PIN_PH);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1023/6; //convert the analog into millivolt
  return 3.5*phValue - 2.0;                      //convert the millivolt into pH value
}

float readTemp(){
  int Vo = analogRead(PIN_THERM); 
  float Vo2 = ((float)Vo / 1023.)*3.3; 
  float R2 = (THERM_R * Vo2)/(3.3-Vo2); 
  float log_r = log(R2/6800.);
  return (1./(THERM_A + THERM_B * log_r + THERM_C * log_r * log_r + THERM_D*log_r*log_r*log_r)) - 273.15 + 0.5;
}


int buf2[10],temp2;
float avgValue2;  //Store the average value of the sensor feedback


float readEC() 
{
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf2[i]=analogRead(PIN_EC);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf2[i]>buf2[j])
      {
        temp2=buf2[i];
        buf2[i]=buf2[j];
        buf2[j]=temp2;
      }
    }
  }
  avgValue2=0;
  for(int i=2;i<8;i++) //take the average value of 6 center sample
    avgValue2+=buf2[i];
  return 14.726*avgValue/6.0 - 6270.6; // converts to ppm
}