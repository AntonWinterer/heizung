#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

//long = 32Bit
//int = 16Bit
//char = 8Bit

unsigned char debug_led = 0;
unsigned char debug_mask = 0x80;
unsigned char pwm0_percent = 0;
unsigned char pwm0_percent_old = 0;
unsigned char pwm1_percent =100;
unsigned char pwm1_percent_old = 0;

//IIC Variablen
#define MAX_IIC_RX_DATA 5
char IIC_RX_Data[MAX_IIC_RX_DATA]; //idx 0 = register; idx 1 = value

//idx 0..3 = second counter
//idx 4..7 = current counter impuls
//idx 8..11 = hour counter 1 (Heizung)
//idx 12..15 = hour counter 2 (UWP 1)
//idx 16..19 = hour counter 3 (UWP 2)
//idx 20..23 = hour counter 3 (BWP)
#define MAX_IIC_TX_DATA 24
char IIC_TX_Data[MAX_IIC_TX_DATA];
signed char rx_count = 0;
signed char tx_count = 0;

//Zaehlerimpuls vom Stromzaehler
unsigned long current_counter = 0;

//Betriebsstundenzaehler
unsigned long secound_counter = 0;
unsigned long secound_counter_old = 0;
unsigned long hour_counter1 = 0;
unsigned long hour_counter2 = 0;
unsigned long hour_counter3 = 0;
unsigned long hour_counter4 = 0;

void Init_I2C()
{
  char i;
  //Init IIC Operation
  SOPT2_IICPS = 1; // B6=SDA; B7=SCL

  //This is the address that will be responded to if set up as a slave
  IICA = 0xA0;

  //Multiply factor of 1
  IICF_MULT = 0;

  //SCL divider of 32
  IICF_ICR = 0x09;

  //Enable IIC and interrupts
  IICC  = 0xc0;

  //Initialize Data Array
  for(i<0;i<MAX_IIC_RX_DATA;i++){
    IIC_RX_Data[i] = 0;
  }
  for(i<0;i<MAX_IIC_TX_DATA;i++){
    IIC_TX_Data[i] = 0;
  }

}

void CheckSRW(void)
{
  //Check for Slave receive or transmit
  if(IICS_SRW){
    //Slave Transmit
    IICC_TX = 1;
    IICD = IIC_TX_Data[IIC_RX_Data[0]];
    tx_count++;
  } else {
    IICC_TX = 0;
    //Dummy Read
    IICD;
  }
}



void Set_PWM0(unsigned char percent)
{
//  unsigned int pwmval = percent;
//
//  if(pwmval > 100){
//    pwmval = 100;
//  }
//  if(pwmval>0 && pwmval<15){
//    pwmval = 15;  //bei kleineren Werten laeuft der Luefter nicht an
//  }
//
//  pwmval = 100 - pwmval;
//  pwmval *= 4;
//  if(pwmval > 0){
//    pwmval += 1;  //Wenn Wert groesser als PWM Modulo, dann wird 100% moeglich
//  }
//
//  TPMC0V = pwmval;

  if(percent > 100){
    percent = 100;
  }
  if(percent>0 && percent<15){
    percent = 15;  //bei kleineren Werten laeuft der Luefter nicht an
  }

  percent = 100 - percent;
  percent *= 2;
  if(percent > 0){
    percent += 1;  //Wenn Wert groesser als PWM Modulo, dann wird 100% moeglich
  }
  TPMC0VH = 0;
  TPMC0VL = percent;
}

void Set_PWM1(unsigned char percent)
{
  if(percent > 100){
    percent = 100;
  }
  if(percent>0 && percent<15){
    percent = 15;  //bei kleineren Werten laeuft der Luefter nicht an
  }

  percent = 100 - percent;
  percent *= 2;
  if(percent > 0){
    percent += 1;  //Wenn Wert groesser als PWM Modulo, dann wird 100% moeglich
  }
  TPMC1VH = 0;
  TPMC1VL = percent;
}

void Init_PWM(void)
{
  TPMMODH = 0x00; // set period to 200
  TPMMODL = 0xc8;
//  TPMMODH = 0x01; // set period to 400
//  TPMMODL = 0x90;


  TPMC0VH = 0x00;
  TPMC0VL = 0x00;
  TPMC0SC = 0x24; // set channel 0 for PWM

  TPMC1VH = 0x00;
  TPMC1VL = 0x00;
  TPMC1SC = 0x24; // set channel 1 for PWM

  TPMSC = 0x08;   // select bus clock and start timer

  Set_PWM0(0);
  Set_PWM1(0);
}


void main(void)
{

  EnableInterrupts;

  PTBDD_PTBDD4 = 1; // Set PTB4 as an output for debug LED
  PTBD_PTBD4 = 1;
  PTBD_PTBD4 = 0;
  PTBD_PTBD4 = 1;

  /** KBI Set Up PTA1 */
  KBIPE_KBIPE1 =1; /* Enable Keyboard Pin */
  KBISC_KBIE = 1;  /* Enable Keyboard Interrupts */
  KBISC_KBACK = 1; /* Clear Pending Keyboard Interrupts */
  PTAPE_PTAPE1 = 1; /* Enable Pullup for Keyboard pin */

  PTADD_PTADD2 = 0; // Set PTA2 as an input for hour_counter1
  PTAPE_PTAPE2 = 1; // Enable Pullup

  PTADD_PTADD3 = 0; // Set PTA3 as an input for hour_counter2
  PTAPE_PTAPE3 = 1; // Enable Pullup

  PTADD_PTADD5 = 0; // Set PTA5 as an input for hour_counter3
  PTAPE_PTAPE5 = 1; // Enable Pullup

  PTBDD_PTBDD3 = 0; // Set PTB3 as an input for hour_counter4
  PTBPE_PTBPE3 = 1; // Enable Pullup

  Init_I2C();

  //MTIMSC
  MTIMSC_TOIE = 1; //interrupt enable

  MTIMCLK = 0x08; //counts busclk; prescaler = 256
  //MTIMMOD = 156;  //156 -> 1h liefert 3885 counts anstatt 3600
  //MTIMMOD = 157;  //157 -> 1h liefert 3862 counts anstatt 3600
  //MTIMMOD = 168;  //168 -> 1h liefert 3322 counts anstatt 3600
  //MTIMMOD = 160;  //160 -> 1m liefert 58 counts anstatt 60
  //MTIMMOD = 159;  //159 -> 1m liefert 59 counts anstatt 60

  //MTIMMOD = 156;  //156 -> 1h liefert 3577 counts anstatt 3600
  //MTIMMOD = 157;  //157 -> 1h liefert 3553 counts anstatt 3600
  //MTIMMOD = 158;  //158 -> 1h liefert 3532 counts anstatt 3600
  //MTIMMOD = 159;  //159 -> 1h liefert 3509 counts anstatt 3600
  MTIMMOD = 155;  //155 -> 1h liefert 3599 counts anstatt 3600

  MTIMSC;
  MTIMSC_TOF = 0;

  MTIMSC_TSTP = 0; //starting...


  //PWM
  Init_PWM();

  for(;;) {

    if(debug_led&0x20){
      debug_led &= ~0x20;
    }else{
      debug_led |= 0x20;
    }

    if(secound_counter != secound_counter_old){
      secound_counter_old = secound_counter;

      if(debug_led&0x80){
        debug_led &= ~0x80;
      }else{
        debug_led |= 0x80;
      }

      if(PTAD_PTAD2 == 0){
        hour_counter1++;
        IIC_TX_Data[8] = (unsigned char)((hour_counter1&0x000000ff));
        IIC_TX_Data[9] = (unsigned char)((hour_counter1&0x0000ff00)>>8);
        IIC_TX_Data[10] = (unsigned char)((hour_counter1&0x00ff0000)>>16);
        IIC_TX_Data[11] = (unsigned char)((hour_counter1&0xff000000)>>24);
        debug_led |= 0x01;
      }else{
        debug_led &= ~0x01;
      }

      if(PTAD_PTAD3 == 0){
        hour_counter2++;
        IIC_TX_Data[12] = (unsigned char)((hour_counter2&0x000000ff));
        IIC_TX_Data[13] = (unsigned char)((hour_counter2&0x0000ff00)>>8);
        IIC_TX_Data[14] = (unsigned char)((hour_counter2&0x00ff0000)>>16);
        IIC_TX_Data[15] = (unsigned char)((hour_counter2&0xff000000)>>24);
        debug_led |= 0x02;
      }else{
        debug_led &= ~0x02;
      }

      if(PTAD_PTAD5 == 0){
        hour_counter3++;
        IIC_TX_Data[16] = (unsigned char)((hour_counter3&0x000000ff));
        IIC_TX_Data[17] = (unsigned char)((hour_counter3&0x0000ff00)>>8);
        IIC_TX_Data[18] = (unsigned char)((hour_counter3&0x00ff0000)>>16);
        IIC_TX_Data[19] = (unsigned char)((hour_counter3&0xff000000)>>24);
        debug_led |= 0x04;
      }else{
        debug_led &= ~0x04;
      }

      if(PTBD_PTBD3 == 0){
        hour_counter4++;
        IIC_TX_Data[20] = (unsigned char)((hour_counter4&0x000000ff));
        IIC_TX_Data[21] = (unsigned char)((hour_counter4&0x0000ff00)>>8);
        IIC_TX_Data[22] = (unsigned char)((hour_counter4&0x00ff0000)>>16);
        IIC_TX_Data[23] = (unsigned char)((hour_counter4&0xff000000)>>24);
        debug_led |= 0x08;
      }else{
        debug_led &= ~0x08;
      }

    }

    if(debug_led&debug_mask){
      PTBD_PTBD4 = 1;
    }else{
      PTBD_PTBD4 = 0;
    }

    //Daten die ueber IIC gekommen sind, zuordnen bzw. verteilen
    if(rx_count > 1){
      if(IIC_RX_Data[0] == 0){
        debug_mask = IIC_RX_Data[1];
      }else if(IIC_RX_Data[0] == 1){
        pwm0_percent = IIC_RX_Data[1];
        Set_PWM0(pwm0_percent);
      }else if(IIC_RX_Data[0] == 2){
        pwm1_percent = IIC_RX_Data[1];
        Set_PWM1(pwm1_percent);
      }
      rx_count = -1;
    }

    __RESET_WATCHDOG(); /* feeds the dog */
  }


}

/** MTIM ISR */
//alle 10ms, dauer 6us
interrupt 12 void  MTIM_ISR(void)
{
  static char sec = 0;
  MTIMSC;
  MTIMSC_TOF = 0;

  sec++;
  if(sec>=100){
    secound_counter++;
    sec = 0;

    IIC_TX_Data[0] = (unsigned char)((secound_counter&0x000000ff));
    IIC_TX_Data[1] = (unsigned char)((secound_counter&0x0000ff00)>>8);
    IIC_TX_Data[2] = (unsigned char)((secound_counter&0x00ff0000)>>16);
    IIC_TX_Data[3] = (unsigned char)((secound_counter&0xff000000)>>24);

  }

}


interrupt 17 void IIC_ISR(void)
{
  //Clear Interrupt Flag
  IICS_IICIF =1;

  //Master or Slave?
  if(IICC_MST){
    ; // no master, only slave mode is used
  }else{

    //Slave Operation
    if(IICS_ARBL){ //Arbitration lost?
      IICS_ARBL= 1; //clears arbitration lost flag

      //Check For Address Match
      if(IICS_IAAS){
        tx_count =0;
        rx_count = 0;
        CheckSRW();
      }
    }else{
      //Arbitration not Lost
      if(IICS_IAAS){ //addressed as a slave?
        tx_count = 0;
        rx_count = 0;
        CheckSRW();
      }else {
        if(IICC_TX){ //1 = slave transmit data
          //Check for rec ACK
          if(!IICS_RXAK){
            //ACK Recieved
            //IICD = IIC_TX_Data[tx_count];
            IICD = IIC_TX_Data[IIC_RX_Data[0]];
            tx_count++;
          } else{
            IICC_TX = 0;
            IICD;
          }
        }else{ //master send data, slave receives data
          IIC_RX_Data[rx_count] = IICD;
          rx_count++;
        }
      }
    }
  }

}


/** KBI ISR */

//scheinbar loesen beide flanken einen IRQ aus
interrupt 18 void  KBI_ISR(void)
{
  KBISC_KBACK = 1; /* Clear Pending Keyboard Interrupts */

  current_counter++;

  IIC_TX_Data[4] = (unsigned char)((current_counter&0x000000ff));
  IIC_TX_Data[5] = (unsigned char)((current_counter&0x0000ff00)>>8);
  IIC_TX_Data[6] = (unsigned char)((current_counter&0x00ff0000)>>16);
  IIC_TX_Data[7] = (unsigned char)((current_counter&0xff000000)>>24);

  if(debug_led&0x10){
    debug_led &= ~0x10;
  }else{
    debug_led |= 0x10;
  }

}




