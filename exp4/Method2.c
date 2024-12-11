#include  <reg51.h>
#include "delay.h"
#include "main.h"

char s[15] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x40,0x46,0x70 };//输出字码表

int f1 = 0x0401;//开启显示功能码
int f2 = 0x0201;//清除显示功能码

sbit k1 = P1 ^ 6;
sbit k2 = P1 ^ 7;

sbit din = P1 ^ 1;
sbit dclk = P1 ^ 2;
sbit load = P1 ^ 3;
//完成端口初始化定义
int counter = 0;

int spinclockwise() {//步进电机顺时针旋转函数
	counter++;
	P0 = 0x06;
	delay_ms(5);
	P0 = 0x07;
	delay_ms(5);
	P0 = 0x03;
	delay_ms(5);
	P0 = 0x0b;
	delay_ms(5);
	P0 = 0x09;
	delay_ms(5);
	P0 = 0x0d;
	delay_ms(5);
	P0 = 0x0c;
	delay_ms(5);
	P0 = 0x0e;
	delay_ms(5);
	return 0;
}
int spinanticlockwise() {//步进电机逆时针旋转函数
	counter--;
	P0 = 0x0e;
	delay_ms(5);
	P0 = 0x0c;
	delay_ms(5);
	P0 = 0x0d;
	delay_ms(5);
	P0 = 0x09;
	delay_ms(5);
	P0 = 0x0b;
	delay_ms(5);
	P0 = 0x03;
	delay_ms(5);
	P0 = 0x07;
	delay_ms(5);
	P0 = 0x06;
	delay_ms(5);
	return 0;
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
		showbit(11, 5);
		for (k = 0; k < 5; k++) {
			bitt = (-counter) % 10;
			showbit(bitt, k);
			counter = counter / 10;
		}
	}
	else {
		showbit(10, 5);
		for (k = 0; k < 5; k++) {
			bitt = counter % 10;
			showbit(bitt, k);
			counter = counter / 10;
		}
	}

	if (k2) {//判断顺时针还是逆时针
		showbit(12, 7);
	}
	else {
		showbit(13, 7);
	}

	return 0;
}


void main(void) {
	int i;
	int v;
	int command;
	EA = 1;
	ET0 = 1;
	TMOD = 0x02;
	TH0 = 0x0f;
	//中断初始化
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
	TR0 = 1;//开中断
	while (1){}//无限循环等待执行中断处理程序
}

int time0() interrupt 1{
	TR0 = 0;//关中断
	show(counter);
	if (k1) {//判断是否旋转
		if (k2) {
			spinclockwise();
		}
		else {//判断顺时针还是逆时针
			spinanticlockwise();
		}
	}
	else {}
	TR0 = 1;//开中断
}