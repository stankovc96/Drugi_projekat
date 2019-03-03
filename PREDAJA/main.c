//PREDAJNI DEO
//Baudrate 1200 za serijsku komunikaciju
#include<p30fxxxx.h>
#include <p30F4013.h>
#include"Tajmeri.h"

_FOSC(CSW_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
_FWDT(WDT_OFF);

unsigned char tempRX;
unsigned int broj1,broj2;
unsigned int brojac_ms,stoperica,ms,sekund; // za Delay_ms


void initUART1(void)
{
    U1BRG=0x0207;//ovim odredjujemo baudrate 1200
				
    U1MODEbits.ALTIO=0;//biramo koje pinove koristimo za komunikaciju osnovne ili alternativne
    IEC0bits.U1RXIE=1;//omogucavamo rx1 interupt
    U1STA&=0xfffc;
    U1MODEbits.UARTEN=1;//ukljucujemo ovaj modul
    U1STAbits.UTXEN=1;//ukljucujemo predaju
}
void __attribute__ ((__interrupt__)) _T2Interrupt(void) //PREKID TAJMERA 2 (na svakih 0.1ms!!!)
{
 
    TMR2 =0;//postavljamo registar tajmera 2 na nulu
    ms=1;//fleg za milisekundu ili prekid;potrebno ga je samo resetovati u funkciji
 
    brojac_ms++;//brojac milisekundi
    stoperica++;//brojac za funkciju Delay_ms
 
    if (brojac_ms==10000)//sek
        {
          brojac_ms=0;
          sekund=1;//fleg za sekundu
         }
    IFS0bits.T2IF = 0; //interrupt flag T2
       
}
void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;
    tempRX=U1RXREG;
} 

/*********************************************************************
* Ime funkcije      : WriteUART1                                     *
* Opis              : Funkcija upisuje podatak u registar 			 *
* 					  za slanje podataka U1TXREG,   				 *
* Parametri         : unsigned int data-podatak koji treba poslati   *
* Povratna vrednost : Nema                                           *
*********************************************************************/
void Delay_ms (unsigned int vreme)// 0.1ms prekid
{
        stoperica = 0;
        while(stoperica < vreme);
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
    ADPCFG=0xffff;//svi digitalni
    //ulazni digitalni pinovi
    TRISBbits.TRISB0=1;//desno
    TRISBbits.TRISB1=1;//levo
    TRISBbits.TRISB2=1;//gore
    TRISBbits.TRISB3=1;//dole
    TRISBbits.TRISB4=1;//pwm(turbo)
    TRISBbits.TRISB5=1;//light
    TRISBbits.TRISB8=1;//sirena
    
    //izlazni digitalni pinovi
    TRISFbits.TRISF0=0;//zelena
    TRISFbits.TRISF1=0;//crvena
    TRISFbits.TRISF4=0;//zuta
    TRISFbits.TRISF5=0;//plava
    
	for(broj1=0;broj1<10000;broj1++);
    initUART1();
    Init_T2();
    RS232_putst("UART test");

	while(1)
	{
        switch(PORTB & 0x013F){//maskiramo pinove koje koristimo za tastere
            case(0x0001):{ //desno
                LATFbits.LATF0=1;
                RS232_putst("000desno");
                Delay_ms(500);//50ms
                LATFbits.LATF0=0;
            }break;
            case(0x0002):{ //levo
                LATFbits.LATF0=1;
                RS232_putst("000levom");
                Delay_ms(500);
                LATFbits.LATF0=0;
            }break;
            case(0x0004):{ //napred
                LATFbits.LATF0=1;
                RS232_putst("000napre");
                Delay_ms(500);
                LATFbits.LATF0=0;
            }break;
            case(0x0008):{  //nazad
                LATFbits.LATF0=1;
                RS232_putst("000nazad");
                Delay_ms(500);
                LATFbits.LATF0=0;
            }break;
            case(0x0010):{  //turbo
                LATFbits.LATF1=1;
                RS232_putst("000turbo");
                Delay_ms(500);
                LATFbits.LATF1=0;
            }break;
            case(0x0020):{  //svetla
                LATFbits.LATF4=1;
                RS232_putst("000light");
                Delay_ms(500);
                LATFbits.LATF4=0;
            }break;
            case(0x0100):{  //sirena
                LATFbits.LATF5=1;
                RS232_putst("000siren");
                Delay_ms(500);
                LATFbits.LATF5=0;
            }
        }

}//od whilea
return 0;
}//od maina