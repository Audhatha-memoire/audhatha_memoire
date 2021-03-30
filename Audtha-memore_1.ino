#include <DS3231.h>//Memanggil RTC3231 Library
#include <Wire.h>  // i2C Conection Library
#include <LiquidCrystal.h> //Libraries
#include <EEPROM.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Arduino pins to lcd

#define bt_menu   A0
#define bt_up     A1
#define bt_down   A2
#define bt_select  A3

//#define buzzer 8

// Init DS3231
DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  t; //pencacah string time()

int hh = 0, mm = 0, ss = 0, dd = 0, bb = 0, set_day;
int yy = 0;
String Day = "  ";

//int AlarmHH  = 21, AlarmMM  = 22, AlarmSS  = 23, setMode = 0, setAlarm = 0, alarmMode=0;

int stop =0, mode=0, flag=0;
int menu_mode=0, pillbox=0,setMode=0;
//Eeprom Store Variable
uint8_t HH;
uint8_t MM;
char menu[7][16] = {"SETUP-MENU","PILL IN/RESET","CLOCK TIME","ALARM","REMAINDER TIME","PHONE NUMBER","EXIT"};
char time_slot[3][10]={"MORNING","LUNCH","NIGHT"};

//Medicine - tablet -count
int morn_tablet[8]={},lun_tablet[8]={},nig_tablet[8]={};

void setup() {
// Setup Serial connection
  Serial.begin(9600);

  rtc.begin(); // memulai koneksi i2c dengan RTC

 pinMode(bt_menu,  INPUT_PULLUP);
 pinMode(bt_up,    INPUT_PULLUP);
 pinMode(bt_down,  INPUT_PULLUP);
 pinMode(bt_select, INPUT_PULLUP);

 

  
  
  lcd.begin(16, 2); // Configura lcd numero columnas y filas
  lcd.setCursor(0,0);  //Show "TIME" on the LCD

  lcd.setCursor (0,0);
  
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
  lcd.print((hh/10)%10);
  lcd.print(hh % 10); 
  lcd.print(":");
  lcd.print((mm/10)%10);
  lcd.print(mm % 10);
  lcd.print(":");
  lcd.print((ss/10)%10);
  lcd.print(ss % 10);
  lcd.print(" ");  
  if(mode==1){lcd.write(2);}
  else{lcd.print(" ");}   
  lcd.print(" "); 
  lcd.write(1); 
  lcd.print(rtc.getTemp(),0);
  lcd.write(223); 
  lcd.print("C");
  lcd.print("  "); 

  lcd.setCursor(1,1);
  lcd.print(Day);
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
pill_box();
//delay(100);
//blinking();
}


void select_menu(){
  if (menu_mode==0 && digitalRead(bt_menu)==0){
    lcd.clear();
    delay(500);
    mode=1;
    menu_mode= 1;}
  if (menu_mode>=1 && menu_mode<6 && digitalRead(bt_up)==0 ){
    menu_mode= menu_mode+1;
    lcd.clear();
    delay(500);
    }
  if (menu_mode>1 && menu_mode<=6 && digitalRead(bt_down)==0 ){
    menu_mode= menu_mode-1;
    lcd.clear();
    delay(500);
  }
  if (menu_mode==6 && digitalRead(bt_select)==0 ){
    menu_mode=0;
    mode=0;
    lcd.clear();
    delay(500);
    }



  if (menu_mode>0){
    //lcd.clear();
    lcd.setCursor(0,0);
    if (menu_mode==1){
      lcd.print("   ");lcd.print(menu[0]);lcd.print("   ");
      }
    else{
      lcd.print(menu_mode-1);lcd.print(".");lcd.print(menu[menu_mode-1]);}

    lcd.setCursor(0,1);
    lcd.print("->");
    lcd.print(menu[menu_mode]);
    lcd.setCursor(0,1);lcd.print("  ");
    }
}



void pill_box(){
  if (menu_mode==1 && digitalRead(bt_select)==0 && pillbox==0){
    pillbox=1;
    lcd.clear();
    menu_mode=0;
    delay(500);
    }
  if (pillbox>0 && pillbox<=8 && digitalRead(bt_up)==0){
    pillbox=pillbox+1;
    lcd.clear();
    delay(500);}
  if (pillbox>1 && pillbox<=9 && digitalRead(bt_up)==0){
    pillbox=pillbox-1;
    lcd.clear();
    delay(500);}
    
  // exit from pill box menu and go inside the every pill box
  if (pillbox==9 && digitalRead(bt_select)==0){
    pillbox=0;
    menu_mode=0;mode=0;
    lcd.clear();
    delay(500);}
  else if (0<pillbox<9 && digitalRead(bt_select)==0){
    pillbox =pillbox*10;
    }


    if (pillbox>0 && pillbox<10){
    //lcd.clear();
    lcd.setCursor(0,0);
    if (pillbox==1){
      lcd.print("   ");lcd.print("SELECT BOX");lcd.print("   ");
      }
    else{
      lcd.print("->");lcd.print("PILL BOX - ");lcd.print(pillbox-1);}

    lcd.setCursor(0,1);
    lcd.print("->");
    if (pillbox==9){lcd.print("EXIT");}
    else{lcd.print("PILL BOX - ");lcd.print(pillbox);}
    lcd.setCursor(0,1);lcd.print("  ");
    
    }
    



    
  }
  








void blinking(){
  //if (menu_mode==1){lcd.setCursor(0,1);lcd.print("                ");}
  }



