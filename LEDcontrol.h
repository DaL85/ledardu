#ifndef LEDcontrol.h
#define LEDcontrol.h

#include "Arduino.h"
class ledcontrol
{
  public:
    ledcontrol(int red, int green, int blue);
    void set_dimtime(int starthour, int startminute, int startsecond);
    int execute_dim_on(int currenthour, int currentminute, int currentsecond);
    int execute_dim_off(int currenthour, int currentminute, int currentsecond);
    boolean get_button_ausdimmen();
    boolean get_andimmen_aktiv();
    boolean get_ausdimmen_aktiv();
    //boolean get_dimon_finished();
    int get_dimonSh();
    int get_dimonSm();
    void set_dimonSh(int v);
    void set_dimonSm(int v);
    void set_button_ausdimmen(boolean v);
    void set_period_on(int v);
    void set_period_off(int v);
    void set_valuered(int v);
    void set_valueblue(int v);
    void set_valuegreen(int v);


  private:
    void calculateAnWriteLedValues(int currentRatio);
    int pinred, pingreen, pinblue; 
    int valuered, valueblue, valuegreen;
    int dimonSh ,dimonSm ,dimonSs, Eh, Em, Es;
    int Sh_settime, Sm_settime, Ss_settime;
    boolean button_ausdimmen,ausdimmen_aktiv,andimmen_aktiv,dimon_finished;
    int period_on, period_off;


};

#endif
