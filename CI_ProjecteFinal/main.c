/* Main.c file generated by New Project wizard
 *
 * Created:   jue abr 26 2018
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#define _XTAL_FREQ 8000000  
#define estrella 1
#define player 2

#define right PORTAbits.RA0
#define top PORTAbits.RA1
#define bot PORTAbits.RA2
#define left PORTAbits.RA3

#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include "config.h"
#include "GLCD.h"

//TAULER
char tauler[8][15];

//Mode de joc
char mode = 0;
char special = 0;
char is_finished = 0;

short puntuacio = 0;
short moure_estrella;

short temps_total = 0;
short decimes = 0;
short temps = 0;
short mode_especial=0;

short valor_estrelles = 10;
short apareixe_estrella = 0;
int cont = 0;
char init = 0;
short record = 0;

char c_estrella = '#';

//STRINGS	
const char * s_titol = "S T A R  H U N T E R";  
const char * s_inici = "Press X to start"; 
const char * s_title_end = "G A M E O V E R"; 
const char * s_end = "Press X to continue"; 
const char * s_maxPuntuacio = "Record:";   
const char * s_dif = "Dif->";   
const char * v_dificultats[] = {{"1:Easy  "}, {"2:Normal" }, {"3:Hard  "}, {"4:Harder"} };
const char * s_activat = "Mode S activated";
const char * s_desactivat = "Mode S desactivated";  
const char * s_error = "Caracter no valid"; 
const char * s_punt = "Points";  
const char * s_time = "Time";  
const char * s_dificultat = "Level";  

/* $ -> jugador */
/* # * -> estrella */
/* " -> quadrat */



//Enviar dates en cs

void putc_usart1 (char xc) {

   while (! PIR1bits.TX1IF);
   TXREG1 = xc;

}

void puts_usart1 (char * s) {

   while(*s!='\0')
      putc_usart1 (*s++);

}


char is_grid(int x , int y) {
   
   if(x > -1 && x < 8 && y > -1 && y < 15) return 1;
   else return 0; 

}


// Initialize PORTs and basic PIC resources
void InitPIC() {

   
   //PORTS
   
    ANSELA=0;
    ANSELC=0x04;
    ANSELD=0;
    ANSELB=0;
    
    PORTA=0;
    PORTC=0;
    PORTD=0; 		   //Donem uns valors inicials als ports
    PORTB=0; 
    
    TRISA=0xFF;
    TRISD=0x00;		   //Configurem D i B de sortida
    TRISB=0x00;  
    TRISC=0xFF;

    //TIMERS
       
    IPEN = 1;
    INTCON = 0xE0; 	
    
    T0CON = 0x01;	// Configuracion Timer0 (Prescaler 1:4)
    TMR0H = 0x3C;	// Contador  a 50000 
    TMR0L = 0xB0; 
    
    TMR2IE = 1;
    PR2 = 0x7D;
    T2CON = 0x4A; //Prescaler 16 Postscaler 10
    
    
    //AD Converter
    
    ADCON0 = 0b00111001; //AN0 / ENABLE ADC 
    ADCON1 = 0x00; //REFERENCE (VSS, VD)
    ADCON2 = 0b10100001;  //Right justified, TAD = 8 Tosc, Tacq = 4 Tad 
      
    //LINIA SERIE
    
    RCSTA1 = 0b10010000; 
    TXSTA1 = 0b00000100;
    BAUDCON1bits.BRG16 = 1;
    SPBRG1 = 0x10; 
    RC1IF = 0;
    RC1IP = 1;
    RC1IE = 1;
        
}


void canviarDificultat(char num){
   
   switch(num) {
      
	 case 0:
	    moure_estrella = 20;
	    apareixe_estrella = 2000;	  
	    break;
	 case 1:
	    moure_estrella = 15;
	    apareixe_estrella = 1500;
	    break;
	 case 2:
	    moure_estrella = 10;
	    apareixe_estrella = 1000;
	    break;
	 case 3:
	    moure_estrella = 5;
	    apareixe_estrella = 500;
	    break;           
   }
      
}


char checkDificultat( char num ) {
      ADCON0bits.GO = 1; // Demanem una conversió
      while(ADCON0bits.GO){} // Espera

      int valor = ((ADRESH<<8) | ADRESL);

      if(valor < 256) num = 3;
      else if (valor < 512)num = 2;
      else if (valor <768)num = 1;
      else  num = 0;
	 	 
      if(valor != num) {
	 for(int i=0;i<8;i++)
	       putchGLCD(6,17+i,v_dificultats[num][i]);  
	 
	 canviarDificultat(num);	 
	 valor = num;
	 
      }

      return num;
}



void pintar_tauler() {
   
   for(int i=0; i<8; i++) {
       for(int j=0; j<15; j++) {
	    if(tauler[i][j]==estrella) putchGLCD(i,j,c_estrella);  
      }  
   }

}



void joc_finalitza() {
   
   TMR0ON = 0;
   TMR2ON = 0;
   
   if(puntuacio>record) record = puntuacio;
   
   clearGLCD(0,7,0,127);      
   
   for(int i=0;i<15;i++) 
      putchGLCD(1,5+i,s_title_end[i]);
   
   for(int i=0;i<6;i++)
      putchGLCD(3,9+i, s_punt[i]);  
   
   if(puntuacio == 0) putchGLCD(3,17,'0');
   else writeNum(3,17,puntuacio);
   
   for(int i=0;i<19;i++)
      putchGLCD(6,3+i,s_end[i]);
     
   mode=0;
   
   while(!mode) {}
      
}

void new_star() {
   
   int x,y;
   
   do{
      
      x = (rand() % 8);   
      y = (rand() % 15);    
   
   } while(tauler[x][y]!=0);

      tauler[x][y] = estrella;
      putchGLCD(x,y,c_estrella);  
        
}

void play() {
   
   char x=0;
   char y=0;   
      
   do{
      x = (rand() % 8);   
      y = (rand() % 15);    
   } while(tauler[x][y]!=0);

   tauler[x][y] = player;
   putchGLCD(x,y,'$');  
   
   //activar interrupcions timers etc
   valor_estrelles = 10;
   mode_especial=0;
   special=0;
   decimes = 0;
   cont = 0;
   mode = 1;
   TMR2IE = 1;
   TMR0IE = 1;
   TMR0ON = 1;
   TMR2ON = 1;
   is_finished = 0;
      
//   byte boto_top = 0;
//   byte boto_bot = 0;
//   byte boto_right = 0;
//   byte boto_left = 0;
   
   while(!is_finished) {
            
	 if (top) { //&& !boto_top) {
	 __delay_ms(10);
	 if (top) { 	    
	    if(is_grid(x-1,y)){
	       if(tauler[x-1][y] == estrella) {
		  tauler[x-1][y] = 0;
		  puntuacio += valor_estrelles;
		  writeNum(5,19, puntuacio);
	       }
	       putchGLCD(x,y,' ');  
	       putchGLCD(x-1,y,'$');
	       tauler[x][y] = 0;
	       tauler[x-1][y] = player;
	       x -= 1;	       
	       }
	    }
	     while(top);
	 }
	 
	 if (bot) { //&& !boto_bot) {
	 __delay_ms(10);
	 if (bot) { 	    
	    if(is_grid(x+1,y)){
	       if(tauler[x+1][y] == estrella){
		  tauler[x+1][y] = 0;
		  puntuacio += valor_estrelles;
		  writeNum(5,19, puntuacio);
	       }
	       putchGLCD(x,y,' ');  
	       putchGLCD(x+1,y,'$');  
	       tauler[x][y] = 0;
	       tauler[x+1][y] = player;
	       x += 1;	       
	       }
	    }  
	       while(bot); 
	 }
      
	 if (right) { //&& !boto_right) {
	 __delay_ms(10);
	 if (right) { 	    
	    if(is_grid(x,y+1)){
	       if(tauler[x][y+1] == estrella) {
		  tauler[x][y+1] = 0;
		  puntuacio += valor_estrelles;
		  writeNum(5,19, puntuacio);
	       }
	       putchGLCD(x,y,' ');  
	       putchGLCD(x,y+1,'$');  
	       tauler[x][y] = 0;
	       tauler[x][y+1] = player;
	       y += 1;	       
	       }
	    }   
	       while(right);
	 }
      
	 if (left) { //&& !boto_left) {
	 __delay_ms(10);
	 if (left) { 	    
	    if(is_grid(x,y-1)){
	       if(tauler[x][y-1] == estrella){
		  tauler[x][y-1] = 0;
		  puntuacio += valor_estrelles;
		  writeNum(5,19, puntuacio);
	       }
	       putchGLCD(x,y,' ');  
	       putchGLCD(x,y-1,'$');  
	       tauler[x][y] = 0;
	       tauler[x][y-1] = player;
	       y -= 1;	       
	       }
	    }   
	       while(left);
	 }
	 
	 //boto_left = left;
	 //boto_right = right;
	 //boto_top = top;
	 //boto_bot = bot;
	          
	 //num = checkDificultat(num); retarda massa joc

	 
  }

    joc_finalitza(); 
   
}


void InitTauler(char num) {
   
   for(int i=0; i<8; i++) 
       for(int j=0; j<15; j++) 
	 tauler[i][j]=0;
       
   c_estrella = '#'; 
       
   clearGLCD(0,7,0,127);      
   
   for(int i=0;i<8;i++)
      putchGLCD(i,15,'|');
   
   for(int i=0;i<4;i++)
      putchGLCD(1,18+i,s_time[i]);  
   
   putchGLCD(2,19, '0');

   for(int i=0;i<6;i++)
      putchGLCD(4,17+i,s_punt[i]);  

   putchGLCD(5,19, '0');

   for(int i=0;i<8;i++)
	       putchGLCD(7,17+i,v_dificultats[num][i]);
   
   char x=0;
   char y=0;   
   
   for(int i=0;i<3;i++) new_star();
   
   play();
   
}

char se_potmoure(int i, int j) {
   
   if(is_grid(i+1,j) && tauler[i+1][j] == 0) return 1;
   if(is_grid(i-1,j) && tauler[i-1][j] == 0) return 1;
   if(is_grid(i,j+1) && tauler[i][j+1] == 0) return 1;
   if(is_grid(i,j-1) && tauler[i][j-1] == 0) return 1;
   
   return 0;
   
 }


void moure_estrelles() {
   
    for(int i=0; i<8; i++) {
       for(int j=0; j<15; j++) {
	  
	    if(tauler[i][j] == estrella && se_potmoure(i,j)==1){ 
	      	       
	      int x = 0, y = 0;
	       
	       do {
	       
	       int move = rand() % 4;
		  		  
	       switch(move) {
		case 0:
		  x=i-1;
		  y=j;
		  break;
		
		case 1:
		   x=i;
		  y=j+1;
		  break;
		case 2:
		   x=i+1;
		  y=j;
		  break;
		case 3:
		   x=i;
		  y=j-1;
		  break;
				  
	       } 
	    }while(is_grid(x,y)==0 || tauler[x][y] != 0);
		 
	    tauler[i][j] = 0;	
	    tauler[x][y] = estrella;	
	    putchGLCD(i,j,' ');  
	    putchGLCD(x,y,c_estrella);	  
	    
	    
	    
	 } 

       }
    } 
   
          
    
} 

  

//Inici joc i elegir nivell

void InitJoc(){
  
   clearGLCD(0,7,0,127);      
   
   for(int i=0;i<20;i++)
      putchGLCD(1,3+i,s_titol[i]);
   
   for(int i=0;i<16;i++)
      putchGLCD(4,5+i,s_inici[i]);
   
   for(int i=0;i<7;i++)
      putchGLCD(6,1+i,s_maxPuntuacio[i]);
   
   if(record == 0) putchGLCD(6,9,'0');
   else writeNum(6,9,record);   
   
   for(int i=0;i<5;i++)
      putchGLCD(6,12+i,s_dif[i]);
   
   int valor = 0;   
   mode = 0;
   char num = 4;
   
   while( !mode ) {
            
      num = checkDificultat(num);
   
   }
   
   InitTauler(num);      

}


void interrupt high_ISR(void) {

   if(TMR0IE && TMR0IF){
      TMR0H = 0x3C;
      TMR0L = 0xB0;
      TMR0IF = 0;
      decimes++;
      temps++;
   
      if(decimes==moure_estrella) {
	 
	 moure_estrelles();
	 decimes = 0;	 
	
      }
      
      if(temps == 10){
	 temps_total++;
	 temps=0;
	 writeNum(2, 19,temps_total);
	 if(temps_total == 180) {
	   temps_total=0;
	    is_finished = 1;   
	 }
      }    
   
      
   }
   
   if(TMR2IE && TMR2IF){
      TMR2IF = 0;
      cont++;
      if(cont==apareixe_estrella) {	
	new_star();
	cont = 0; 
      }
	
      if(special){
	 
	 mode_especial++;
	 
	 if(mode_especial==3000){
	    mode_especial=0;
	    special=0;
       	    valor_estrelles = 10;
	    c_estrella = '#';
	    pintar_tauler();
	    
	    TXSTA1bits.TXEN=1;
	    puts_usart1(s_desactivat);
	    
	 }
	 
      }
  
      
   }
   
   if(RC1IF && RC1IE) {
      
     char sub = RCREG1;
      if((sub =='s' || sub =='S')&&mode==1&&!special) {
	 
	 TXSTA1bits.TXEN=1;
	 puts_usart1(s_activat);
	 special=1;
	 valor_estrelles = 20;
	 c_estrella = '"';
	 pintar_tauler();
	 
      } else if((sub =='x' || sub =='X') && mode==0){
	 mode=1;
      } else { 
	 TXSTA1bits.TXEN=1;
	 puts_usart1(s_error);

      }
      
   }
   
     
}






void main(void)
{
	InitPIC(); 

	GLCDinit();		   //Inicialitzem la pantalla
	setStartLine(0);           //Definim linia d'inici
	
	while(1)
	   InitJoc();
   	   
}