#ifndef F_CPU
#define F_CPU 4000000UL//8000000UL	
#endif
//#define F_CPU 8000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include <stdio.h>
#define RX_COMPLETE (1<<RXC) //флаг конца приема данных
#define FRAMING_ERROR (1<<FE) //ошибка чтения стоп бита
#define DATA_OVERFLOW (1<<OR)
//#define A 0

//#undef __OPTIMIZE__ //попытка передать элемент массива в функцию задержки
//unsigned int height[7] = {4748, 4480, 4228, 3992, 3768, 3556, 3356};
//volatile 
unsigned int height[9] = //{2374, 2174, 1996, 1884, 1678, 1496, 1332, 2374};
	{120, 107, 95, 90, 80, 71, 63, 60, 0};// 3260, 3167, 3067, 2374};//2374};
unsigned int height_delay[9] = 
	{1911, 1702, 1516, 1432, 1275, 1136, 1012, 9555};
//volatile 
unsigned int duration[4] = {1600, 800, 400, 200};//{1000, 500, 250, 125};


void uart_init(){
	USR = 0x0;//UCSRA = 0x00; //состояние
	UCR = (1<<RXEN) | (1<<TXEN); //управление
	//UBRRH=0;	//	UBRR=f/(16*band)-1 f=8000000Гц band=9600, 
	//UBRRL=51;
	UBRR = 11; //скорость 19219
	//UCSRC=0b10000110;   //  8-битовая посылка
}

unsigned char uart_receive(){
	while(!(USR&(1<<RXC)));
	return UDR;
}

void send_Uart(unsigned char c)//	Отправка байта
{
	while(!(USR&(1<<UDRE)))	//	Устанавливается, когда регистр свободен
	{}
	UDR = c;
}

void send_Uart_str(unsigned char *s)//	Отправка строки
{
	while (*s != 0) send_Uart(*s++);
}

void send_Uart_number(unsigned int c)//	Отправка байта
{
	while(!(USR&(1<<UDRE)))	//	Устанавливается, когда регистр свободен
	{}
	UDR = c;
}

void send_int_Uart(unsigned int c)// отправка числа от 0000 до 9999
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
		
	send_Uart_str(" melody:");//	отправка строки
	send_Uart(13);
}

char write_buffer[6][39];
int sym_count;
char prev_buf; //предыдущий символ при считывании

void write_melody(int *notes_number, unsigned char buffer, int *write_index, unsigned char status, int *write_permit, int mode){
		int note_drt = 0; // для режима 1
		char check_sym = buffer;
		if (*write_index == -1){ //начало записи мелодии
			if (mode == 0) 
				*write_permit = 1;
			*write_index = 0;
			sym_count = 0; // -- начало записи
			prev_buf = ' ';
		}
		if ((mode == 0) && (*write_permit == 1) && ((buffer == 13) || (*write_index == 38))){ //по нажатию на enter - конец записи мелодии
			write_buffer[*notes_number][*write_index] = '#';
			*write_permit = 0;
			*write_index = -1;
			*notes_number += 1;	
			// выход из функции		
		}
		// запись по "интервалам" зажатия клавиш
		if (mode == 1){
			// количество введенных одинаковых символов
			if ((buffer == prev_buf)){ // количество введенных символов
			
				sym_count += 1;
				*write_permit = 0;
			}
			// пришла новая нота, можно записывать предыдущую
			if ((buffer != prev_buf) && (prev_buf != ' ')){
				note_drt = sym_count;
				sym_count = 1; 
				check_sym = prev_buf;
				prev_buf = buffer;
				*write_permit = 1;
			} 
			// когда пришел самый первый символ
			if ((sym_count == 0)){ // в случае ввода по длительности нажатия, запоминаем появившийся символ
				prev_buf = buffer; //запоминаем символ
				sym_count = 1;
				//*write_index += 1;
				*write_permit = 0;
			}
		}
		
		if ((*write_permit == 1) && (*write_index + 1 < 39) && (check_sym != 8)){ // 41 для '#'
			if (*write_index % 2 == 0){
				
				switch (check_sym){
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
				if (mode == 1){ // определение длительности
					*write_index += 1;
					if (note_drt == 1)
						write_buffer[*notes_number][*write_index] = '0'; //целая нота
					if ((note_drt > 1) && (note_drt <= 4))
						write_buffer[*notes_number][*write_index] = '1'; //целая нота
					if ((note_drt >= 5) && (note_drt <= 8))
						write_buffer[*notes_number][*write_index] = '2'; //целая нота
					if (note_drt > 8)
						write_buffer[*notes_number][*write_index] = '3'; //целая нота
					*write_index += 1;	
					// конеу мелодии
					if (((buffer == 13) && (*write_permit == 1)) || (*write_index == 38)){ 
						write_buffer[*notes_number][*write_index] = '#';
						*write_permit = 0;
						*write_index = -1;
						*notes_number += 1;	
						// выход из функции		
					}
				}
				

			}
				//default:
				//write_buffer[*notes_number][*write_index] = buffer - 1;
			// запись длительности
			if (mode == 0) {
				if (*write_index % 2 == 1){
					switch (buffer){
					case ('1'):
					case ('2'):
					case ('3'):
					case ('4'):
						write_buffer[*notes_number][*write_index] = buffer - 1;
						break;
					default:
						break;
					}
				}
			
				*write_index += 1;
				
			}
		}
	

		if ((buffer == 8) && (mode == 0)) // при удалении символов с помощью backspace
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

void piano(unsigned char t){ //клавиатура 1-й октавы
	//unsigned int i = 0;
	/*for (int i =  0; i < 8; i++){
		if ((t&1) == 0){
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
	}*/

	// 1911, 1702, 1516, 1432, 1275
	//(PINC&(1<<pc_arr[i]))
	//try to play chord
	if (((t&(1<<PA0)) == 0) && ((t&(1<<PA2)) == 0) && ((t&(1<<PA4)) == 0)&& ((t&(1<<PA7)) == 0)){
	// doesn't work with 4th note (c2)
		for (int i = 0; i < 5; i++){
			PORTD |= (1<<PD5);
			_delay_us(1911);//3831);
			PORTD &= ~(1<<PD5);
			_delay_us(1911);
		}
		for (int i = 0; i < 5; i++){
			PORTD |= (1<<PD5);
			_delay_us(1516);//3831);
			PORTD &= ~(1<<PD5);
			_delay_us(1516);
		}
		for (int i = 0; i < 5; i++){
			PORTD |= (1<<PD5);
			_delay_us(1275);//3831);
			PORTD &= ~(1<<PD5);
			_delay_us(1275);
		}
		for (int i = 0; i < 5; i++){
			PORTD |= (1<<PD5);
			_delay_us(9555);//3831);
			PORTD &= ~(1<<PD5);
			_delay_us(9555);
		}

	} else 
		if (((t&(1<<PA0)) == 0) && ((t&(1<<PA2)) == 0) && ((t&(1<<PA4)) == 0)){
			for (int i = 0; i < 5; i++){
				PORTD |= (1<<PD5);
				_delay_us(1911);//3831);
				PORTD &= ~(1<<PD5);
				_delay_us(1911);
			}
			for (int i = 0; i < 5; i++){
				PORTD |= (1<<PD5);
				_delay_us(1516);//3831);
				PORTD &= ~(1<<PD5);
				_delay_us(1516);
			}
			for (int i = 0; i < 5; i++){
				PORTD |= (1<<PD5);
				_delay_us(1275);//3831);
				PORTD &= ~(1<<PD5);
				_delay_us(1275);
			}
		} else 
			if (((t&(1<<PA0)) == 0) && ((t&(1<<PA2)) == 0)){
				for (int i = 0; i < 5; i++){
					PORTD |= (1<<PD5);
					_delay_us(1911);//1911);//3831);
					PORTD &= ~(1<<PD5);
					_delay_us(1911);
				}
				for (int i = 0; i < 5; i++){
					PORTD |= (1<<PD5);
					_delay_us(1516);//1516);//3831);
					PORTD &= ~(1<<PD5);
					_delay_us(1516);
				}
			} else ////////////
				if (((t&(1<<PA0)) == 0) && ((t&(1<<PA4)) == 0)){
					/*for (int i = 0; i < 5; i++){
						PORTD |= (1<<PD5);
						_delay_us(1911);//3831);
						PORTD &= ~(1<<PD5);
						_delay_us(1911);
					}
					for (int i = 0; i < 5; i++){
						PORTD |= (1<<PD6);
						_delay_us(1516);//3831);
						PORTD &= ~(1<<PD6);
						_delay_us(1516);
					}*/
					_delay_us(200);
					//while (!(t|1) == 0){
				
					OCR1A = height[0];//4748;
					_delay_ms(500);
					OCR1A = height[4];
					TCCR1A = 0X50;
					//duration_delay(duration[0]);			
					_delay_ms(500);
					
					TCCR1A = 0X00; //отключение динамика
					
					// _delay_us(200);
					//while (!(t|1) == 0){
				
					//OCR1A = height[2];//4748;
					//TCCR1A = 0X40;
					//duration_delay(duration[0]);			
					//_delay_ms(500);


				} else 
				for (int i =  0; i < 8; i++){
					if ((t&1) == 0){
						_delay_us(200);
						//while (!(t|1) == 0){
				
							OCR1A = height[i];//4748;
							TCCR1A = 0X40;
							//duration_delay(duration[0]);			
							_delay_ms(500);
							//_delay_us(200);
						//}
					} 
					TCCR1A = 0X00; //отключение динамика
					t >>= 1;	
				}

		//only with PD5
		/*for (int i = 0; i < 5; i++){
			PORTD |= (1<<PD5);
			_delay_us(1911);//3831);
			PORTD &= ~(1<<PD5);
			_delay_us(1911);
		}
		for (int i = 0; i < 5; i++){
			PORTD |= (1<<PD5);
			_delay_us(1516);//3831);
			PORTD &= ~(1<<PD5);
			_delay_us(1516);
		}
		for (int i = 0; i < 5; i++){
			PORTD |= (1<<PD5);
			_delay_us(1275);//3831);
			PORTD &= ~(1<<PD5);
			_delay_us(1275);
		}*/
		

		//does't sound a chord with timer
		/*
		for (int i = 0; i < 5; i++){
			OCR1A = height[0];//4748;
			TCCR1A = 0X40;
		//duration_delay(duration[0]);			
			_delay_ms(500);
		}
		for (int i = 0; i < 5; i++){
			OCR1A = height[2];//4748;
			TCCR1A = 0X40;
		//duration_delay(duration[0]);			
		_delay_ms(500);
		}
		for (int i = 0; i < 5; i++){
			OCR1A = height[4];//4748;
			TCCR1A = 0X40;
		//duration_delay(duration[0]);			
		_delay_ms(500);
		}*/		
	//}


	 
}

void duration_delay(int ms){ //длительности

	while (ms > 0){
	//for (int i = 1; i < ms + 1; i++){
		_delay_ms(300);//250);
		ms = ms - 200;//125;
	}
	TCCR1A = 0X00; // выключение звука
}

void note(int h){
	int note_h =0;
	note_h = h;//(int)h - 48;
	OCR1A = height[h];//4748;
	TCCR1A = 0X40;
}

void play_melody(unsigned int melody_number/*char record[41]*/){ //* - начало, # - конец, max - 20 нот
	unsigned int i = 0;
	unsigned int ms = 0;
	//int j = 0;
			_delay_ms(1000);
			while (write_buffer[melody_number][i] != '#'){
				// задание высоты
				if (write_buffer[melody_number][i] != '8')
					note(((int)write_buffer[melody_number][i] - 48));
				//включение звука и задержка

				//длительность
				duration_delay(duration[((int)write_buffer[melody_number][i+1] - 48)]);
				//TCCR1A = 0X00; // выключение звука

				// без функций
				/*OCR1A = height[j];//4748;
				TCCR1A = 0X40;
				
				ms = duration[3];//duration[((int)write_buffer[melody_number][i+1] - 48)];
				while (ms > 0){
				//for (int i = 1; i < ms + 1; i++){
					_delay_ms(125);
					ms -= 125;
				}
				TCCR1A = 0X00; // выключение звука

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

//	PORTC = 0xff; //клавиатура 1-й октавы и выбора мелодии
//	DDRC = 0x00; //на ввод
	PORTA = 0xff; //кнопки выбора мелодии и записи мелодии
	DDRA = 0x00;
	PORTD = 0x0C; // PD2 - переключение режима записи/воспроизведения, 
				 // PD3 - способ записи
	DDRD = 0x60;// на вывод PD5/OC1A - динамик
//	PORTE = 0x00;
//	DDRE = 0x04; //на вывод PE2/OC1B - динамик
	PORTB = 0xf8; //PB0..PB2 - динамики для трезвучия
	DDRB = 0x07;
	TCCR1A = 0x00;
	TCCR1B = 0x0B;//0x09;
	for (int i = 0; i < 6; i++){
		write_buffer[i][0] = '#';
	}

	// Для определения частот для высоты каждой ноты
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



	uart_init();				//	инициализация UART
	_delay_ms(1000);			//	задержка 1c
	int melody_number = 0;
	int note_number = -1;

	send_Uart_str("Notes: Do - a, Re - s, Mi - d, Fa - f,");
	send_Uart(13);
	send_Uart_str("Sol - g, La - h, Si - j, Do - k, Pause - Space, the end - Enter");
	send_Uart(13);
	send_Uart_str("Durations: 1 - 1, 2 - 1/2, 3 - 1/4, 4 - 1/8");
	send_Uart(13);			//	перенос строки
	info_output(melody_number);

	UCR &= ~(1<<RXEN);


	char status, data;// = UDR;
	status = USR; //USR

	int write_permit = 0;
	int input_mode = 0; // способ записи мелодии
	//unsigned int write_index = 0;
	//char write_buffer[41]; // 41 - для enter
	


	//unsigned int write_permit = 0;


//	unsigned int k = 0; // -----для проверки

	unsigned char pa_arr[6] = {PA0, PA1, PA2, PA3, PA4, PA5};
	//int button_permit = 0;

	while(1)					//	бесконечный рабочий цикл
    {
		
		temp = PINA;
		if ((PIND&(1<<PD3))==0){
			input_mode = 0; // если нажата, то ввод с определением длительностей
		} else {
			input_mode = 1;			
		}

		if ((PIND&(1<<PD2))==0) { //если нажата кнопка на запись
			uart_init();
			piano(temp);
			
			if(USR&(1<<RXC))		//	если пришёл байт по UART
			{
				data = uart_receive(); // <- status = UCSRA
				
				status = USR;//??
				
				write_melody(&melody_number, data, &note_number, status, &write_permit, input_mode);
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
						note_number = -1;
						//send_Uart_str(13);
						send_Uart_str("Wrong format!");
						send_Uart_str(13);
						info_output(melody_number);
						write_buffer[melody_number][0] = '#';
						//write_melody(&melody_number, data, &note_number, status, &write_permit);
					}
					else {
						if (melody_number < 6)
							info_output(melody_number);
						else send_Uart_str("Done!");
					}
					//info_output(melody_number);
				}
			}
			 // запрет на прием
		} 
		//if (PINB&(1<<PB3)) {
		if (PIND&(1<<PD2)) { // если кнопка отжата
			UCR &= ~(1<<RXEN);
			for (i = 0; i < 6; i++){
				if ((PINA&(1<<pa_arr[i])) == 0)
					play_melody(i);
			}
		}
		
		// попытка проиграть трезвучие
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
			//nothing succeeded
				/*PORTB |= (1<<PB0) | (1<<PB1);//PORTB = 0x01;
				_delay_us(3033);
				PORTB &= ~(1<<PB1);//PORTB = 0x01;
				_delay_us(798);
				PORTB &= ~(1<<PB0);//PORTB = 0x00;
				_delay_us(500);*/

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
				
				//4748, 4480, 4228, 3992, 3768,
				for (int i = 0; i < 5; i++){
					PORTB |= (1<<PB0);
					_delay_us(1911);//3831);
					PORTB &= ~(1<<PB0);
					_delay_us(1911);
				}
				
				for (int i = 0; i < 5; i++){
					PORTB |= (1<<PB1);
					_delay_us(1516);//3033);
					PORTB &= ~(1<<PB1);
					_delay_us(1516);
					//_delay_ms(500);
				}

				for (int i = 0; i < 5; i++){
					PORTB |= (1<<PB2);
					_delay_us(1275);//2551);
					PORTB &= ~(1<<PB2);
					_delay_us(1275);
					//_delay_ms(500);
				}
			//}
		}
				
    }





/*	while (1){
		temp = PINC;
		help = temp >> 4;;
		for (i = 0; i < 4; i++){
			if ((temp&1) == 0){
				OCR1A = height[i];//4748; задаем счетчику значение
				TCCR1A = 0X40; //OC1A переключается в рпотивоположное состояние по совпадению
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
		


		// Здесь при нажатии одной кнопки проигрываются 2 динамика
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
			//help >>= 1;  // С этой строчкой вместе не проигрываются почему-то
		}
		//конец

		TCCR1A = 0X00;
	}*/
}
