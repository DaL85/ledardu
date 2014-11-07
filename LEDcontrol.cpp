#include "LEDcontrol.h"
#include "Arduino.h"

ledcontrol::ledcontrol(int red, int green, int blue){
  pinred=red;
  pingreen=green;
  pinblue=blue;
}
boolean ledcontrol::get_button_ausdimmen(){
  return button_ausdimmen;
}
boolean ledcontrol::get_andimmen_aktiv(){
  return andimmen_aktiv;
}
boolean ledcontrol::get_ausdimmen_aktiv(){
  return ausdimmen_aktiv;
}
//boolean ledcontrol::get_dimon_finished(){
//  return dimon_finished;
//}
int ledcontrol::get_dimonSh(){
  return dimonSh;
}
int ledcontrol::get_dimonSm(){
  return dimonSm;
}
void ledcontrol::set_dimonSh(int v){
  dimonSh=v;
}
void ledcontrol::set_dimonSm(int v){
  dimonSm=v;
}
void ledcontrol::set_button_ausdimmen(boolean v){
  button_ausdimmen=v;
}
void ledcontrol::set_period_on(int v){
  period_on=v;
}
void ledcontrol::set_period_off(int v){
  period_off=v;
}
void ledcontrol::set_dimtime(int starthour, int startminute, int startsecond)
{
  Sh_settime = starthour;
  Sm_settime = startminute;
  Ss_settime = startsecond;
  button_ausdimmen = false;
  if(dimon_finished)
  {
    ausdimmen_aktiv = true;    
    Em = Sm_settime + period_off;
  }
  else
  {
    andimmen_aktiv=true;
    Em = Sm_settime + period_on;
  }   
 
  Eh = Sh_settime;
  Es = Ss_settime; //in App nur Minutenweise setzbar
  if(Em>=60)
  {
    int anzUebertrag = Em/60;
    Em = Em%(anzUebertrag*60);
    Eh = Eh + anzUebertrag;
    if(Eh<=24)
    {
      Eh = Eh%24;
    }
  }
}
int ledcontrol::execute_dim_on(int currenthour, int currentminute, int currentsecond)
{
  int eh_calc = Eh-Sh_settime;  
  int em_calc = Em-Sm_settime;
  int ch_calc = currenthour-Sh_settime;
  int cm_calc = currentminute-Sm_settime;
  int cs_calc = currentsecond-Ss_settime;
  int timerange=(eh_calc*3600)+em_calc*60; //+es_calc=es-ss=0      ->die gesamte Zeitspanne
  int curr_timerange=(ch_calc*3600)+cm_calc*60+cs_calc;                //-> die bisher verstrichene Zeitspanne
//   Serial.println("timerange:"+String(timerange));
//   Serial.println("curr_timerange:"+String(curr_timerange));
  int setvalue_all=255;
  if(timerange!=0)
  {
    setvalue_all=(int)(((double)curr_timerange/(double)timerange)*255);  
  }  //der zu setztende Wert
  if(setvalue_all>=255||setvalue_all<0)
  {
    setvalue_all=255;
    andimmen_aktiv=false;
    dimon_finished=true;
    //nicht mehr schalten LED bleiben an
  }
 
  Serial.println("setvalue_all: "+String(setvalue_all));
  analogWrite(pinred, setvalue_all);
  analogWrite(pingreen, setvalue_all);
  analogWrite(pinblue, setvalue_all); 
}  
int ledcontrol::execute_dim_off(int currenthour, int currentminute, int currentsecond)
{
		
	  /*Serial.println("funktion ledausdimmen_durchfuehren");
	   Serial.println("eh:"+String(eh));
		Serial.println("em:"+String(em));
		Serial.println("ch:"+String(ch));
		Serial.println("cm:"+String(cm));*/
  int eh_calc = Eh-Sh_settime;  
  int em_calc = Em-Sm_settime;
  int ch_calc = currenthour-Sh_settime;
  int cm_calc = currentminute-Sm_settime;
  int cs_calc = currentsecond-Ss_settime;
		 /*Serial.println("eh_calc:"+String(eh_calc));
		  Serial.println("em_calc:"+String(em_calc));
		  Serial.println("ch_calc:"+String(ch_calc));
		  Serial.println("cm_calc:"+String(cm_calc));*/
  int timerange=(eh_calc*3600)+em_calc*60;
  int curr_timerange=(ch_calc*3600)+cm_calc*60+cs_calc;
	   /*Serial.println("timerange:"+String(timerange));
	   Serial.println("curr_timerange:"+String(curr_timerange));*/
  int setvalue_all=(int)(255-((double)curr_timerange/(double)timerange)*255);
  if(setvalue_all<=0||setvalue_all>255)
  {
    setvalue_all=0;
    ausdimmen_aktiv=false;
    dimon_finished=false;
    //nicht mehr schalten LED bleiben aus
  }
  Serial.println("setvalue_all: "+String(setvalue_all));
  analogWrite(pinred, setvalue_all);
  analogWrite(pingreen, setvalue_all);
  analogWrite(pinblue, setvalue_all); 
}
