#include <DS1307RTC2.h>

#include <Time.h>

#include <TimeAlarms.h>

#include <Wire.h>

String cmd;
String value;
boolean Kommandostart=false;
boolean Valuestart=false;
int ledpin = 13; // LED connected to pin 48 (on-board LED)
int ledgreen = 9;
int ledred = 10;
int ledblue = 11;
long durchlauf=0;
int tanhour=0;
int tanmin=0;


void setup() {

  pinMode(ledpin, OUTPUT);  // pin 48 (on-board LED) as OUTPUT
  Serial.begin(9600);       // start serial communication at 9600bps

}

void loop()
{
  if(durchlauf>100000)//ca alle 39sec Abfrage
  {  
    durchlauf=0;
  }
    
  durchlauf++;
  //Beispielstring: #red_255;
  if(durchlauf<=1)
  {
    tmElements_t tm;
     if (RTC.read(tm)) //Auslesen der RTC
     {
      Serial.print("Ok, Time = ");
      Serial.print(tm.Hour);
      Serial.write(':');
      Serial.print(tm.Minute);
      Serial.write(':');
      Serial.print(tm.Second);
      Serial.println();
      if(tm.Hour==tanhour&&tm.Minute==tanmin)
      {
        ledandimmen();
      }
    } 
    else 
    {
      if (RTC.chipPresent()) 
      {
        Serial.println("The DS1307 is stopped.  Please run the SetTime");
        Serial.println("example to initialize the time and begin running.");
        Serial.println();
      } 
      else 
      {
        Serial.println("DS1307 read error!  Please check the circuitry.");
        Serial.println();
      }
    }
  }

    char nextChar;  
      
    if (Serial.available() > 0) 
    {            
        nextChar = Serial.read();
         if( nextChar == '#' ) //Kommando start
        {
          Kommandostart=true;
        }
        else
        {    
          if( Kommandostart )
          {            
            if( nextChar == ';' ) //Kommando vollständig
            {          
              if( cmd == "red" )
              {
                red(value);
              }
              else if ( cmd == "green" )
              {
                green(value);    
              }             
              else if ( cmd == "blue" )
              {
                blue(value);
              }
              else if ( cmd == "time" )
              {
                time(value);
              }
              else if ( cmd == "tan" )
              {
                tanhour = value.substring(0,2).toInt();
                tanmin = value.substring(3,5).toInt();                
              }
              else if ( cmd == "pan" )
              {
                
              }
              else if ( cmd == "paus" )
              {
                
              }
              
              cmd = "";
              value = "";
              Kommandostart = false;    //Rücksetzen des Startkommandos
              Valuestart = false;
            }
            else 
            {
              if( nextChar == '_' )
              {
                Valuestart=true;
              }
              else if ( !Valuestart )  //Teilstring noch im Kommandobereich
              {
                cmd += nextChar;
              } 
              else                      //Teilstring im Valuebereich
              {
                value += nextChar;              
              }
            } 
          }
        }        
    }
  
 // analogWrite(ledgreen, 100);
}

void red(String v) 
{
  int vr = v.toInt();
  analogWrite(ledred, vr);
}

void green(String v)
{
  int vg = v.toInt();
  analogWrite(ledgreen, vg);
}

void blue(String v)
{
  int vb = v.toInt();
  analogWrite(ledblue, vb);
}

void time(String t) //Schreiben der RTC
{
  tmElements_t tm;

  String h = t.substring(0,2);
  String m = t.substring(3,5);
  String sec = t.substring(6,8);
  Serial.print("hour: " +h);
  Serial.print("minutes:" +m);
  Serial.print("seconds:"+sec);
  tm.Hour = h.toInt();
  tm.Minute = m.toInt();
  tm.Second = sec.toInt();
  RTC.write(tm);
}

void ledandimmen()
{
  Serial.print("funktion ledandimmen");
}
