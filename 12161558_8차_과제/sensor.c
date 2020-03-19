#include<avr/io.h>
#define CDS_VALUE 871
void init_adc();
unsigned short read_adc();
void show_adc(unsigned short value);

int main(){
	unsigned short value;
	DDRA=0xff;
	init_adc();
	while(1){
		value=read_adc();
		show_adc(value);
	}
}

void init_adc(){
	ADMUX=0x00;
		//REFS(1:0)="00" AREF(+5V) �������л�� 
		//ADLAR='0' ����Ʈ ������ ����
		//MUX(4:0)="00000" ADC0 ���, �ܱ� �Է�
	ADCSRA=0x87;
		//ADEN='1' ADC�� Enable
		//ADFR='0' single conversion ���
		//ADPS(2:0)="111" ���������Ϸ� 128����
}

unsigned short read_adc(){
	unsigned char adc_low,adc_high;
	unsigned short value;
	ADCSRA |= 0x40;   //ADC start conversion, ADSC='1'
	while((ADCSRA&0x10)!=0x10); // ADC ��ȯ �Ϸ� �˻�
	adc_low=ADCL; //��ȯ�� Low �� �о����
	adc_high=ADCH;  //��ȯ�� High �� �о����
	value=(adc_high<<8) | adc_low;  //value�� High �� Low ���� 16��Ʈ ��
 	return value;
}

void show_adc(unsigned short value){
	if(value<CDS_VALUE) //���ذ� �̸��̸�
		PORTA=0xff; //���ε� (LED) ON
	else	//���ذ� �̻��̸�
		PORTA=0x00; 	//���ε� (LED) OFF
}
