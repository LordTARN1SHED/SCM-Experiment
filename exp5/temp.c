#include "delay.h"
#include"reg51.h"
#include"stdio.h"
#include "main.h"

char s[15] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x40,0x46,0x70 };//输出字码表

int f1 = 0x0401;//开启显示功能码
int f2 = 0x0201;//清除显示功能码

sbit din = P1 ^ 1;
sbit dclk = P1 ^ 2;
sbit load = P1 ^ 3;
//完成端口初始化定义
int counter = 0;

//  定义位变量DQ，作为与18B20通信的单总线
//  目前使用P0.0，可以根据需要自行修改
sbit 		DQ = P0^0;			
#define 	DQ_H	DQ = 1;
#define 	DQ_L	DQ = 0;

//函数原型声明，可以提出为单个头文件
void reset_ds18b20(void);
void write_byte(unsigned char dat);
unsigned char read_byte(void);

/*
执行时间：约等于10*n微秒，当主频为12MHz的时候，增加一个nop
如果考虑到其他语句的执行，可以再减掉一个nop
*/
void delay_10us(unsigned char n){
	unsigned char i;
	for(i=n; i>0; --i){
		i=i;
		//_nop_(); //主频为12MHz
	}
}


/**********************************************************************************************************
函数名称：DS18B20初始化子程序
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/

void reset_ds18b20(void){
  	DQ_H;          		//  DQ先置高
	delay_10us(10);     //  延时
  	DQ_L;             	//  发送复位脉冲
	delay_10us(60);     //  延时（480us - 960us)
  	DQ_H;          		//  拉高数据线
	delay_10us(24);     //  DS18B20会产生应答脉冲，持续60-240us
}


/**********************************************************************************************************
函数名称：写一字节数据到DS18B20
输入参数：数据
输出参数：无
函数返回：无
**********************************************************************************************************/
void write_byte(unsigned char dat){ 
	unsigned char i=0; 		
	for (i = 8; i > 0; i--){
		//为避免判断的时间加入到时序中，将判断放到外面
		if(dat & 0x01)
        {
			DQ_L;
			i++; i--; //延时 2us
            DQ_H;   //释放总线
			delay_10us(6);
        }
        else
        {
			DQ_L;
			delay_10us(6);
            DQ_H;
			i++; i--; //延时 2us
        }
		dat>>=1;
	}
	delay_10us(3);
}
/**********************************************************************************************************
函数名称：从ds18b20读一字节数据函数
输入参数：无
输出参数：无
函数返回：读出的字节
**********************************************************************************************************/

unsigned char read_byte(void)  			
{
	unsigned char i = 0; 		
	unsigned char dat = 0;
	bit b;
    //先读低位再读高位
    for (i = 8; i > 0; i--)  		
	{
		DQ_L;
		i++; i--; //延时 2us
		DQ_H;  //释放总线
		dat >>= 1;  //必须在15us内读数据, 用这个表达式的时间占位
		b= DQ;

    	if (DQ){
            dat|=0x80;
        }

		delay_10us(6);
	}
    return(dat);
}


float hexToFloat(unsigned char highByte, unsigned char lowByte) {
    // Combine the two 8-bit values into a 16-bit integer
    unsigned int combinedValue = (highByte << 8) | lowByte;

    // Convert the 16-bit integer to a float
    float result = *(float*)&combinedValue;

    return result;
}

int showbit(int bitt, int x) {//显示一位数字
	int i;
	char k = s[bitt];

	load = 0;

	for (i = 0; i < 8; i++) {
		if (k & 0x01) din = 1;
		else din = 0;
		dclk = 0;
		k >>= 1;
		dclk = 1;
	}

	for (i = 0; i < 3; i++) {
		din = x % 2;
		x = x / 2;
		dclk = 0;
		dclk = 1;
	}

	din = 1;
	dclk = 0;
	dclk = 1;

	load = 1;

	return 0;
}

int show(int counter) {//显示数字
	int bitt;
	int k;

	if (counter < 0) {
		//showbit(11, 5);
		for (k = 0; k < 8; k++) {
			bitt = (-counter) % 10;
			showbit(bitt, k);
			counter = counter / 10;
		}
	}
	else {
		//showbit(10, 5);
		for (k = 0; k < 8; k++) {
			bitt = counter % 10;
			showbit(bitt, k);
			counter = counter / 10;
		}
	}

	return 0;
}

unsigned int temper_change(int t)
{
    unsigned int temper;
    float tp;
    temper=t;
    if(temper<0)    //考虑负温度的情况
    {
        temper=temper-1;
        temper=~temper;
        tp=temper*0.0625;  //16位温度转换成10进制的温度
        temper=tp*100+0.5;   //留两个小数点，并四舍五入
    }
    else
    {
        tp=temper*0.0625;  //16位温度转换成10进制的温度
        temper=tp*100+0.5;  //留两个小数点，并四舍五入
    }
    return temper;
}


void main(void) {
    unsigned char x1=0;
	unsigned char x2=0;
    int x=0;
	unsigned int t=0;

	int i;
	int v;
	int command;


	
	din = 0;
	din = 1;

	load = 0;
	command = f2;
	for (i = 0; i < 12; i++) {
		if (command & 0x0001) din = 1;
		else din = 0;
		dclk = 0;
		command >>= 1;
		dclk = 1;
	}
	load = 1;
	//清除显示的控制字输出
	load = 0;
	command = f1;
	for (i = 0; i < 12; i++) {
		if (command & 0x0001) din = 1;
		else din = 0;
		dclk = 0;
		command >>= 1;
		dclk = 1;
	}
	load = 1;
	//开启显示的控制字输出

    while(1){
	    reset_ds18b20();
		//read_byte();
		write_byte(0xCC);
		write_byte(0x44);
		DQ_H;          		//  DQ先置高
	    delay_10us(60000);     //  延时
		reset_ds18b20();
		//read_byte();
		write_byte(0xCC);
		write_byte(0xBE);
		x1=read_byte();
		x2=read_byte();
		//x=hexToFloat(x1,x2);
		t=x2;
		t<<=8;
	    t=t|x1;
		x=temper_change(t);
        show(x);
		//printf("%bu,%bu",x1,x2);
	}

}