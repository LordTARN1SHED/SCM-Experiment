#include "delay.h"
#include"reg51.h"
#include"stdio.h"
#include "main.h"

//  ����λ����DQ����Ϊ��18B20ͨ�ŵĵ�����
//  Ŀǰʹ��P0.0�����Ը�����Ҫ�����޸�
sbit 		DQ = P0 ^ 0;
#define 	DQ_H	DQ = 1;
#define 	DQ_L	DQ = 0;

//����ԭ���������������Ϊ����ͷ�ļ�
void reset_ds18b20(void);
void write_byte(unsigned char dat);
unsigned char read_byte(void);

/*
ִ��ʱ�䣺Լ����10*n΢�룬����ƵΪ12MHz��ʱ������һ��nop
������ǵ���������ִ�У������ټ���һ��nop
*/
void delay_10us(unsigned char n) {
	unsigned char i;
	for (i = n; i > 0; --i) {
		i = i;
		//_nop_(); //��ƵΪ12MHz
	}
}


/**********************************************************************************************************
�������ƣ�DS18B20��ʼ���ӳ���
�����������
�����������
�������أ���
**********************************************************************************************************/

void reset_ds18b20(void) {
	DQ_H;          		//  DQ���ø�
	delay_10us(10);     //  ��ʱ
	DQ_L;             	//  ���͸�λ����
	delay_10us(60);     //  ��ʱ��480us - 960us)
	DQ_H;          		//  ����������
	delay_10us(24);     //  DS18B20�����Ӧ�����壬����60-240us
}


/**********************************************************************************************************
�������ƣ�дһ�ֽ����ݵ�DS18B20
�������������
�����������
�������أ���
**********************************************************************************************************/
void write_byte(unsigned char dat) {
	unsigned char i = 0;
	for (i = 8; i > 0; i--) {
		//Ϊ�����жϵ�ʱ����뵽ʱ���У����жϷŵ�����
		if (dat & 0x01)
		{
			DQ_L;
			i++; i--; //��ʱ 2us
			DQ_H;   //�ͷ�����
			delay_10us(6);
		}
		else
		{
			DQ_L;
			delay_10us(6);
			DQ_H;
			i++; i--; //��ʱ 2us
		}
		dat >>= 1;
	}
	delay_10us(3);
}
/**********************************************************************************************************
�������ƣ���ds18b20��һ�ֽ����ݺ���
�����������
�����������
�������أ��������ֽ�
**********************************************************************************************************/

unsigned char read_byte(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	bit b;
	//�ȶ���λ�ٶ���λ
	for (i = 8; i > 0; i--)
	{
		DQ_L;
		i++; i--; //��ʱ 2us
		DQ_H;  //�ͷ�����
		dat >>= 1;  //������15us�ڶ�����, ��������ʽ��ʱ��ռλ
		b = DQ;

		if (DQ) {
			dat |= 0x80;
		}

		delay_10us(6);
	}
	return(dat);
}


unsigned int temper_change(int t)
{
	unsigned int temper;
	float tp;
	temper = t;
	if (temper < 0)    //���Ǹ��¶ȵ����
	{
		temper = temper - 1;
		temper = ~temper;
		tp = temper * 0.0625;  //16λ�¶�ת����10���Ƶ��¶�
		temper = tp * 100 + 0.5;   //������С���㣬����������
	}
	else
	{
		tp = temper * 0.0625;  //16λ�¶�ת����10���Ƶ��¶�
		temper = tp * 100 + 0.5;  //������С���㣬����������
	}
	return temper;
}


int readtemper(void) {
	unsigned char x1 = 0;
	unsigned char x2 = 0;
	int x = 0;
	unsigned int t = 0;

	reset_ds18b20();
	write_byte(0xCC);
	write_byte(0x44);
	DQ_H;          		//  DQ���ø�
	delay_10us(60000);     //  ��ʱ
	reset_ds18b20();
	write_byte(0xCC);
	write_byte(0xBE);
	x1 = read_byte();
	x2 = read_byte();
	t = x2;
	t <<= 8;
	t = t | x1;
	x = temper_change(t);

	return x;
}