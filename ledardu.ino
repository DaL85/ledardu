#include <DS1307RTC2.h>

#include <Time.h>

#include <TimeAlarms.h>

#include <Wire.h>

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
int pan=0;
int paus=0;
tmElements_t tm;

boolean button_ausdimmen = false;
int sh_and, sm_and, ss_and;
int sh_ber, sm_ber, ss_ber, endminute, endhour, endsec;
int eh_calc=0, em_calc=0, es_calc=0, ch_calc=0, cm_calc=0, cs_calc=0, timerange=0, curr_timerange=0;
int setvalue_all=0;
boolean andimmen_aktiv=false;
boolean andimmen_abgeschlossen=false;
boolean ausdimmen_aktiv=false;


void setup() {

  pinMode(ledpin, OUTPUT);  // pin 48 (on-board LED) as OUTPUT
  Serial.begin(9600);       // start serial communication at 9600bps

}

void loop()
{
  if(durchlauf>100000)//ca alle 3sec Abfrage
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
      if(andimmen_aktiv)//Zeit in ledandimmen wurde gesetzt, hier andimmen des Lichtes (abfrage der verbleibenden Zeit -> Steuern der Helligkeit)
      {
        ledandimmen_durchfuehren(sh_ber,sm_ber,ss_ber,tm.Hour,tm.Minute, tm.Second,endhour,endminute, endsec);
      }
      else if(ausdimmen_aktiv)
      {
        ledausdimmen_durchfuehren(sh_ber,sm_ber,ss_ber,tm.Hour,tm.Minute, tm.Second,endhour,endminute, endsec);
      }
      else if((tm.Hour==sh_and&&tm.Minute==sm_and)|| button_ausdimmen )
      {
        dimmzeit_setzen(tm.Hour, tm.Minute, tm.Second);
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
                sh_and = value.substring(0,2).toInt();
                sm_and = value.substring(3,5).toInt();                
              }
              else if (cmd == "baus" )
              {
                button_ausdimmen = true;
                Serial.println("Button ausdimmen gesetzt");
              }
              else if ( cmd == "pan" )
              {
                Serial.println("pan: "+value);
                pan = value.toInt();
              }
              else if ( cmd == "paus" )
              {
                paus = value.toInt();
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

void setred() 
{
  analogWrite(ledred, valuered);
}

void setgreen()
{
  analogWrite(ledgreen, valuegreen);
}

void setblue()
{
  analogWrite(ledblue, valueblue);
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

void dimmzeit_setzen(int sh, int sm, int ss)//hier wird die Startzeit des Andimmens gesetzt und die Endzeit berechnet
{
  sh_ber = sh;
  sm_ber = sm;
  ss_ber = ss;
  button_ausdimmen = false;
  Serial.print("funktion dimmzeit_setzen");
  if(andimmen_abgeschlossen)
  {
    ausdimmen_aktiv = true;    
    endminute = sm + paus;
  }
  else
  {
    andimmen_aktiv=true;
    endminute = sm + pan;
  }   
  
  endhour = sh;
  endsec=ss; //in App nur Minutenweise setzbar
  if(endminute>=60)
  {
    int anzUebertrag = endminute/60;
    endminute = endminute%(anzUebertrag*60);
    endhour = endhour + anzUebertrag;
    if(endhour<=24)
    {
      endhour = endhour%24;
    }
  }
//  Serial.println("pan: "+String(pan));
//  Serial.println("endhour: "+String(endhour));
//  Serial.println("endminute: "+String(endminute));
  
}

void ledandimmen_durchfuehren(int sh, int sm, int ss, int ch, int cm, int cs, int eh, int em, int es)//über starthour sh,..., currenthour ch,..., endhour eh,..., wird der aktuell zu setzende Wert berechnet und an den PWM-Ausgangs geschrieben
{
  Serial.println("funktion ledandimmen_durchfuehren");
    Serial.println("eh:"+String(eh));
    Serial.println("em:"+String(em));
    Serial.println("ch:"+String(ch));
    Serial.println("cm:"+String(cm));
  eh_calc = eh-sh;  //Fehler in der Berechnung? currtimerange  hat falsche Werte-> überprüfen mit serial.print...
  em_calc = em-sm;
  ch_calc = ch-sh;
  cm_calc = cm-sm;
  cs_calc = cs-ss;
  Serial.println("eh_calc:"+String(eh_calc));
  Serial.println("em_calc:"+String(em_calc));
  Serial.println("ch_calc:"+String(ch_calc));
  Serial.println("cm_calc:"+String(cm_calc));
  timerange=(eh_calc*3600)+em_calc*60; //+es_calc=es-ss=0      ->die gesamte Zeitspanne
  curr_timerange=(ch_calc*3600)+cm_calc*60+cs;                //-> die bisher verstrichene Zeitspanne
//   Serial.println("timerange:"+String(timerange));
//   Serial.println("curr_timerange:"+String(curr_timerange));
  setvalue_all=(int)(((double)curr_timerange/(double)timerange)*255);    //der zu setztende Wert
  if(setvalue_all>=255||setvalue_all<0)
    {
      setvalue_all=255;
      andimmen_aktiv=false;
      andimmen_abgeschlossen=true;
      //nicht mehr schalten LED bleiben an
    }
  Serial.println("setvalue_all: "+String(setvalue_all));
  analogWrite(ledred, setvalue_all);
  analogWrite(ledgreen, setvalue_all);
  analogWrite(ledblue, setvalue_all);  
}
void ledausdimmen_durchfuehren(int sh, int sm, int ss, int ch, int cm, int cs, int eh, int em, int es)  //ähnliche Funktion wie andimmen nur wird der setvalue_all mit 255- ursprungswert berechnet
{
  Serial.println("funktion ledausdimmen_durchfuehren");
   Serial.println("eh:"+String(eh));
    Serial.println("em:"+String(em));
    Serial.println("ch:"+String(ch));
    Serial.println("cm:"+String(cm));
  eh_calc = eh-sh;  //Fehler in der Berechnung? currtimerange  hat falsche Werte-> überprüfen mit serial.print...
  em_calc = em-sm;
  ch_calc = ch-sh;
  cm_calc = cm-sm;
  cs_calc = cs-ss;
 Serial.println("eh_calc:"+String(eh_calc));
  Serial.println("em_calc:"+String(em_calc));
  Serial.println("ch_calc:"+String(ch_calc));
  Serial.println("cm_calc:"+String(cm_calc));
  timerange=(eh_calc*3600)+em_calc*60;
  curr_timerange=(ch_calc*3600)+cm_calc*60+cs_calc;
   Serial.println("timerange:"+String(timerange));
   Serial.println("curr_timerange:"+String(curr_timerange));
  setvalue_all=(int)(255-((double)curr_timerange/(double)timerange)*255);
  if(setvalue_all<=0||setvalue_all>255)
    {
      setvalue_all=0;
      ausdimmen_aktiv=false;
      andimmen_abgeschlossen=false;
      //nicht mehr schalten LED bleiben aus
    }
  Serial.println("setvalue_all: "+String(setvalue_all));
  analogWrite(ledred, setvalue_all);
  analogWrite(ledgreen, setvalue_all);
  analogWrite(ledblue, setvalue_all);  
}
