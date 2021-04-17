#include <DS3231.h>//Memanggil RTC3231 Library
#include <Wire.h>  // i2C Conection Library
//#include <LiquidCrystal.h> //Libraries
#include <EEPROM.h>
#include<CheapStepper.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

//Create software serial object to communicate with SIM800L
SoftwareSerial sim800l(4, 5); //SIM800L Tx & Rx is connected to Arduino #4 & #5

//CheapStepper tabletangle (0,1,3,2);

//LiquidCrystal lcd(A5,A4); //Arduino pins to lcd

// control buttons
#define bt_menu   6
#define bt_up     7
#define bt_down   9
#define bt_select  8
//#define bt_door A0
#define buzzer 10
#define led1 11
#define led2 12

// Init DS3231
DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  t; //pencacah string time()

int hh = 0, mm = 0, ss = 0, dd = 0, bb = 0, set_day;
int yy = 0;
String Day = "  ";


int setMode=0,HH=0,MM=0,DD=0,MO=0,YY=0,AM_PM=0;  // time and date reseting (current time, remainder time,)
int stop =0, mode=0, flag=0;
int menu_mode=0, pillbox=0,pill_inside=0, box =0;

char menu[7][16] = {"SETUP-MENU","PILL IN/RESET","TIME/DATE","REMAINDER TIME","PHONE NUMBER","EXIT"};

//Medicine - tablet-count
char time_slot[3][10]={"MORNING","LUNCH","NIGHT"};
int medicine[3][8]={{0,3,0,0,0,0,0,4},{2,3,4,0,3,0,2,1},{0,1,2,3,4,5,6,7}};

//remaider alarm variable
int remain_slot=0,remain_time[3][2]={6,0,13,0,20,0},t_h_m=0;
//tele-number reseting and tele number storing
int tele_num_id=0,tele_num[9]={0,0,0,0,0,0,0,0,0};
char tele_number[12]="+94";
//medicine taking
int medi_confirm[3]={0,0,0};

int bt_door=1;
int medi_time=0;


void setup() {
// Setup Serial connection
    Serial.begin(9600);
    rtc.begin(); // memulai koneksi i2c dengan RTC

     pinMode(bt_menu,  INPUT_PULLUP);
     pinMode(bt_up,    INPUT_PULLUP);
     pinMode(bt_down,  INPUT_PULLUP);
     pinMode(bt_select, INPUT_PULLUP);
     
      rtc.setTime (05, 50, 55);
      lcd.begin(16, 2); // Configura lcd numero columnas y filas
      lcd.setCursor(0,0);  //Show "TIME" on the LCD  
      lcd.print(" Audhatha-Memore ");
      lcd.setCursor (0,1);
      lcd.print(" Medi-Care-Box ");
      delay (2000);
      lcd.clear(); 

}

void loop() {
  t = rtc.getTime();
  Day = rtc.getDOWStr(1);

  if (setMode == 0){
      hh = t.hour,DEC;
      mm = t.min,DEC;
      ss = t.sec,DEC;
      dd = t.date,DEC;
      bb = t.mon,DEC;
      yy = t.year,DEC;
      }  

  if(mode==0){
      lcd.setCursor(0,0); 
      lcd.print("   ");
      lcd.print((hh/10)%10);
      lcd.print(hh % 10); 
      lcd.print(":");
      lcd.print((mm/10)%10);
      lcd.print(mm % 10);
      lcd.print(":");
      lcd.print((ss/10)%10);
      lcd.print(ss % 10);
      lcd.setCursor(1,1);
      lcd.print(" ");
      lcd.print((dd/10)%10);
      lcd.print(dd % 10); 
      lcd.print("/");
      lcd.print((bb/10)%10);
      lcd.print(bb % 10);
      lcd.print("/"); 
      lcd.print((yy/1000)%10);
      lcd.print((yy/100)%10);
      lcd.print((yy/10)%10);
      lcd.print(yy % 10);
      }
  select_menu();      
  medicine_take();
  alarming();
  msg_sending();
}


void select_menu(){
  if (menu_mode==0 && digitalRead(bt_menu)==0 && mode==0){
      lcd.clear();
      delay(500);
      mode=1;
      menu_mode= 1;}
  if (menu_mode>0){
      if (menu_mode<5 && digitalRead(bt_up)==0 ){
            menu_mode= menu_mode+1;
            lcd.clear();
            delay(500);}
      else if (menu_mode>1 && menu_mode<=5 && digitalRead(bt_down)==0 ){
            menu_mode= menu_mode-1;
            lcd.clear();
            delay(500);}
      lcd.setCursor(0,0);
      if (menu_mode==1){
            lcd.print("   ");lcd.print(menu[0]);lcd.print("   ");}
      else{
            lcd.print(menu_mode-1);lcd.print(".");lcd.print(menu[menu_mode-1]);}
    
        lcd.setCursor(0,1);
        lcd.print("->");
        lcd.print(menu[menu_mode]);
        lcd.setCursor(0,1);lcd.print("  ");

        if (menu_mode==5 && digitalRead(bt_select)==0 ){
          menu_mode=0;
          mode=0;
          lcd.clear();
          delay(500);}
        else{
          pill_box();
          time_date();
          remainder_time();
          phone_number();
          select_menu();}
    }
}



void pill_box(){
  if (menu_mode==1 && digitalRead(bt_select)==0 && pillbox==0){
        pillbox=1;   lcd.clear();    menu_mode=0;mode=1;   delay(500);}
  if (pillbox>0 && pillbox<=8 && digitalRead(bt_up)==0){
        pillbox=pillbox+1;  lcd.clear();   delay(200);}
  if (pillbox>1 && pillbox<=9 && digitalRead(bt_down)==0){
        pillbox=pillbox-1;  lcd.clear();   delay(200);}
    
  // exit from pill box menu and go inside the every pill box
  if (digitalRead(bt_select)==0){
        if (pillbox==9 ){
              pillbox=0;
              menu_mode=1;
              lcd.clear();
              delay(200);}
        else if (pillbox<9 && pillbox>0  && pill_inside==0){
              pill_inside=1;
              delay(300);
              lcd.clear();
              pill__inside(pillbox);}
    }
 if (pillbox>0 && pillbox<10){
        lcd.setCursor(0,0);
        if (pillbox==1){
                  lcd.print("   ");lcd.print("SELECT BOX");lcd.print("   ");}
        else{
                  lcd.print("->");lcd.print("PILL BOX - ");lcd.print(pillbox-1);}
        lcd.setCursor(0,1);lcd.print("->");
        //delay(50);
        //lcd.setCursor(0,1);lcd.print("->");
        if (pillbox==9){lcd.print("EXIT");}
        else{
                  lcd.print("PILL BOX - ");lcd.print(pillbox);
                  //stepper(pillbox);
                  }
        lcd.setCursor(0,1);lcd.print("  ");
        pill_box();
            }
  }


void pill__inside(int box){
  //iterate time slot in pill box
  if (pill_inside>0 && pill_inside<3 && digitalRead(bt_select)==0){
        lcd.clear();
        pill_inside=pill_inside+1;delay(500);}
  if (pill_inside>0){
        int count =medicine[pill_inside-1][box-1];
        if (digitalRead(bt_up)==0 && count<10){
                count=count+1;
                delay(500);}
        else if (digitalRead(bt_down)==0 && count>0 ){
                count = count-1;
                delay(500);}
        lcd.setCursor(0,0);  
        if (pill_inside==1){
                lcd.print("   TIME-COUNT   ");}
        else{
                lcd.print(pill_inside-1);lcd.print(".");lcd.print(time_slot[pill_inside-2]);
                lcd.setCursor(14,0);lcd.print("0");lcd.print(medicine[pill_inside-2][box-1]);}
        lcd.setCursor(0,1);lcd.print(pill_inside);lcd.print(".");lcd.print(time_slot[pill_inside-1]);
        lcd.setCursor(14,1);lcd.print("0");lcd.print(count);
        delay(50);
        lcd.setCursor(14,1);lcd.print("  ");
        medicine[pill_inside-1][box-1]=count;   }
  if (pill_inside==3 && digitalRead(bt_select)==0){
        lcd.clear();lcd.setCursor(0,0);lcd.print("Saved Successfully");delay(2000);
        lcd.clear();pill_inside=0; pillbox=box;}
  else{pill__inside(box);}
  }  


void time_date(){
  if (menu_mode==2 && digitalRead(bt_select)==0 && setMode==0){
      setMode=1; menu_mode=0; mode=1;
      lcd.clear();
      delay(500);}
  if (setMode>0){
        if (digitalRead(bt_select)==0){
              if (setMode==5 ){ 
                    setMode=0; menu_mode=2;mode=1;
                    lcd.clear();
                    rtc.setTime (hh, mm, ss);
                    rtc.setDate (dd, bb, yy);
                    lcd.setCursor(0,0);lcd.print("  TIME-DATE  "); 
                    lcd.setCursor(0,1);lcd.print("UPDATED SUCESSFULY");
                    delay(2000);
                    lcd.clear();}
              else if (setMode<5 ){
                    setMode=setMode+1; if (setMode==3){lcd.clear();}
                    delay(500);}
              }
         else if (digitalRead(bt_up)==0){
              if(setMode==1){hh=hh%24+1;}
              if(setMode==2){mm=mm%60+1;}
              if(setMode==3){dd=dd%30+1;}
              if(setMode==4){bb=bb%12+1;}
              if(setMode==5){yy=yy+1;}
              delay(300);
            }
        else if (digitalRead(bt_down)==0){
              if(setMode==1){hh=hh%24-1;}
              if(setMode==2){mm=mm%60-1;}
              if(setMode==3){dd=dd%30-1;}
              if(setMode==4){bb=bb%12-1;}
              if(setMode==5){yy=yy-1;}
              delay(300);
              }
        if (setMode<3){
              lcd.setCursor(0,0);
              lcd.print("  TIME RESET  ");
              lcd.setCursor(0,1);
              lcd.print("   ");if(hh<10){lcd.print("0");}
              lcd.print(hh); 
              lcd.print(":");if(mm<10){lcd.print("0");}
              lcd.print(mm);
              lcd.print(":");
              lcd.print("00");
              lcd.print(" ");
              }
         else if (setMode>2 && setMode<6){
              lcd.setCursor(0,0);
              lcd.print("   DATE RESET    ");
              lcd.setCursor(0,1);
              lcd.print("  ");if(dd<10){lcd.print("0");}   
              lcd.print(dd);
              lcd.print("/");if(bb<10){lcd.print("0");}
              lcd.print(bb);
              lcd.print("/"); 
              lcd.print(yy);
              }
         time_date();
      }
  }

void remainder_time(){
  if(digitalRead(bt_select)==0){
      if (menu_mode==3 && remain_slot==0){
        remain_slot=1;menu_mode=0;mode=1;lcd.clear();delay(500);}
      if (remain_slot==3 && t_h_m==1){
        lcd.clear();
        remain_slot=0;t_h_m=0;
        lcd.setCursor(0,0);lcd.print("REMAINDER UPDATED");
        menu_mode=3;mode=1;delay(1000);}
      }
  if (remain_slot>0 && remain_slot<4 && digitalRead(bt_select)==0){
        lcd.clear();delay(200);
        if (t_h_m==0){t_h_m=1;}
        else{remain_slot=remain_slot+1;t_h_m=0;}}
    
  if(remain_slot>0){
        int vary = remain_time[remain_slot-1][t_h_m];
        if (digitalRead(bt_up)==0){
              if(t_h_m==0){vary=vary%24+1;if (vary==24){vary=0;}}
              else if(t_h_m==1){vary=vary%60+1;if (vary==60){vary=0;}}
              delay(300);
          }
        else if (digitalRead(bt_down)==0){
              if(t_h_m==0){vary=vary%24-1;if (vary==24){vary=0;}}
              else if(t_h_m==1){vary=vary%60-1;if (vary==60){vary=0;}}
              delay(300);
          }
        remain_time[remain_slot-1][t_h_m]=vary;
        lcd.setCursor(0,0);
        lcd.print(" REMAINDER TIME   ");
        lcd.setCursor(0,1);
        lcd.print(remain_slot);lcd.print(".");
        lcd.print(time_slot[remain_slot-1]);
        lcd.setCursor(11,1);   
        if(remain_time[remain_slot-1][0]<10){lcd.print("0");}
        lcd.print(remain_time[remain_slot-1][0]);lcd.print(":");
        if(remain_time[remain_slot-1][1]<10){lcd.print("0");}
        lcd.print(remain_time[remain_slot-1][1]);

        remainder_time();
        
            }
    }
void phone_number(){
   if (digitalRead(bt_select)==0){
       if( menu_mode==4 && tele_num_id==0){
            menu_mode=0;mode=1;tele_num_id=1;
            lcd.clear();
            lcd.setCursor(0,0);lcd.print(" PHONE-NUMBER ");
            lcd.setCursor(0,1);lcd.print(" +94-");
            for(int i=0;i<9;i++){lcd.print(tele_num[i]);}
            delay(300);}
       else if(tele_num_id>0 && tele_num_id<10 ){
            tele_num_id=tele_num_id+1;delay(300);}
        if(tele_num_id==10){
            menu_mode=4;mode=1;tele_num_id=0;
            for (int i=0;i<9;i++){
              tele_number[i+3]=char(tele_num[i]);}
            lcd.clear();
            lcd.setCursor(0,0);lcd.print(tele_number);
            delay(20000);
            lcd.setCursor(0,0);lcd.print("Saved Sucessfully");
            delay(500);}
      }
    if (tele_num_id>0 && tele_num_id<10 ){
        int cur_posi=4+tele_num_id;
        if (digitalRead(bt_up)==0){
                tele_num[tele_num_id-1]=(tele_num[tele_num_id-1]+1)%10;
                lcd.setCursor(cur_posi,1);
                lcd.print(tele_num[tele_num_id-1]);
                delay(300);}
        else if (digitalRead(bt_down)==0){
                tele_num[tele_num_id-1]=(tele_num[tele_num_id-1]-1)%10;
                lcd.setCursor(cur_posi,1);
                lcd.print(tele_num[tele_num_id-1]);
                delay(300);
                }
        lcd.setCursor(cur_posi,1);lcd.print(" ");
        lcd.setCursor(cur_posi,1);lcd.print(tele_num[tele_num_id-1]);
       phone_number();
        }
  }

void alarming(){ 
  for(int i=0;i<3;i++){
        if(medi_confirm[i]==0 && hh==remain_time[i][0] && mm>=remain_time[i][1]){
              lcd.clear();             
              lcd.setCursor(0,0);lcd.print(time_slot[i]);lcd.print(" MEDICINE");
              lcd.setCursor(0,1);lcd.print("   TIME   ");
              speaker();
              medi_confirm[i]=-1;
              delay(10000);
              lcd.clear();
              break;}
        }
  }
  
void msg_sending(){
  for(int i=0;i<3;i++){
        if(medi_confirm[i]==-1 && (hh-1)==remain_time[i][0] && mm>=remain_time[i][1]){
              lcd.clear();
              medi_confirm[0]=-2;
              sms_module(i+1);
              lcd.setCursor(0,0);lcd.print("YOU FORGOT");
              lcd.setCursor(0,1);lcd.print(time_slot[i]);lcd.print(" MEDICINE");
              delay(5000);
              lcd.clear();
              break;
              }
        }
  }

void medicine_take(){
    if(digitalRead(bt_select)==0 && mode ==0 && digitalRead(bt_door)==1 ){
          mode=2;
          lcd.clear();
          for(int i=0;i<3;i++){
                if((remain_time[i][0]+2)>hh && (remain_time[i][0]-2)<hh){
                      if (medi_confirm[i]==1){
                        lcd.setCursor(0,0);lcd.print("YOU ALREADY");
                        lcd.setCursor(0,1);lcd.print(" TAKE MEDICINE ");
                        mode=0;
                        lcd.clear();}
                      else{
                        lcd.setCursor(0,0);lcd.print("NOW YOU CAN TAKE");
                        lcd.setCursor(0,1);lcd.print(time_slot[i]);lcd.print(" MEDICINE ");
                        medi_time=i+1;
                        delay(1000);
                        }
                      break;
                } 
              else if(i==2){
                        lcd.setCursor(0,0);lcd.print("YOU WAIT");
                        lcd.setCursor(0,1);lcd.print(" EDITING ");
                        delay(5000);
                        mode=0;medi_time=0;
                        lcd.clear();}
          }
      }

    if(medi_time>0){
          lcd.clear();
          lcd.setCursor(0,0);lcd.print(time_slot[medi_time-1]);lcd.print(" TIME PILLS");
          lcd.setCursor(0,1);lcd.print("   Press up >>");
          int box=0,finish=0;
          while(finish==0){
                if(digitalRead(bt_up)==0){
                       while (box<8){
                              if(medicine[medi_time-1][box]>0){break;}
                              else{box=box+1;}}
                       if(box==8){
                              mode=0;stepper(0);finish=1;medi_time=0;
                              lcd.clear();}
                        else {
                              lcd.clear();
                              lcd.setCursor(0,0);lcd.print("Pill Box -");lcd.print(box+1);
                              lcd.setCursor(0,1);lcd.print(medicine[medi_time-1][box]);lcd.print(" - pills");
                              stepper(box);box=box+1;}
                        }
                 else if(digitalRead(bt_down)==0){
                      box=box-2;
                      while(box>0){
                              if(medicine[medi_time-1][box]>0){
                                      lcd.clear();
                                      lcd.setCursor(0,0);lcd.print("Pill Box -");lcd.print(box+1);
                                      lcd.setCursor(0,1);lcd.print(medicine[medi_time-1][box]);lcd.print(" - pills");
                                      stepper(box);box=box+1;
                                      break;}
                              else{box=box-1;}}
                      }
             } 
        }
    }
int motor_posi=0;  
void stepper(int box){
        CheapStepper tabletangle (8,9,10,11);
        tabletangle.setRpm(12);
        int angle = (box-motor_posi)*45;
        tabletangle.moveDegreesCW (angle);
        delay(1000); 
  }
void speaker(){
        tone(buzzer,500,2000);
        delay(2000);
        }
void sms_module(int slot){
        sim800l.begin(9600);   //Module baude rate, this is on max, it depends on the version
        sim800l.print("AT+CMGF=1\r");                   //Set the module to SMS mode
        delay(100);
        sim800l.print("AT+CMGS=\r");  //Your phone number don't forget to include your country code, example +212123456789"
        sim800l.print(tele_number);
        delay(500);
        sim800l.print(time_slot[slot-1]);
        sim800l.print("SIM800l is working");       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
        delay(500);
        sim800l.print((char)26);// (required according to the datasheet)
        delay(500);
        sim800l.println();
  }
void led(){
  }
