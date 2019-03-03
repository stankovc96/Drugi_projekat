//PRIJEMNI DEO
//Baudrate 1200 za serijsku komunikaciju
#include<p30fxxxx.h>
#include "outcompare.h" //naknadno ubacen h fajl za PWM
#include "timer2.h"
_FOSC(CSW_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
_FWDT(WDT_OFF);

unsigned char tempRX,prevRX;
unsigned int broj1,broj2,r;
unsigned char poruka[10]="";
unsigned char i=0,br=0;
unsigned char enable=1;
unsigned char num_of_zeros=0;
unsigned char stop=1;
unsigned char turbo=0;

void __attribute__((__interrupt__)) _T2Interrupt(void)
{
   	TMR2 =0;
    IFS0bits.T2IF = 0;
}

void initUART1(void)
{
U1BRG=0x0207;//ovim odredjujemo baudrate 1200
				
U1MODEbits.ALTIO=0;//biramo koje pinove koristimo za komunikaciju osnovne ili alternativne

IEC0bits.U1RXIE=1;//omogucavamo rx1 interupt

U1STA&=0xfffc;

U1MODEbits.UARTEN=1;//ukljucujemo ovaj modul

U1STAbits.UTXEN=1;//ukljucujemo predaju
}


void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
	//prima 000abcde (tri nule i 5 karaktera u zavisnosti od pritisnutog tastera)
    IFS0bits.U1RXIF = 0;
    prevRX=tempRX;
    tempRX=U1RXREG;
    if(tempRX=='0' && prevRX=='0'){
        num_of_zeros++;
        LATDbits.LATD2=~LATDbits.LATD2;//signalna LED dioda treperi
    }
    if(num_of_zeros>=2){
        poruka[i]=tempRX;
        i++;
        if(i>5){
            num_of_zeros=0;
            i=0;   
        }
    }  
} 

void WriteUART1(unsigned int data)
{
	  while(!U1STAbits.TRMT);

    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}
void RS232_putst(register const char *str)
{
  while((*str)!=0)
  {
    WriteUART1(*str);
    if (*str==13) WriteUART1(10);
    if (*str==10) WriteUART1(13);
    str++;
  }
}


int main(void)
	{
        initUART1();
        WriteUART1('t');
        
        for(broj1=0;broj1<10000;broj1++);
        //LED bele i crvene RB0,RB1,RB2,RB3
        ADPCFGbits.PCFG0=1;
        ADPCFGbits.PCFG1=1;
        ADPCFGbits.PCFG2=1;
        ADPCFGbits.PCFG3=1;
        
        TRISBbits.TRISB0=0;
        TRISBbits.TRISB1=0;
        TRISBbits.TRISB2=0;
        TRISBbits.TRISB3=0;
        
        ADPCFGbits.PCFG9=1;
        ADPCFGbits.PCFG10=1;
        ADPCFGbits.PCFG11=1;
        ADPCFGbits.PCFG12=1;
        
        TRISBbits.TRISB9=0;
        TRISBbits.TRISB10=0;
        TRISBbits.TRISB11=0;
        TRISBbits.TRISB12=0;
        
        TRISDbits.TRISD2=0;//LED signalna RD2
        TRISDbits.TRISD3=0;//sirena RD3
        
        LATBbits.LATB9=0;//RB9,RB10,RB11,RB12 za upravljanje L298
        LATBbits.LATB10=0;
        LATBbits.LATB11=0;
        LATBbits.LATB12=0;
        
        LATBbits.LATB0=0;
        LATBbits.LATB1=0;
        LATBbits.LATB2=0;
        LATBbits.LATB3=0;
        
        TRISDbits.TRISD0=0;//konfigurisemo kao izlaz za pwm

        //za PWM koristimo RD0 i RD1
        PR2=2499;//odredjuje frekvenciju po formuli
        
        OC1RS=20;//postavimo pwm
        OC1R=1000;//inicijalni pwm pri paljenju samo
        OC1CON =OC_IDLE_CON & OC_TIMER2_SRC & OC_PWM_FAULT_PIN_DISABLE& T2_PS_1_256;//konfiguracija pwma
        
        OC2RS=20;//postavimo pwm
        OC2R=1000;//inicijalni pwm pri paljenju samo
        OC2CON =OC_IDLE_CON & OC_TIMER2_SRC & OC_PWM_FAULT_PIN_DISABLE& T2_PS_1_256;//konfiguracija pwma
        
        
        T2CONbits.TON=1;//ukljucujemo timer koji koristi
        
        //r=2500;
        r=2500;
        OC1RS=r;//ovim postavljamo faktor ispune
        OC2RS=r;
        
        for(broj1=0;broj1<700;broj1++)
        for(broj2=0;broj2<3000;broj2++);
        

	while(1)
	{
        if(stop==1){
           stop=0;
           
           WriteUART1('s');
        }
           
            
            
        if(poruka[1]=='n' && poruka[2]=='a' && poruka[3]=='p' && poruka[4]=='r'&& poruka[5]=='e' && stop==0){
            WriteUART1('1');
            
            
            LATBbits.LATB9=1;
            LATBbits.LATB10=0;
            LATBbits.LATB11=1;
            LATBbits.LATB12=0;
            poruka[1]=' ';
            poruka[2]=' ';
            poruka[3]=' ';
            poruka[4]=' ';
            poruka[5]=' ';
            stop=1;
        }//000napred
            
        else if(poruka[1]=='n' && poruka[2]=='a' && poruka[3]=='z' && poruka[4]=='a'&& poruka[5]=='d'&& stop==0){  
                 
            WriteUART1('2');       
            LATBbits.LATB9=0;   
            LATBbits.LATB10=1;  
            LATBbits.LATB11=0;   
            LATBbits.LATB12=1; 
            poruka[1]=' ';
            poruka[2]=' ';
            poruka[3]=' ';
            poruka[4]=' ';
            poruka[5]=' ';
            stop=1;
        }
        
        else if(poruka[1]=='l' && poruka[2]=='e' && poruka[3]=='v' && poruka[4]=='o'&& poruka[5]=='m'&& stop==0){
           WriteUART1('3');
           LATBbits.LATB9=0;
           LATBbits.LATB10=1;
           LATBbits.LATB11=1;
           LATBbits.LATB12=0;
           poruka[1]=' ';
           poruka[2]=' ';
           poruka[3]=' ';
           poruka[4]=' ';
           poruka[5]=' ';
           stop=1;
        }
        else if(poruka[1]=='d' && poruka[2]=='e' && poruka[3]=='s' && poruka[4]=='n'&& poruka[5]=='o'&& stop==0){
           WriteUART1('4');
           LATBbits.LATB9=1;
           LATBbits.LATB10=0;
           LATBbits.LATB11=0;
           LATBbits.LATB12=1;
           poruka[1]=' ';
           poruka[2]=' ';
           poruka[3]=' ';
           poruka[4]=' ';
           poruka[5]=' ';
           stop=1;
        }
        
        else if(poruka[1]=='t' && poruka[2]=='u' && poruka[3]=='r' && poruka[4]=='b'&& poruka[5]=='o'&& stop==0){
            if(turbo==1){
                turbo=0;
                OC1RS=2500;//ovim postavljamo pwm
                OC2RS=2500;
                
            }
                
            else{
                turbo=1;
                OC1RS=1900;//ovim postavljamo pwm
                OC2RS=1900;
                
            }
            poruka[1]=' ';
            poruka[2]=' ';
            poruka[3]=' ';
            poruka[4]=' ';
            poruka[5]=' ';
            WriteUART1('5');
            for(broj1=0;broj1<1000;broj1++)
            for(broj2=0;broj2<600;broj2++);
            
        }
        else if(poruka[1]=='l' && poruka[2]=='i' && poruka[3]=='g' && poruka[4]=='h'&& poruka[5]=='t'&& stop==0){
            WriteUART1('6');
            LATBbits.LATB0=~LATBbits.LATB0;
            LATBbits.LATB1=~LATBbits.LATB1;
            LATBbits.LATB2=~LATBbits.LATB2;
            LATBbits.LATB3=~LATBbits.LATB3;
            for(broj1=0;broj1<1000;broj1++)
            for(broj2=0;broj2<600;broj2++);
            poruka[1]=' ';
            poruka[2]=' ';
            poruka[3]=' ';
            poruka[4]=' ';
            poruka[5]=' ';
            
        }   
        else if(poruka[1]=='s' && poruka[2]=='i' && poruka[3]=='r' && poruka[4]=='e'&& poruka[5]=='n'&& stop==0){
            LATDbits.LATD3=1;
            WriteUART1('7');
            poruka[1]=' ';
            poruka[2]=' ';
            poruka[3]=' ';
            poruka[4]=' ';
            poruka[5]=' ';
            for(broj1=0;broj1<1000;broj1++)
            for(broj2=0;broj2<500;broj2++);
        }
        
       for(broj1=0;broj1<1000;broj1++)
            for(broj2=0;broj2<200;broj2++);//200
       LATDbits.LATD3=0;//sirena 
            
       LATBbits.LATB9=0;   
       LATBbits.LATB10=0;
       LATBbits.LATB11=0;
       LATBbits.LATB12=0;    
//WriteUART1('s');
/*
if(tempRX){
	WriteUART1(tempRX);
	tempRX=0;
}   
                WriteUART1(poruka[1]);
                WriteUART1(poruka[2]);
                WriteUART1(poruka[3]);
                WriteUART1(poruka[4]);
                WriteUART1(poruka[5]);
*/
           
    }//od whilea
return 0; 
}//od maina






