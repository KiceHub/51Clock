#include<reg51.h>
#include<absacc.h>
//全局变量
code unsigned char LedCode[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
code unsigned char Ext[]={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF};
unsigned char year=20,mon=6,day=30,hour=23,min=59,sec=57;
unsigned int x=0,y=0,z=0xFF,set=0;

//声明时间递增函数
void timeinc(unsigned int m);

main()
{
//外中断初始化
IT0=1;PX0=1;EX0=1; //外中断0，优先级1
IT1=1;PX1=0;EX1=1; //外中断1，优先级0
//定时器0初始化
TMOD=TMOD|0x01;
TL0=(65536-50000)%256;
TH0=(65536-50000)/256;
TR0=1;
ET0=1;
//定时器1初始化
TMOD=TMOD|0x10;
TL1=(65536-50000)%256;
TH1=(65536-50000)/256;
TR1=0;
ET1=1;
//总中断开关
EA=1;
//其他初始化
P1=0xFF;
while(1)
	{
	//主任务
	XBYTE[0xFEFF]=LedCode[2];
	XBYTE[0xFDFF]=LedCode[0];
	XBYTE[0xFCFF]=LedCode[year/10];
	XBYTE[0xFBFF]=LedCode[year%10];	
	XBYTE[0xFAFF]=LedCode[mon/10];
	XBYTE[0xF9FF]=LedCode[mon%10];
	XBYTE[0xF8FF]=LedCode[day/10];
	XBYTE[0xF7FF]=LedCode[day%10];
	XBYTE[0xF6FF]=LedCode[hour/10];
	XBYTE[0xF5FF]=LedCode[hour%10];
	XBYTE[0xF4FF]=LedCode[min/10];
	XBYTE[0xF3FF]=LedCode[min%10];
	XBYTE[0xF2FF]=LedCode[sec/10];
	XBYTE[0xF1FF]=LedCode[sec%10];
	}
}

Eint0() interrupt 0
{
//外中断0任务，实现循环设置时间
//停止定时器0，启动定时器1
TR0=0;
TR1=1;
set++;
if(set>6)
	{
	set=0;
	//恢复定时器0，停止定时器1
	TR0=1;
	TR1=0;
	}
P1=0xFF;
}

Eint1() interrupt 2
{
//外中断1任务，实现设置项+1
if(set) timeinc(set);
}

Timer0() interrupt 1
{
TL0=(65536-50000)%256;
TH0=(65536-50000)/256;
//定时器0任务，实现sec+1
if(++x==10) 
	{
	x=0;
	timeinc(0);
	} 
}

Timer1() interrupt 3
{
TL0=(65536-50000)%256;
TH0=(65536-50000)/256;
//定时器1任务，实现设置项闪烁
if(++y==3) 
	{
	y=0;
	if(z==0xFF)	z=Ext[set];
	else z=0xFF;
	P1=z;
	}	
}

void timeinc(unsigned int m)
{
//判断当月的天数
unsigned int n;
if(mon==1||mon==3||mon==5||mon==7||mon==8||mon==10||mon==12) n=31;
else if(mon==4||mon==6||mon==9||mon==11) n=30;
else if(year%4==0&&mon==2) n=29;
else n=28;
//选择设定项并+1
switch(m)
	{
	case 0:
		{
		sec++;
		if(sec>59) {min++;sec=0;}
		if(min>59) {hour++;min=0;}
		if(hour>23) {day++;hour=0;}
		if(day>n) {mon++;day=1;}
		if(mon>12) {year++;mon=1;}
		if(year>99) {year=0;}
		break;
		}
	case 1:
		{
		if(++sec>59) sec=0;
		break;
		}
	case 2:
		{
		if(++min>59) min=0;
		break;
		}
	case 3:
		{
		if(++hour>23) hour=0;
		break;
		}
	case 4:
		{
		if(++day>31) day=1;
		break;
		}
	case 5:
		{
		if(++mon>12) mon=1;
		break;
		}
	case 6:
		{
		if(++year>99) year=0;
		break;
		}
	}
}
