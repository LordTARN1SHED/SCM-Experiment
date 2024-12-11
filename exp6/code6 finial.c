#include  <reg51.h>
#include "delay.h"
#include "main.h"
#include"reg51.h"
#include"stdio.h"

sbit ad_in=P2^0;
sbit ad_out=P2^1;
sbit ad_clk=P2^2;
sbit ad_eoc=P2^3;
sbit ad_cs=P2^4;

// 定义控制引脚
sbit CS = P0^0; // 寄存器选择
sbit SID = P0^1; // 读/写选择
sbit SCLK = P0^2; // 使能信号
sbit RST = P0^3; // 使能信号
sbit RS = P0^0; // 寄存器选择
sbit RW = P0^1; // 读/写选择

sbit BEEP=P1^0;
sbit WARN=P1^1;
sbit FAN=P1^2;

int T0count=0;
int T1count=0;
int timecount=0;
int timehold=0;
long int volt;
int mode=0;

void writebit(bit x){//写入一位
   SCLK=1;
   SID=x;
   SCLK=0;
}

void writebyte(unsigned char byte){//写入一字节
   int i=0;
   bit x=0;
   for(i=7;i>=0;i--){
      x=(byte >> i)&0x01;
      writebit(x);
   }
}

void writeword(unsigned char word){//写入一个字
   unsigned char x;
   unsigned char y;

   CS=1;
   writebyte(0x0FA);
   x=(word >> 4)&0x0F;
   y=word&0x0F;
   x=x<<4;
   y=y<<4;
   writebyte(x);
   writebyte(y);
   CS=0;
}

void writecmd(unsigned char cmd){// 向 LCD 发送命令
   unsigned char x;
   unsigned char y;
   CS=1;
   writebyte(0xF8);
   x=(cmd >> 4)&0x0F;
   y=cmd&0x0F;
   x=x<<4;
   y=y<<4;
   writebyte(x);
   writebyte(y);
   CS=0;
}

void LCD_Init() {// LCD 初始化
    
    delay_ms(40); // 上电后等待足够长的时间
    RST=0;
    delay_ms(5);
    RST=1;
    writecmd(0x01); // 清屏
    delay_ms(5);;
    writecmd(0x30); // 基本指令集
    delay_ms(5);
    writecmd(0x0C); // 显示开，光标关
    delay_ms(5);
    writecmd(0x06); //
    delay_ms(5);
    
    // 可以添加更多初始化设置
}

void ddram(unsigned char position){//确认字符在lcd上的起始位置
    RS=0;
    RW=0;
    writecmd(position);
}

void write(char str[]){//高层调用的函数，直接在函数内输入所需显示内容即可
    int i;
    for(i=0;str[i]!='\0';i++){
       RS=1;
       RW=0;
       writeword(str[i]);
       delay_ms(1);
    }
}

int adRead(unsigned int port){//test ad_in
    unsigned char i=0;
    int intVolt=0;
    ad_clk=0;
    ad_cs=0;
    for(i=0;i<12;i++){
        ad_in=port&0x80;//last 4 pos output is invalid, but still do this cause i'm to lazy to rewrite a for loop
        if(ad_out) intVolt+=1;
        ad_clk=1;
        delay_ms(3);
        ad_clk=0;
        delay_ms(3);
        port<<=1;
        intVolt*=2;
    }
    ad_cs=1;
    intVolt/=2;
    return intVolt;
}
int voltTrans(unsigned int volt, int maxValue){//转换电压
    volt=volt*maxValue;
    volt=volt/4096;
    return volt;
}

void toASCIIWrite(){//写入的内容转换成ASCII并进行显示
    int offset=0;
    char temp[3];
    offset=volt/100;
    temp[0]='0'+offset;
    offset=(volt%100)/10;
    temp[1]='0'+offset;
    offset=volt%10;
    temp[2]='0'+offset;
    write(temp);
}

void main(void)
{
    EA=1;
    ET0=1;
    ET1=1;
    TMOD=0x11;
    TH0=0x04c;//FC18 1MS
    TL0=0x0f7;//B1E0 20MS D8F0 10MS EC78 5MS 3CB0 50MS
    TH1=0xff;//
    TL1=0x00;//4cf7 50ms
    TR0=1;
    //TR1=1;

    LCD_Init(); // 初始化 LCD

     while(1){
        ddram(0x82);
           write("吉林大学");
        ddram(0x90);
           write("Jilin University");
        ddram(0x8B);
           if(volt<200) write("正常");
        else if(volt<600) write("警告");
        else write("危险");
        ddram(0x99);
           write("测量值：");
        toASCIIWrite();
    }
}
void TIMEOUT0() interrupt 1{
    TH0=0x04c;
    TL0=0x0f7;
    T0count++;
    if(T0count==20){
        T0count=0;
        volt=adRead(0x0000);
        volt*=1000;
        volt/=4096;
        if(volt<200){
            mode=0;
            TR1=0;
            WARN=1;
            FAN=1;
            timehold=0;
        }
        else if(volt<600){
            mode=1;
            TR1=1;
            FAN=0;
            timehold=0;
        }
        else{
            timehold%=5;
            if(timehold<=2){mode=2;}
            else{mode=3;}
            TR1=1;
            WARN=0;
            FAN=0;
            timehold++;
        }
    }
}
void TIMEOUT1() interrupt 3 using 2{
    TH1=0xff;
    TL1=0x00;
    T1count++;
    if(T1count==100){
        T1count=0;
        if(mode==1){
            WARN=!WARN;
        }
    }
    if(mode==2){
        BEEP=~BEEP;
    }
}
