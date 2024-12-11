#include  <reg51.h>
#include "delay.h"
#include<stdio.h>


char s[12][8]={
{0,1,1,1,1,1,1,1}//0
,{0,0,0,0,0,1,1,0}//1
,{0,1,0,1,1,0,1,1}//2
,{0,1,0,0,1,1,1,1}//3
,{0,1,1,0,0,1,1,0}//4
,{0,1,1,0,1,1,0,1}//5
,{0,1,1,1,1,1,0,1}//6
,{0,0,0,0,0,1,1,1}//7
,{0,1,1,1,1,1,1,1}//8
,{0,1,1,0,1,1,1,1}//9
,{0,0,0,0,0,0,0,0}//
,{0,1,0,0,0,0,0,0}//-
};

sbit k1=P1^6;
sbit k2=P1^7;

sbit din=P1^1;
sbit dclk=P1^2;
sbit load=P1^3;

int counter=0;

int spinclockwise(){
        counter++;
        P0 = 0x06; 			
		delay_ms(10);
		P0 = 0x07; 			
		delay_ms(10);
		P0 = 0x03; 			
		delay_ms(10);
		P0 = 0x0b; 			
		delay_ms(10);
		P0 = 0x09; 			
		delay_ms(10);
		P0 = 0x0d; 			
		delay_ms(10);
		P0 = 0x0c; 			
		delay_ms(10);
		P0 = 0x0e; 			
		delay_ms(10);
		return 0;
}
int spinanticlockwise(){
        counter--;
        P0 = 0x0e; 			
		delay_ms(10);
		P0 = 0x0c; 			
		delay_ms(10);
		P0 = 0x0d; 			
		delay_ms(10);
		P0 = 0x09; 			
		delay_ms(10);
		P0 = 0x0b; 			
		delay_ms(10);
		P0 = 0x03; 			
		delay_ms(10);
		P0 = 0x07; 			
		delay_ms(10);
		P0 = 0x06; 			
		delay_ms(10);
		return 0;
}

int showbit(int bitt,int x){

int i;
     din=1;
     dclk=0;
     dclk=1;
	 din=1;
     dclk=0;
     dclk=1;
	 if(x==1||x==2){
	 din=1;
	 }else{
	 din=0;
	 }
     dclk=0;
     dclk=1;
	 if(x==1||x==3){
	 din=1;
	 }else{
	 din=0;
	 };
     dclk=0;
     dclk=1;
  for(i=0;i<8;i++){
     din=s[bitt][i];
     dclk=0;
     dclk=1;
  }

   load=0;
   load=1;

   return 0;
}


int main(){

    EA=1;
	ET0=1;
	TMOD=0x02;
	TH0=0x00;
	TR0=1;
    
    
	
 	while(1)
	{
	    //show0();
	    //spinonce();
	}
	return 0;
}

int show(int counter){
   int bitt=0;
   while(counter%10!=0){
      bitt++;
	  counter--;
   }
   showbit(bitt,1);
   bitt=0;

   counter=counter/10;
   while(counter%10!=0){
      bitt++;
	  counter--;
   }
   showbit(bitt,2);
   bitt=0;

   counter=counter/10;
   while(counter%10!=0){
      bitt++;
	  counter--;
   }
   showbit(bitt,3);
   bitt=0;

   counter=counter/10;
   while(counter%10!=0){
      bitt++;
	  counter--;
   }
   showbit(bitt,4);
   
   return 0;
}

int time0() interrupt 1{
    show(counter);
    //show0();
    if(k1){
	   if(k2){
          spinclockwise(); 
       }else{
	      spinanticlockwise();
	    } 
	}else{}
} 