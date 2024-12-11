#include"delay.h"
#include"reg51.h"
#include"stdio.h"

sbit SHCP=P1^1;//位移输入时钟
sbit STCP=P1^2;//写入时钟
sbit DATAS=P1^3;//数据线
sbit A0=P0^0;
sbit A1=P0^1;
sbit A2=P0^2;
sbit A3=P0^3;

unsigned int x[16];

unsigned char decimalToBinary(unsigned int i) {
    unsigned char binaryValue = 0;
    unsigned char bitPosition = 0;

    while (i > 0) {
        if (i % 2 == 1) {
            // 如果当前最低位是1，设置对应的位为1
            binaryValue |= (1 << bitPosition);
        }
        
        // 右移一位，处理下一位
        i >>= 1;
        bitPosition++;
    }

    return binaryValue;
}

int writebyte(unsigned char a){
    int b[8];
    int i=0;
    for(i=0;i<8;i++){
        b[i]=a&1;
        a=a>>1;
    }
    for(i=7;i>=0;i++){
        SHCP=0;
        DATAS=b[i];
        SHCP=1;
    }
    STCP=0;
    NOP();
    STCP=1;
    return 0;
}

int outp0(int i){
    switch (i) {
        case 0:
            A0=1;
            A1=1;
            A2=1;
            A3=1;
            break;
        case 1:
            A0=1;
            A1=1;
            A2=1;
            A3=0;
            break;
        case 2:
            A0=1;
            A1=1;
            A2=0;
            A3=1;
            break;
        case 3:
            A0=1;
            A1=1;
            A2=0;
            A3=0;
            break;
        case 4:
            A0=1;
            A1=0;
            A2=1;
            A3=1;
            break;
        case 5:
            A0=1;
            A1=0;
            A2=1;
            A3=0;
            break;
        case 6:
            A0=1;
            A1=0;
            A2=0;
            A3=1;
            break;
        case 7:
            A0=1;
            A1=0;
            A2=0;
            A3=0;
            break;
        case 8:
            A0=0;
            A1=1;
            A2=1;
            A3=1;
            break;
        case 9:
            A0=0;
            A1=1;
            A2=1;
            A3=0;
            break;
        case 10:
            A0=0;
            A1=1;
            A2=0;
            A3=1;
            break;
        case 11:
            A0=0;
            A1=1;
            A2=0;
            A3=0;
            break;
        case 12:
            A0=0;
            A1=0;
            A2=1;
            A3=1;
            break;
        case 13:
            A0=0;
            A1=0;
            A2=1;
            A3=0;
            break;
        case 14:
            A0=0;
            A1=0;
            A2=0;
            A3=1;
            break;
        case 15:
            A0=0;
            A1=0;
            A2=0;
            A3=0;
            break;
            
        default:
            break;
    }
	return 0;
}


int showmatrix(){
    int i;
    for(i=0;i<16;i++){
        unsigned char lowByte = (unsigned char)(x[i] & 0xFF);
        unsigned char highByte = (unsigned char)((x[i] >> 8) & 0xFF);
        writebyte(lowByte);
        writebyte(highByte);
        outp0();
        //P0=decimalToBinary(i);
        delay_ms(10);
    }
    return 0;
}

int main(){
	int i=0;
    for(i=0;i<16;i++){
		x[i]=0xffff;
	}
	while(1){
		showmatrix();
	}
	
    return 0;
}
