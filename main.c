#include <16F876A.h>
#device ADC=10
#use delay(crystal=20000000)
#fuses NOWDT, HS, NOCPD,NOBROWNOUT, NOPROTECT, NOLVP
#use fast_io(all)
#use RS232(BAUD = 19200, BITS = 8, PARITY = N, XMIT = PIN_C6, RCV = PIN_C7)
#define COIN                     PIN_A1
#define SERVO                    PIN_A2
#define LEDSERVO                 PIN_A4
#define LCD_RS_PIN               PIN_B0
#define LCD_RW_PIN               PIN_B1
#define LCD_ENABLE_PIN           PIN_B2
#define LCD_DATA4                PIN_B4
#define LCD_DATA5                PIN_B5
#define LCD_DATA6                PIN_B6
#define LCD_DATA7                PIN_B7
#define CSID                     PIN_C0
#define MFRC522_CS               PIN_C1
#define MFRC522_RST              PIN_C2
#define MFRC522_SCK              PIN_C3
#define MFRC522_SI               PIN_C4
#define MFRC522_SO               PIN_C5
#define TX                       PIN_C6
#define RX                       PIN_C7
#include<RC522.h>
#include <LCD.c>
char i = 0, Time = 0, UID[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, DatosTX[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, DatosRX[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int TagType;
long long Precio = 0, Tarjeta = 0;

#int_RDA
void SerialInterrupt(){
   if(kbhit()){
      DatosRX[i] = getc();
      i++;
   }
}

void main(){
   set_tris_a(0x00);
   set_tris_b(0x00);
   set_tris_c(0x90);
   output_a(0x00);
   output_b(0x00);
   output_c(0x00);
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_RDA);
   lcd_init();
   printf(lcd_putc, "\fESPERANDO\nTARJETA");
   DatosTX[1] = 0x00;
   DatosTX[2] = 0x01;
   DatosTX[3] = 0x00;
   DatosTX[4] = 0x05;;
   DatosTX[5] = UID[0];
   DatosTX[6] = UID[1];
   DatosTX[7] = UID[2];
   DatosTX[8] = UID[3];
   DatosTX[9] = 0x00;
   DatosTX[10] = 0x0D;
   MFRC522_Init();
   output_low(LEDSERVO);
   for(i = 0; i <= 0xFE; i++){
      output_high(SERVO);
      delay_ms(1);
      output_low(SERVO);
      delay_ms(3);
   }
   output_high(LEDSERVO);
   while(TRUE){
      if(MFRC522_isCard(&TagType)){       
         if(MFRC522_ReadCardSerial(&UID)){
            DatosTX[5] = UID[0];
            DatosTX[6] = UID[1];
            DatosTX[7] = UID[2];
            DatosTX[8] = UID[3];
            for(i = 0; i <= 10; i++){
               DatosRX[i] = 0x00;
            }
            printf(lcd_putc, "\fLOADING\nPLEASE WAIT");
            DatosTX[0] = 0x69;
            for(i = 0; i <= 10; i++){
               putc(DatosTX[i]);
               delay_ms(1);
            }
            i = 0;
            Time = 0;
            while(DatosRX[10] == 0x00){
               delay_ms(10);
               Time++;
               if(Time == 0xFF){
                  printf(lcd_putc, "\fERROR 02\nSERVER NO FOUND");
                  delay_ms(3000);
                  reset_cpu();
               }
            }
            if((DatosTX[0] == DatosRX[0]) && (DatosTX[1] == DatosRX[1]) && (DatosTX[2] == DatosRX[2]) && (DatosTX[3] == DatosRX[3]) && (DatosTX[4] == DatosRX[4])){
               Precio = ((DatosRX[5] * 16777216) + (DatosRX[6] * 65536) + (DatosRX[7] * 256) + DatosRX[8]);
               Tarjeta = ((DatosTX[5] * 16777216) + (DatosTX[6] * 65536) + (DatosTX[7] * 256) + DatosTX[8]);
               printf(lcd_putc, "\f%Lu\n$%Lu", Tarjeta, Precio);
               output_low(LEDSERVO);
               for(i = 0; i <= 0xFE; i++){
                  output_high(SERVO);
                  delay_ms(1);
                  delay_us(500);
                  output_low(SERVO);
                  delay_ms(2);
                  delay_us(500);
               }
               delay_ms(5000);
               for(i = 0; i <= 0xFE; i++){
                  output_high(SERVO);
                  delay_ms(1);
                  output_low(SERVO);
                  delay_ms(3);
               }
               output_high(LEDSERVO);
            }
         }
      }
   }

}
