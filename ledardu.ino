#include <DS1307RTC2.h>

#include <Time.h>

#include <TimeAlarms.h>

#include <Wire.h>

#include "LEDcontrol.h"

String cmd;
String value;
int valuered, valueblue, valuegreen;
boolean Kommandostart=false;
boolean Valuestart=false;
int ledpin = 13; // LED connected to pin 48 (on-board LED)
int ledgreen = 9;
int ledred = 10;
int ledblue = 11;
long durchlauf=0;
String fragment = "manuell";
tmElements_t tm;
ledcontrol ls(ledred,ledgreen,ledblue);

void setup() {

  pinMode(ledpin, OUTPUT);  // pin 48 (on-board LED) as OUTPUT
  Serial.begin(9600);       // start serial communication at 9600bps

}

void loop()
{
  if(fragment == "automatik" )
  {
    if(durchlauf>10000)// verhindert überlastung durch unnötige Berechngung
    {  
      durchlauf=0;
    }    
    durchlauf++;
    //Beispielstring: #red_255;
    if(durchlauf<=1)
    {    
       if (RTC.read(tm)) //Auslesen der RTC
       {
        Serial.print("Ok, Time = ");
        Serial.print(tm.Hour);
        Serial.write(':');
        Serial.print(tm.Minute);
        Serial.write(':');
        Serial.print(tm.Second);
        Serial.println();
        if(ls.get_andimmen_aktiv())//Zeit in ledandimmen wurde gesetzt, hier andimmen des Lichtes (abfrage der verbleibenden Zeit -> Steuern der Helligkeit)
        {
          ls.execute_dim_on(tm.Hour,tm.Minute, tm.Second);
        }
        else if(ls.get_ausdimmen_aktiv())
        {
          ls.execute_dim_off(tm.Hour,tm.Minute, tm.Second);
        }
        else if((tm.Hour==ls.get_dimonSh()&&tm.Minute==ls.get_dimonSm())|| ls.get_button_ausdimmen() )
        {
          ls.set_dimtime(tm.Hour, tm.Minute, tm.Second);
        } 
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
            valuered = value.toInt();
            setred();
          }
          else if ( cmd == "green" )
          {
            valuegreen = value.toInt();
            setgreen();    
          }             
          else if ( cmd == "blue" )
          {
            valueblue = value.toInt();
            setblue();
          }
          else if ( cmd == "time" )
          {
            time(value);
          }
          else if ( cmd == "tan" )
          {                
            ls.set_dimonSh(value.substring(0,2).toInt());
            ls.set_dimonSm(value.substring(3,5).toInt());                
          }
          else if (cmd == "baus" )
          {
            ls.set_button_ausdimmen(true);
            Serial.println("Button ausdimmen gesetzt");
          }
          else if ( cmd == "pan" )
          {
            Serial.println("pan: "+value);
            ls.set_period_on(value.toInt());
          }
          else if ( cmd == "paus" )
          {
            ls.set_period_off(value.toInt());
          }
          else if ( cmd == "man" )
          {
            fragment = "manuell";
          }
          else if ( cmd == "auto" )
          {
            fragment = "automatik";
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
}

void setred() 
{
  if(fragment == "manuell")
  {
    analogWrite(ledred, valuered);
  }
}

void setgreen()
{
  if(fragment == "manuell")
  {
    analogWrite(ledgreen, valuegreen);
  }
}

void setblue()
{
  if(fragment == "manuell")
  {
    analogWrite(ledblue, valueblue);
  }
}

void time(String t) //die übergebene Zeit wird aufgeteilt und an das RTC gesendet
{
  Serial.println("funktion time");
  tmElements_t tms;

  String h = t.substring(0,2);
  String m = t.substring(3,5);
  String sec = t.substring(6,8);
  Serial.print("hour: " +h);
  Serial.print("minutes:" +m);
  Serial.print("seconds:"+sec);
  tms.Hour = h.toInt();
  tms.Minute = m.toInt();
  tms.Second = sec.toInt();
  RTC.write(tms);
}


