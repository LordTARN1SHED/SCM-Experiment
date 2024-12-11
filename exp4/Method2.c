#include  <reg51.h>
#include "delay.h"
#include "main.h"

char s[15] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x40,0x46,0x70 };//��������

int f1 = 0x0401;//������ʾ������
int f2 = 0x0201;//�����ʾ������

sbit k1 = P1 ^ 6;
sbit k2 = P1 ^ 7;

sbit din = P1 ^ 1;
sbit dclk = P1 ^ 2;
sbit load = P1 ^ 3;
//��ɶ˿ڳ�ʼ������
int counter = 0;

int spinclockwise() {//�������˳ʱ����ת����
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
int spinanticlockwise() {//���������ʱ����ת����
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

int showbit(int bitt, int x) {//��ʾһλ����
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

int show(int counter) {//��ʾ����
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

	if (k2) {//�ж�˳ʱ�뻹����ʱ��
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
	//�жϳ�ʼ��
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
	//�����ʾ�Ŀ��������
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
	//������ʾ�Ŀ��������
	TR0 = 1;//���ж�
	while (1){}//����ѭ���ȴ�ִ���жϴ������
}

int time0() interrupt 1{
	TR0 = 0;//���ж�
	show(counter);
	if (k1) {//�ж��Ƿ���ת
		if (k2) {
			spinclockwise();
		}
		else {//�ж�˳ʱ�뻹����ʱ��
			spinanticlockwise();
		}
	}
	else {}
	TR0 = 1;//���ж�
}