#ifndef F_CPU
#define F_CPU 4000000UL//8000000UL	
#endif
//#define F_CPU 8000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include <stdio.h>
#define RX_COMPLETE (1<<RXC) //���� ����� ������ ������
#define FRAMING_ERROR (1<<FE) //������ ������ ���� ����
#define DATA_OVERFLOW (1<<OR)
//#define A 0

//#undef __OPTIMIZE__ //������� �������� ������� ������� � ������� ��������
//unsigned int height[7] = {4748, 4480, 4228, 3992, 3768, 3556, 3356};
//volatile 
unsigned int height[9] = //{2374, 2174, 1996, 1884, 1678, 1496, 1332, 2374};
	{120, 107, 95, 90, 80, 71, 63, 60, 0};// 3260, 3167, 3067, 2374};//2374};
//volatile 
unsigned int duration[4] = {1000, 500, 250, 125};


void uart_init(){
	USR = 0x0;//UCSRA = 0x00; //���������
	UCR = (1<<RXEN) | (1<<TXEN); //����������
	//UBRRH=0;	//	UBRR=f/(16*band)-1 f=8000000�� band=9600, 
	//UBRRL=51;
	UBRR = 11; //�������� 19219
	//UCSRC=0b10000110;   //  8-������� �������
}

unsigned char uart_receive(){
	while(!(USR&(1<<RXC)));
	return UDR;
}

void send_Uart(unsigned char c)//	�������� �����
{
	while(!(USR&(1<<UDRE)))	//	���������������, ����� ������� ��������
	{}
	UDR = c;
}

void send_Uart_str(unsigned char *s)//	�������� ������
{
	while (*s != 0) send_Uart(*s++);
}

void send_Uart_number(unsigned int c)//	�������� �����
{
	while(!(USR&(1<<UDRE)))	//	���������������, ����� ������� ��������
	{}
	UDR = c;
}

void send_int_Uart(unsigned int c)// �������� ����� �� 0000 �� 9999
{
	unsigned char temp;
	c=c%10000;
	temp=c/100;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
	temp=c%100;;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
}

void info_output(unsigned int num){
	send_Uart_str("Input ");
	switch (num){
		case 0: send_Uart_str("1st");
			break;
		case 1: send_Uart_str("2nd");
			break;
		case 2: send_Uart_str("3d");
			break;
		case 3: send_Uart_str("4th");
			break;
		case 4: send_Uart_str("5th");
			break;
		case 5: send_Uart_str("6th");
			break;
		default:
			break;
	}
		
	send_Uart_str(" melody:");//	�������� ������
	send_Uart(13);
}

char write_buffer[6][39];

void write_melody(int *notes_number, unsigned char buffer, int *write_index, unsigned char status, int *write_permit){
	//if (((status & FRAMING_ERROR) == 0)){// && ((status & RX_COMPLETE) == 0)){ //���������� ������ 
	//if (button_permit == 1){
		if (*write_index == -1){ //������ ������ �������
			*write_permit = 1;
			*write_index = 0;
	
			/*OCR1A = height[4];//4748;
			TCCR1A = 0x40;
			//duration_delay(duration[0]);			
			_delay_ms(50);
			TCCR1A = 0x00;*/
		}
		if ((buffer == 13) || (*write_index == 38)){ //�� ������� �� enter - ����� ������ �������
			write_buffer[*notes_number][*write_index] = '#';
			*write_permit = 0;
			*write_index = -1;
			*notes_number += 1;
			 // ������� ����� ������� - enter
			/*OCR1A = height[7];//4748;
			TCCR1A = 0x40;
			//duration_delay(duration[0]);			
			_delay_ms(50);
			TCCR1A = 0x00;*/

			//����� �������
			//unsigned char buf;
			//itoa(*notes_number, &buf, 10);
			//buf = (char)(*notes_number + 48);
			
			
			//info_output(*notes_number);
			/*send_Uart_str("Input ");
			switch (*notes_number){
				case 1: send_Uart_str("2nd");
				break;
				case 2: send_Uart_str("3d");
				break;
				case 3: send_Uart_str("4th");
				break;
				case 4: send_Uart_str("5th");
				break;
				case 5: send_Uart_str("6th");
				break;
				default:
				break;
			}
		
			send_Uart_str(" melody:");//	�������� ������
			send_Uart(13);*/
		}
		//write_index = -1;
		if (*write_permit == 1 && *write_index + 1 < 39 && buffer != 8){ // 41 ��� '#'
			if (*write_index % 2 == 0){
				switch (buffer){
					case 'a':
						write_buffer[*notes_number][*write_index] = '0';
						break;
					case 's':
						write_buffer[*notes_number][*write_index] = '1';
						break;
					case 'd':
						write_buffer[*notes_number][*write_index] = '2';
						break;
					case 'f':
						write_buffer[*notes_number][*write_index] = '3';
						break;
					case 'g':
						write_buffer[*notes_number][*write_index] = '4';
						break;
					case 'h':
						write_buffer[*notes_number][*write_index] = '5';
						break;
					case 'j':
						write_buffer[*notes_number][*write_index] = '6';
						break;
					case 'k':
						write_buffer[*notes_number][*write_index] = '7';
						break;
					case ' ':
						write_buffer[*notes_number][*write_index] = '8';
						break;
					default: break;
				}
			}
				//default:
				//write_buffer[*notes_number][*write_index] = buffer - 1;
			if (*write_index % 2 == 1){
				switch (buffer){
				case ('1'):
				case ('2'):
				case ('3'):
				case ('4'):
					write_buffer[*notes_number][*write_index] = buffer - 1;
					break;
				/*case '2':// '2' || '3' || '4'):
					write_buffer[*notes_number][*write_index] = buffer -  1;
					break;
				case '3':// '2' || '3' || '4'):
					write_buffer[*notes_number][*write_index] = buffer - 1;
					break;
				case '4':// '2' || '3' || '4'):
					write_buffer[*notes_number][*write_index] = buffer - 1;
					break;*/
				default:
					break;
				}
			}
			/*OCR1A = height[2];//4748;
			TCCR1A = 0x40;
			//duration_delay(duration[0]);			
			_delay_ms(50);
			TCCR1A = 0x00;*/
			*write_index += 1;
		}
		if (buffer == 8) //backspace
			*write_index -= 1;
	//}
}

unsigned int format_check(unsigned int num){
	for (int i = 0; i < 38; i++){
		if ((write_buffer[num][i] == '#' || i == 38)) break;
		if (i % 2 == 0 && !(write_buffer[num][i] == '0' || write_buffer[num][i] == '1' || 
						write_buffer[num][i] == '2' || write_buffer[num][i] == '3' || 
						write_buffer[num][i] == '4' || write_buffer[num][i] == '5' || 
						write_buffer[num][i] == '6' || write_buffer[num][i] == '7'
						|| write_buffer[num][i] == '8')){
			return 1;
			//error = 1;
			//break;
		}
		if (i % 2 == 1 && !(write_buffer[num][i] == '0' || write_buffer[num][i] == '1' || 
						write_buffer[num][i] == '2' || write_buffer[num][i] == '3')){
			return 1;
			//error = 1;
			//break;
		}
		
		
	}
	/*if (error == 0) 
		return 0;
	else 
		return 1;*/
	return 0;
}

void piano(unsigned char t){ //���������� 1-� ������
	unsigned int i = 0;
	for (i =  0; i < 8; i++){
		if ((t&1) == 0 /*&& height[i] != 0*/){
			//_delay_us(200);
			//while (!(t|1) == 0){
				
				OCR1A = height[i];//4748;
				TCCR1A = 0X40;
				//duration_delay(duration[0]);			
				_delay_ms(500);
				//_delay_us(200);
			//}
		} 
		t >>= 1;	
	}
	TCCR1A = 0X00; //���������� ��������
}

void duration_delay(int ms){ //������������

	while (ms > 0){
	//for (int i = 1; i < ms + 1; i++){
		_delay_ms(250);
		ms = ms - 125;
	}
	TCCR1A = 0X00; // ���������� �����
}

void note(int h){
	int note_h =0;
	note_h = h;//(int)h - 48;
	OCR1A = height[h];//4748;
	TCCR1A = 0X40;
}

void play_melody(unsigned int melody_number/*char record[41]*/){ //* - ������, # - �����, max - 20 ���
	unsigned int i = 0;
	unsigned int ms = 0;
	//int j = 0;
			_delay_ms(1000);
			while (write_buffer[melody_number][i] != '#'){
				// ������� ������
				if (write_buffer[melody_number][i] != '8')
					note(((int)write_buffer[melody_number][i] - 48));
				//��������� ����� � ��������

				//������������
				duration_delay(duration[((int)write_buffer[melody_number][i+1] - 48)]);
				//TCCR1A = 0X00; // ���������� �����

				// ��� �������
				/*OCR1A = height[j];//4748;
				TCCR1A = 0X40;
				
				ms = duration[3];//duration[((int)write_buffer[melody_number][i+1] - 48)];
				while (ms > 0){
				//for (int i = 1; i < ms + 1; i++){
					_delay_ms(125);
					ms -= 125;
				}
				TCCR1A = 0X00; // ���������� �����

				j += 1;*/
				//_delay_ms(500);
				duration_delay(duration[((int)write_buffer[melody_number][i+1] - 48)]);
				i += 2;
			}
}

int main(void)
{
	//sei();

	unsigned char temp;//, help;
	unsigned int i;
//	unsigned int j;

	PORTC = 0xff; //���������� 1-� ������
	DDRC = 0x00; //�� ����
	PORTA = 0xff; //������ ������ ������� � ������ �������
	DDRA = 0x00;
	PORTD = 0x00;
	DDRD = 0x20; //�� ����� PD5/OC1A - �������
//	PORTE = 0x00;
//	DDRE = 0x04; //�� ����� PE2/OC1B - �������
	PORTB = 0xf8; //PB0..PB2 - �������� ��� ���������
	DDRB = 0x07;
	TCCR1A = 0x00;
	TCCR1B = 0x0B;//0x09;
	for (int i = 0; i < 6; i++){
		write_buffer[i][0] = '#';
	}

	// ��� ����������� ������ ��� ������ ������ ����
	/*unsigned int F_clk = 4000000;
	unsigned int F_do1 = 262;
	unsigned int fl = F_clk /(F_do1 * 128);
	fl = fl + 1;
	height[0] = F_clk /(F_do1 * 128);
	//height[0] = 261;
	for (int i = 1; i < 8; i++){
		if ((i != 4) || (i != 8))
			height[i] = (int)((float)height[i - 1] / 1.12);
		else height[i] = (int)((float)height[i - 1] / 1.06);

	}
	F_do1 = fl;*/



	uart_init();				//	������������� UART
	_delay_ms(1000);			//	�������� 1c
	int melody_number = 0;
	int note_number = -1;

	send_Uart_str("Notes: Do - a, Re - s, Mi - d, Fa - f,");
	send_Uart(13);
	send_Uart_str("Sol - g, La - h, Si - j, Do - k, Pause - Space, the end - Enter");
	send_Uart(13);
	send_Uart_str("Durations: 1 - 1, 2 - 1/2, 3 - 1/4, 4 - 1/8");
	send_Uart(13);			//	������� ������
	info_output(melody_number);

	UCR &= ~(1<<RXEN);


	char status, data;// = UDR;
	status = USR; //USR

	int write_permit = 0;
	//unsigned int write_index = 0;
	//char write_buffer[41]; // 41 - ��� enter
	


	//unsigned int write_permit = 0;


//	unsigned int k = 0; // -----��� ��������

	unsigned char pc_arr[6] = {PC0, PC1, PC2, PC3, PC4, PC5};
	//int button_permit = 0;

	while(1)					//	����������� ������� ����
    {
		
		temp = PINC;
		
		if ((PINA&(1<<PA7))==0) { //���� ������ ������ �� ������
			uart_init();
			piano(temp);
			
			if(USR&(1<<RXC))		//	���� ������ ���� �� UART
			{
				data = uart_receive(); // <- status = UCSRA
				
				status = USR;//??
				
				write_melody(&melody_number, data, &note_number, status, &write_permit);
				/*while (format_check(melody_number - 1) == 0){
					melody_number -= 1;
					send_Uart_str(13);
					send_Uart_str("Wrong format!");
					send_Uart_str(13);
					info_output(melody_number);
					write_melody(&melody_number, data, &note_number, status, &write_permit);
				}*/
				if (data == 13){
					if (format_check(melody_number - 1) == 1){
						melody_number -= 1;
						//send_Uart_str(13);
						send_Uart_str("Wrong format!");
						send_Uart_str(13);
						info_output(melody_number);
						
						//write_melody(&melody_number, data, &note_number, status, &write_permit);
					}
					else
						info_output(melody_number);
					//info_output(melody_number);
				}
			}
			 // ������ �� �����
		} 
		//if (PINB&(1<<PB3)) {
		if (PINA&(1<<PA7)) { // ���� ������ ������
			UCR &= ~(1<<RXEN);
			for (i = 0; i < 6; i++){
				if ((PINC&(1<<pc_arr[i])) == 0)
					play_melody(i);
			}
		}
		
		// ������� ��������� ���������
		// 3831 - do, 3033 - mi, 2551 - sol'
		if ((PINB&(1<<PB3)) == 0) {
			/*PORTB |= (1<<PB0) | (1<<PB1) | (1<<PB2);//height[j];//4748;
			//TCCR1A = 0X10;
			//help >>= 1;
			_delay_us(2551);
			PORTB &= ~(1<<PB2);
			//_delay_us(2551);
			//PORTB |= (1<<PB1);//height[j];//4748;
			_delay_us(482);
			PORTB &= ~(1<<PB1);
			//_delay_us(482);
			//PORTB |= (1<<PB2);//height[j];//4748;
			_delay_us(798);
			PORTB &= ~(1<<PB0);*/
			//_delay_us(798);


			//_delay_ms(500);
			//OCR1A = height[2];//4748;



			//TCCR1A = 0X40;

			//_delay_ms(500);

			//TCCR1A = 0X00;


			//for (int aa = 0; aa < 5; aa++){
				PORTB |= (1<<PB0) | (1<<PB1);//PORTB = 0x01;
				_delay_us(3033);
				PORTB &= ~(1<<PB1);//PORTB = 0x01;
				_delay_us(798);
				PORTB &= ~(1<<PB0);//PORTB = 0x00;
				_delay_us(500);

				/*for (int a = 0; a < 5; a++){
					
					PORTB |= (1<<PB0);
					_delay_us(3831);
					PORTB &= ~(1<<PB0);
					_delay_us(3831);
				//_delay_ms(50);
				}*/

				//_delay_ms(500);

				/*OCR1B = 120;//4748;
				TCCR1A = 0X10;
				//duration_delay(duration[0]);			
				_delay_ms(500);
				TCCR1A = 0x00;*/
				
				/*OCR1A = 95;//4748;
				TCCR1A = 0X40;
				//duration_delay(duration[0]);			
				_delay_ms(500);
				TCCR1A = 0x00;*/

				

				PORTB |= (1<<PB1);
				_delay_us(3033);
				PORTB &= ~(1<<PB1);
				_delay_us(3033);
				_delay_ms(500);


				PORTB |= (1<<PB2);
				_delay_us(2551);
				PORTB &= ~(1<<PB2);
				_delay_us(2551);
				_delay_ms(500);

			//}
		}
				
    }





/*	while (1){
		temp = PINC;
		help = temp >> 4;;
		for (i = 0; i < 4; i++){
			if ((temp&1) == 0){
				OCR1A = height[i];//4748; ������ �������� ��������
				TCCR1A = 0X40; //OC1A ������������� � ��������������� ��������� �� ����������
				//help >>= 1;
				_delay_ms(50);


				
				for (j = 4; j < 8; j++){
					if ((help&1) == 0){
						OCR1B = height[j];//4748;
						TCCR1A = 0X10;
						//help >>= 1;
						_delay_ms(50);
					} 
					help >>= 1;
			
			
				}
			} 
			temp >>= 1;
			
			
		}
/*		for (j = 4; j < 8; j++){
					if ((help&1) == 0){
						//for (i = 0; i = 5; i++){
							PORTB = 0x00;
							_delay_us(261);
							PORTB = 0x01;
							_delay_us(261);
						//}
						OCR1A = height[0];
						TCCR1A = 0X40;
					}
					PORTB = 0x00;
					help >>= 1;
				}*/
		
/*		temp = PINC >> 4;
		for (j = 4; j < 8; j++){
			if ((temp&1) == 0){
				OCR1B = height[j];//4748;
				TCCR1A = 0X10;
				//help >>= 1;
				_delay_ms(500);
			} 
			temp >>= 1;
			
			
		}*/
		


		// ����� ��� ������� ����� ������ ������������� 2 ��������
/*		help = PINC;// >> 4;
		for (j = 4; j < 8; j++){
			if (((help >> j)&1) == 0){
				//for (i = 0; i = 5; i++){
					PORTB = 0x00;//height[j];//4748;
					//TCCR1A = 0X10;
					//help >>= 1;
					_delay_us(height_us[j]);//height_us[j]);
					PORTB = 0x01;
					_delay_us(height_us[j]);//height_us[j]);
				//}
				//OCR1A = height[0];//4748;
				//TCCR1A = 0X40;
				//_delay_ms(50);
			}
			PORTB = 0x00;
			//help >>= 1;  // � ���� �������� ������ �� ������������� ������-��
		}
		//�����

		TCCR1A = 0X00;
	}*/
}
