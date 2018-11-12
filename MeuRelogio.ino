/*
 * Definicao do hardware:
 * Display (MAX7219):
 * 11 DIN
 * 10 CS
 * 13 CLK
 * 
 * Botoes: (dir p/ esq) 12 9 8 4
 * 
 * Buzzer: 5 6
 * 
 * Termometro (DS18b20): A7
 * 
 * RTC (DS3231) / Memoria (24C32):
 * SQW: 2
 * SCL: A5 (Hard SCL)
 * SDA: A4 (Hard SDA)
 * 
 */



#define DISPLAY_DIN 11
#define DISPLAY_CS  10
#define DISPLAY_CLK 13

#define BTN1 12
#define BTN2 9
#define BTN3 8
#define BTN4 4

#define BUZZER_A 5
#define BUZZER_B 6

#define TERM_1WIRE A7

#define RTC_INT_INPUT 2
#define RTC_SCL A5
#define RTC_SDA A4


void setup() {
  // put your setup code here, to run once:

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  pinMode(BUZZER_A, OUTPUT);
  pinMode(BUZZER_B, OUTPUT);

  display_init();

  
  
}

void loop() {
  // put your main code here, to run repeatedly:

  int freq = 0;
  
  if (digitalRead(BTN1) == 0)
  {
    freq = 4000;
  }

  if (digitalRead(BTN2) == 0)
  {
    freq = 5000;
  }
  
  if (digitalRead(BTN3) == 0)
  {
    freq = 6000;
  }
  
  if (digitalRead(BTN4) == 0)
  {
    freq = 10000;
  }

  if (freq != 0)
  {
    unsigned long now = millis();
    
    while(millis() < (now+200))
    {
      digitalWrite(BUZZER_A,HIGH);
      digitalWrite(BUZZER_B,LOW);
      delayMicroseconds(500000/freq);
      digitalWrite(BUZZER_B,HIGH);
      digitalWrite(BUZZER_A,LOW);
      delayMicroseconds(500000/freq);
    }

    digitalWrite(BUZZER_B,LOW);
    digitalWrite(BUZZER_A,LOW);

    freq = 0;
  }

  unsigned int test=0,test2 = 0;
  while(1)
  {
//    display_change_number(0,0,1,2);
//    delay(200);
//    display_change_number(0,0,2,1);
    
    
    display_write_number_to_buffer(0,0,((test/1000) %10));
    display_write_number_to_buffer(4,0,((test/100) %10));
    display_write_number_to_buffer(9,0,((test/10) %10));
    display_write_number_to_buffer(13,0,((test)%10));
    test++;
    if (test>9999) test = 0;
    write_buffer_to_display();
    
    delay(200);
  }

}
