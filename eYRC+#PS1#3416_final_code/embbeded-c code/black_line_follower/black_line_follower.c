/*
*
* Team Id: 				eYRCPlus-PS1#3416
* Author List: 			HIMANSHU SHARMA, PIYUSH SINGH, ABHINAV SINGH, ANKIT KUMAR SINGH(TEAM LEADER)	
* Filename: 		    Grid_Solver.c
* Theme: 				PUZZLE SOLVER - 1
* Functions: 			void lcd_port_config(void);void adc_pin_config(void),void motion_pin_config(void);void port_init();void buzzer_pin_config(void);
						void buzzer_on (void);void buzzer_off (void);void led_pin_config();void Right_led_on();void Right_led_off();
						void Left_led_on();void Right_led_off();void uart2_init(void);void timer5_init();void adc_init();
						unsigned char ADC_Conversion(unsigned char);void print_sensor(char,char,unsigned char);
						void velocity(unsigned char, unsigned char);void left_encoder_pin_config(void);
						void right_encoder_pin_config(void);void left_position_encoder_interrupt_init(void);
						void right_position_encoder_interrupt_init(void);void boot_key_interrupt_init(void);
						void motion_set (unsigned char);void forward();void stop();void back();void left();void right();
						void soft_left();void soft_right();void soft_left_2();void soft_right_2();void angle_rotate(unsigned int);
						void linear_distance_mm(unsigned int);void forward_mm(unsigned int);void back_mm(unsigned int);
						void left_degrees(unsigned int);void right_degrees(unsigned int);void soft_left_degrees(unsigned int);
						void soft_right_degrees(unsigned int);void soft_left_2_degrees(unsigned int);
						void soft_right_2_degrees(unsigned int);void init_devices(void);
						void black_line_follower();void position_correcter(int);void move_y_first(int,int,int,int);
						void move_x_first(int,int,int,int);uint8_t *Select_font(int);int Glcd_display(int);void Led_on(int);
						void Led_on1(int);void about_turn();void Start_solving();
* Global Variables:		unsigned char data, unsigned char ADC_Conversion(unsigned char), unsigned char ADC_Value, unsigned char flag,
						unsigned char Left_white_line, unsigned char Center_white_line, unsigned char Right_white_line, unsigned long int ShaftCountLeft ,
					    unsigned long int ShaftCountRight, unsigned int Degrees, unsigned char bot_dir=N,int Grid1[12],int Grid2[8],int Sol[4][11],int num[4]={0,0,0,0},
						int grid1array[12][2],int grid2array[24][2],int LED_right=0;int LED_left=0,int boot_key_press.
						int c,int p,int q,int flag2,int m,int s,int n;
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "glcd/glcd_testing.c" // included to control "GLCD".
#include <math.h> //included to support power function
#include "lcd.c"
//macros for direction  
#define N 1
#define W 2
#define S 3
#define E 4
/////////////////////

//prototype of functions used in this code
void lcd_port_config(void);
void adc_pin_config(void);
void motion_pin_config(void);
void port_init();
void buzzer_pin_config(void);
void buzzer_on (void);
void buzzer_off (void);
void led_pin_config();
void Right_led_on();
void Right_led_off();
void Left_led_on();
void Right_led_off();
void uart2_init(void);
void timer5_init();
void adc_init();
unsigned char ADC_Conversion(unsigned char);
void print_sensor(char,char,unsigned char);
void velocity(unsigned char, unsigned char);
void left_encoder_pin_config(void);
void right_encoder_pin_config(void);
void left_position_encoder_interrupt_init(void);
void right_position_encoder_interrupt_init(void);
void boot_key_interrupt_init(void);
void motion_set (unsigned char);
void forward();
void stop();
void back();
void left();
void right();
void soft_left();
void soft_right();
void soft_left_2();
void soft_right_2();
void angle_rotate(unsigned int);
void linear_distance_mm(unsigned int);
void forward_mm(unsigned int);
void back_mm(unsigned int);
void left_degrees(unsigned int);
void right_degrees(unsigned int);
void soft_left_degrees(unsigned int);
void soft_right_degrees(unsigned int);
void soft_left_2_degrees(unsigned int);
void soft_right_2_degrees(unsigned int);
void init_devices(void);
//void motors_delay();
void black_line_follower();
void position_correcter(int);
void move_y_first(int,int,int,int);
void move_x_first(int,int,int,int);
uint8_t *Select_font(int);
int Glcd_display(int);
void Led_on(int);
void Led_on1(int);
void about_turn();
void Start_solving();

//global varibles used in this code
unsigned char data;
unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char flag = 0;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;
unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning
unsigned char bot_dir=N; // store robot's direction
int boot_key_press=0;   //robot wont move until this is equal to one(becomes equal to 1 when boot key is pressed)
int Grid1[12];		// for storing Grid1 list recieved serially
int Grid2[8];     // for storing Grid 2 list recieved serially
int Sol[4][11];    // for storing the solution set(generated by python code) recieved serially via serial cable 
int num[4]={0,0,0,0};   // for storing the no. of digits required to make each number in Grid 2 
int grid1array[12][2]={{0,2},{1,2},{3,2},{4,2},{0,1},{1,1},{3,1},{4,1},{0,1},{1,1},{3,1},{4,1}}; //co-ordinates for corresponding cell nos. in Grid 1(only for first picked up number)
int grid2array[24][2]={{6,2},{6,2},{8,2},{8,2},{10,2},{10,2},{6,1},{7,1},{8,1},{9,1},{10,1},{11,1},{6,1},{7,1},{8,1},{9,1},{10,1},{11,1},
					{6,0},{6,0},{8,0},{8,0},{10,0},{10,0}};	   //Co-ordinates for corresponding cell nos. in Grid2 
int grid1array2[12][2]={{1,2},{1,2},{3,2},{3,2},{0,1},{1,1},{2,1},{3,1},{0,1},{1,1},{2,1},{3,1}};  //co-ordinates for corresponding cell nos. in Grid 1(For numbers except the first picked up number from Grid 1)
int LED_right=0;   //keeps track of right led(0-->OFF,1-->ON)
int LED_left=0;    //keeps track of left led(0-->OFF,1-->ON)
int c=0,p=0,q=0,flag2=0,m=0,s=0,n=0; // these all are used when recieve interrupt occurs(UART2 communication)

/*
*
* Function Name: 	lcd_port_config
* Input: 			none
* Output: 		    none
* Logic: 			Function to configure LCD port
* Example Call:		lcd_port_config();
*
*/
void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}


/*
*
* Function Name: 	adc_pin_config
* Input: 			none
* Output: 		    none
* Logic: 			ADC pin configuration
* Example Call:		adc_pin_config();
*
*/
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}

/*
*
* Function Name: 	motion_pin_config
* Input: 			none
* Output: 		    none
* Logic: 			Function to configure ports to enable robot's motion
* Example Call:		motion_pin_config();
*
*/
void motion_pin_config (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

/*
*
* Function Name: 	port_init
* Input: 			none
* Output: 		    none
* Logic: 			Function to Initialize PORTS
* Example Call:		port_init();
*
*/
void port_init()
{
	lcd_port_config();
	adc_pin_config();
	motion_pin_config();
	led_pin_config();
	buzzer_pin_config();
		
	left_encoder_pin_config(); //left encoder pin config
    right_encoder_pin_config(); // right encoder pin config
}

/*
*
* Function Name: 	buzzer_pin_config
* Input: 			none
* Output: 		    none
* Logic: 			Function used for configure buzzer ports
* Example Call:		buzzer_pin_config();
*
*/
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

/*
*
* Function Name: 	buzzer_on
* Input: 			none
* Output: 		    none
* Logic: 			Function used to turn on buzzer
* Example Call:		buzzer_on();
*
*/
void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

/*
*
* Function Name: 	buzzer_off
* Input: 			none
* Output: 		    none
* Logic: 			Function used to turn off buzzer
* Example Call:		buzzer_off();
*
*/
void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}

/*
*
* Function Name: 	led_pin_config
* Input: 			none
* Output: 		    none
* Logic: 			Function used to configure LED ports
* Example Call:		led_pin_config();
*
*/
void led_pin_config()
{
	DDRG = 0x03;// for right led
	DDRH=0x80;// for left led
}

/*
*
* Function Name: 	Right_led_on
* Input: 			none
* Output: 		    none
* Logic: 			Function used to turn on right led
* Example Call:		Right_led_on();
*
*/
void Right_led_on()
{
	PORTG = 0x02;
}

/*
*
* Function Name: 	Right_led_off
* Input: 			none
* Output: 		    none
* Logic: 			Function used to turn off right led
* Example Call:		Right_led_off();
*
*/
void Right_led_off()
{
	PORTG = 0x00;
}

/*
*
* Function Name: 	Left_led_on
* Input: 			none
* Output: 		    none
* Logic: 			Function used to turn on left led
* Example Call:		Left_led_on();
*
*/
void Left_led_on()
{
	PORTH = 0x80;
}
/*
*
* Function Name: 	Left_led_off
* Input: 			none
* Output: 		    none
* Logic: 			Function used to turn off left led
* Example Call:		Left_led_off();
*
*/
void Left_led_off()
{
	PORTH = 0x00;
}

//Function To Initialize UART2
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
void uart2_init(void)
{
 UCSR2B = 0x00; //disable while setting baud rate
 UCSR2A = 0x00;
 UCSR2C = 0x06;
 UBRR2L = 0x5F; //set baud rate lo
 UBRR2H = 0x00; //set baud rate hi
 UCSR2B = 0x98;
}


// the following interrupt is used for recieving serially transmitted data from the python script using pyserial package in python.
SIGNAL(SIG_USART2_RECV){ 		// ISR for receive complete interrupt
	data = UDR2; 				//making copy of data from UDR2 in 'data' variable 
	UDR2 = data; 				//echo data back to PC

	if(data == 0x2E){   // ASCII for '.'
		flag2++;
		if(flag2==2){
			c=0;
			p=0;q=0;m=0,n=0;
		}
	}
	if(flag2==0){          // flag2 will remain 0 until all the numbers from D1 list(task 1) are recieved
		if(data >=0x30 && data <= 0x39 ){  //ASCII for digits	
			Grid1[n]= (int)data - 48;  //saves the D1 list generated in task 1 into Grid1[] array(48 is substracted because numbers are recieved in ASCII form)
			n++;
		}
    }
	if (flag2==1 ){		//flag2 will remain 1 until D2 list(task 1) is recieved 
		if(data == 0x2C){	// ASCII for ','
			Grid2[m]=s;		// saves D2 list from task 1 into Grid2[] array
			c=0;
			p=0;q=0;
			m++;
		}
		else if (data >=0x30 && data <= 0x39){	   //ASCII for digits
			c++;
			q=(int)data - 48;		// 48 is substracted because the numbers are recieved in ASCII form
			if(c==1){			//code below is to recieve double digit numbers as well as single digit numbers from D2 list(task 1)
				s=p*10+q;
			}
			else{
				p=s;
				s=p*10+q;
			}
		}
	}					

	if(flag2==2){	     //flag2 will remain 2 until result of task 2 is recieved
		 if(data==0x2C){     //ASCII for ','
		 	 num[m]=n;		// num[] array keeps account of how many numbers are required by each target_numbers to complete it
		 	 m++;
			 n=0;	
		 }
		 else if(data>=0x30 && data<=0x39){     //ASCII for digits
		 	 Sol[m][n]=(int)data - 48;  // task 2 result is stored in Sol[][] and 48 is substracted because numbers recieved are in ASCII form
			 n++;
		 }
	}	 	 	
}

// Timer 5 initialized in PWM mode for velocity control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

/*
*
* Function Name: 	adc_init
* Input: 			none
* Output: 		    none
* Logic: 			Function used to initialise ADC
* Example Call:		adc_init();
*
*/
void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}


/*
*
* Function Name: 	ADC_Conversion
* Input: 			ch(channel)
* Output: 		    none
* Logic: 			Function For ADC Conversion
* Example Call:		ADC_Conversion(3);
*
*/
unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}


/*
*
* Function Name: 	print_sensor
* Input: 			row,column,channel
* Output: 		    none
* Logic: 			Function To Print Sesor Values At Desired Row And Coloumn Location on LCD
* Example Call:		print_sensor(1,1,3);
*
*/
void print_sensor(char row, char coloumn,unsigned char channel)
{
	
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}


/*
*
* Function Name: 	velocity
* Input: 			left_motor and right_motor (velocity of both in mm/sec)
* Output: 		    none
* Logic: 			Function for velocity control
* Example Call:		velocity(115,125);
*
*/
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

/*
*
* Function Name: 	left_encoder_pin_config
* Input: 			none
* Output: 		    none
* Logic: 			Function used to configure pins for left encoder
* Example Call:		left_encoder_pin_config();
*
*/
void left_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

/*
*
* Function Name: 	right_encoder_pin_config
* Input: 			none
* Output: 		    none
* Logic: 			Function used to configure pins for left encoder
* Example Call:		right_encoder_pin_config();
*
*/
void right_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

/*
*
* Function Name: 	left_position_encoder_interrupt_init
* Input: 			none
* Output: 		    none
* Logic: 			Function to configure INT4 (PORTE 4) pin as input for the right position encoder
* Example Call:		left_position_encoder_interrupt_init();
*
*/
void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt 
}


/*
*
* Function Name: 	right_position_encoder_interrupt_init
* Input: 			none
* Output: 		    none
* Logic: 			Function to configure INT5 (PORTE 5) pin as input for the right position encoder
* Example Call:		right_position_encoder_interrupt_init();
*
*/
void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
 EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
 sei();   // Enables the global interrupt 
}

/*
*
* Function Name: 	boot_key_interrupt_init
* Input: 			none
* Output: 		    none
* Logic: 			Function to configure INT7 (PORTE 7) pin as input for the boot key
* Example Call:		boot_key_interrupt_init();
*
*/
void boot_key_interrupt_init(void)
{
	cli();
	EICRB= EICRB | 0x80;
	EIMSK= EIMSK | 0x80;
	sei();
}

//ISR for boot key press
ISR(INT7_vect)
{
	boot_key_press=1;
}



//ISR for right position encoder
ISR(INT5_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect)
{
 ShaftCountLeft++;  //increment left shaft position count
}

/*
*
* Function Name: 	motion_set
* Input: 			Direction
* Output: 		    none
* Logic: 			Function used to set motion of the robot
* Example Call:		motion_Set();
*
*/
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 		// removing upper nibbel for the protection
 PortARestore = PORTA; 		// reading the PORTA original status
 PortARestore &= 0xF0; 		// making lower direction nibbel to 0
 PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
 PORTA = PortARestore; 		// executing the command
}


void forward (void) //used to move robot in forward direction
{
  motion_set (0x06);
}


void stop (void) //used to stop the robot
{
  motion_set (0x00);
}


void back (void) //both wheels backward
{
  motion_set(0x09);
}

void left (void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

void right (void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

void soft_left (void) //Left wheel stationary, Right wheel forward
{
 motion_set(0x04);
}

void soft_right (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
 motion_set(0x01);
}

void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
 motion_set(0x08);
}

/*
*
* Function Name: 	angle_rotate
* Input: 			Degrees
* Output: 		    none
* Logic: 			Function used to rotate the robot by specified degrees.
* Example Call:		angle_rotate(90);
*
*/
void angle_rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 ShaftCountRight = 0; 
 ShaftCountLeft = 0; 

 while (1)
 {
  if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
  break;
 }
 stop(); //Stop robot
}


/*
*
* Function Name: 	linear_distance_mm
* Input: 			DistanceInMM
* Output: 		    none
* Logic: 			Function used for moving robot forward or backward by specified distance
* Example Call:		linear_distance_mm(30);
*
*/
void linear_distance_mm(unsigned int DistanceInMM)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
  
 ShaftCountRight = 0;
 while(1)
 {
  if(ShaftCountRight > ReqdShaftCountInt)
  {
  	break;
  }
 } 
 stop(); //Stop robot
}

/*
*
* Function Name: 	forward_mm
* Input: 			DistanceInMM
* Output: 		    none
* Logic: 			Called if user wants to move the robot forward by specific distance
* Example Call:		forward_mm(30);
*
*/
void forward_mm(unsigned int DistanceInMM)
{
 forward();
 linear_distance_mm(DistanceInMM);
}
/*
*
* Function Name: 	back_mm
* Input: 			DistanceInMM
* Output: 		    none
* Logic: 			Called if user wants to move robot backward by specific distance
* Example Call:		back_mm(30);
*
*/
void back_mm(unsigned int DistanceInMM)
{
 back();
 linear_distance_mm(DistanceInMM);
}

void left_degrees(unsigned int Degrees)  //used to turn the robot left by specified degrees
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 left(); //Turn left
 angle_rotate(Degrees);
}


void right_degrees(unsigned int Degrees)	//used to turn the robot right by specified degrees
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 right(); //Turn right
 angle_rotate(Degrees);
}


void soft_left_degrees(unsigned int Degrees)	//used to turn the robot soft_left by specified degrees
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left(); //Turn soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}


void soft_right_degrees(unsigned int Degrees)	//used to turn the robot soft_right by specified degrees
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right();  //Turn soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}


void soft_left_2_degrees(unsigned int Degrees)   //used to perform soft_left_2 motion by specified degrees
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left_2(); //Turn reverse soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}


void soft_right_2_degrees(unsigned int Degrees)    //used to perform soft_left_2 motion by specified degrees
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right_2();  //Turn reverse soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

/*
*
* Function Name: 	init_devices
* Input: 			none
* Output: 		    none
* Logic: 			Called in main() to initialise all the devices connected to the robot
* Example Call:		init_devices();
*
*/
void init_devices (void)
{
 	cli(); //Clears the global interrupts
	port_init();
	uart2_init();
	adc_init();
	timer5_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
	boot_key_interrupt_init();
	GLCD_Init();
	lcd_set_4bit();
	lcd_init();
	sei();   //Enables the global interrupts
}

/*
*
* Function Name: 	black_line_follower
* Input: 			none
* Output: 		    none
* Logic: 			detects the black line using the three white line sensors 
* Example Call:		black_line_follower();
*
*/
void black_line_follower()
{
	Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
    Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
    Right_white_line = ADC_Conversion(1);

	print_sensor(1,1,3);	//Prints value of White Line Sensor1
	print_sensor(1,5,2);	//Prints Value of White Line Sensor2
	print_sensor(1,9,1);
	
	if(Center_white_line >=15)		// if centre sensor is on the black line then move forward
	{
		forward();
		velocity(115,125);
	}
	if(Left_white_line >=15 && Center_white_line<15)  //if left sensor is on the black line then move forward with velocity(70,100)
	{
		//forward();
		//velocity(85,125);
		velocity(80,90);
		left();
	}
	if(Right_white_line >=15 && Center_white_line<15)   //if right sensor is on the black line then move forward with velocity(100,70)
	{
		//forward();
		//velocity(125,85);
		velocity(80,90);
		right();
	}
	
	if (Center_white_line<15&& Right_white_line<15 && Left_white_line<15)
	{
		forward();
		velocity(115,125);
	}
}

	
	
/*
*
* Function Name: 	position_position_correcter
* Input: 			an integer correct
* Output: 		    none
* Logic: 			Used to correct the motion of robot if it gets disturbed due to some reasons
* Example Call:		position_position_correcter(1);
*
*/
void position_correcter(int correct)
{
	int node=1;
	while(correct==1)
	{
		black_line_follower();
		if(Center_white_line >30 && (Left_white_line >30 && Right_white_line >30))
		{
			while(1)
			{
				black_line_follower();
				if(Left_white_line <30 || Right_white_line < 30)
				{
					node=0;
					break;
				}
			}
		}
		if(node==0)
		{
			velocity(115,125);
			forward_mm(45);
			return;
		}
	}
	if(correct==0)
	{
		left_degrees(180);
		back_mm(50);
		while(1)
		{
			black_line_follower();
			if(Center_white_line >30 && (Left_white_line >30 || Right_white_line >30))
			{
				while(1)
				{
					black_line_follower();
					if(Left_white_line < 30 || Right_white_line <30)
					{
						node--;
						break;
					}
				}
			}
			if(node==0)
			{
				velocity(115,125);
				forward_mm(45);
				return;
			}
		}
	}
}


/*
*
* Function Name: 	move_y_first
* Input: 			four integer co-ordinates x1,y1,x2,y2
* Output: 		    none
* Logic: 			Grid is considered as a 2-D co-ordinate system. Each node is considered as a point in it. Each cell no. is assigned a co-ordinate.
					Co-ordinate for start position is (2,-1) and other co-ordinates can be thought of logically by referencing to the co-ordinate of start line. 
					Lets say robot has to go from start line to the cell no. '1', then this function will be called as "move_y_first(2,-1,1,2);" because (2,-1)
					is the starting point and (1,2) is the co-ordinate associated with that cell no.
					Co-ordinates for each cell no. are stored statically in array named "grid1array[12][2]" and is a global array.
					When this function is called, Y co-ordinate is traversed first hence the name of the function.

* Example Call:		move_y_first(2,-1,3,4);
*
*/
void move_y_first(int x1,int y1,int x2,int y2)
{
    
	int move_in_x_dir= x2-x1;  //stores the number of nodes robot has to traverse in x-direction
	int move_in_y_dir= y2-y1;  //stores the number of nodes robot has to traverse in y-direction
	
	if(move_in_y_dir!=0)		
	{
		_delay_ms(40);
		velocity(115,125);   
		if(move_in_y_dir > 0)	 // if the value is +ve then move in North direction	
		{
			if(bot_dir==S)  //checks the bot direction
			{
				left_degrees(180);
				bot_dir=N;   //assigns bot direction to north
			}
			if(bot_dir==E)
			{
				position_correcter(1);   //refer to the function definition to know its use. 
				left_degrees(102);
				bot_dir=N;
			}
			if(bot_dir==W)
			{
				right_degrees(102);
				bot_dir=N;
			}
			
			if(x1==2)                  
				move_in_y_dir++;	
		
			while(1)  // following while loop counts the node as the robot moves and breaks when required nodes in North direction are traversed.
			{
				black_line_follower();
				if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)   // Node encountered while traversing Arena
				{
					while(1)
					{
						black_line_follower();
						if(Left_white_line <30 || Right_white_line < 30)   // when robot crosses node then no. of node is decremented by 1
						{
							move_in_y_dir--;
							
							break;
						}
						
					}
				}
				if(move_in_y_dir==0)  // when all nodes traversed in north direction then stop
				{
					stop();
					/*
					if(x2==2)
					{
						velocity(115,125);
						back_mm(80);
					}
					*/
					break;
				}
			}
		}
		/* almost same this is happpening in remaining function(only direction is changing) 
		so they are self explanatory once above comments are understood.
		*/
		else
		{
			if(bot_dir==N)
			{
				left_degrees(180);
				bot_dir=S;
			}
			else if(bot_dir==E)
			{
				
				position_correcter(1);	
				right_degrees(102);
				bot_dir=S;
			}
			else if(bot_dir==W)
			{
					
				position_correcter(0);
				move_in_x_dir--;
				right_degrees(102);
				bot_dir=S;
			}
			while(1)
			{
				black_line_follower();
				if(Center_white_line >40 && Left_white_line>40 && Right_white_line >40) 
				{
					while(1)
					{
						black_line_follower();
						if(Left_white_line <30 || Right_white_line < 30)
						{
							move_in_y_dir++;
							
							break;
						}
					}
				}
				if(move_in_y_dir==0)
				{
					stop();
					
					break;
				}
			}
		}
	}
	if(move_in_x_dir!=0)
	{
		_delay_ms(40);
		velocity(115,125);
		if((y2-y1) != 0)   
			forward_mm(45);   // if bot traversed in Y direction then executed
		
		if(move_in_x_dir < 0)
		{	
			if(bot_dir==N)
			{
				left_degrees(102);
			}
			else if(bot_dir==S)
			{
				right_degrees(102);
			}
			else if(bot_dir==E)
			{
				left_degrees(180);
			}
			
			bot_dir=W;  // W for west
			
			while(1)  
			{
				black_line_follower();
				if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)  
				{
					while(1)
					{
						
						black_line_follower();
						if(Left_white_line <30 || Right_white_line < 30)
						{
							move_in_x_dir++;
							
							break;
						}
					}
				}
				if(move_in_x_dir==0)
				{
					velocity(115,125);
					back_mm(75);
					stop();
					_delay_ms(1);
					
					break;
					
				}
			}
		}
		if(move_in_x_dir>0)
		{
			if(bot_dir==N)
			{
				right_degrees(102);
			}
			else if(bot_dir==S)
			{
				left_degrees(102);
			}
			else if(bot_dir==W)
			{
				left_degrees(180);
			}
				
			bot_dir=E;  //E for east
			while(1)
			{
				black_line_follower();
				if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)  
				{
					while(1)
					{
						black_line_follower();
						if(Left_white_line <30 || Right_white_line < 30)
						{
							move_in_x_dir--;
							
							
							break;
						}
					}
				}
				if(move_in_x_dir==0)
				{
					velocity(115,125);
					back_mm(75);
					stop();
					_delay_ms(1);
					break;
				}
				
			}
		}
	}
	
}

/*
*
* Function Name: 	move_x_first
* Input: 			four integer coordinates (x1,y1,x2,y2)
* Output: 		    none
* Logic: 			Grid is considered as a 2-D co-ordinate system. Each node is considered as a point in it. Each cell no. is assigned a co-ordinate.
					Co-ordinate for start position is (2,-1) and other co-ordinates can be thought of logically by referencing to the co-ordinate of start line. 
					Lets say robot has to go from start line to the cell no. '1', then this function will be called as "move_y_first(2,-1,1,2);" because (2,-1)
					is the starting point and (1,2) is the co-ordinate associated with that cell no.
					Co-ordinates for each cell no. are stored statically in array named "grid1array[12][2]" and is a global array.

* Example Call:		move_y_first(2,-1,3,4);
*
*/	
void move_x_first(int x1,int y1,int x2,int y2)
{
	int move_in_x_dir=x2-x1;
	int move_in_y_dir=y2-y1;

	if(move_in_x_dir!=0)
	{
		_delay_ms(40);
		velocity(115,125);
		if(move_in_x_dir<0) // if the value is -ve then move in West direction
		{	
			if(bot_dir==N)
			{
				left_degrees(102);
			}
			if(bot_dir==S)
			{
				right_degrees(102);
			}
			if(bot_dir==E)
			{
				left_degrees(180);
			}
			else
			{
				move_in_x_dir--;
			}
			bot_dir=W;   // W for west
			
			while(1)     // following while loop counts the node as the robot moves and breaks when required nodes in West direction are traversed.
			{
				black_line_follower();
				if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)    // counts node 
				{
					while(1)
					{
						
						black_line_follower();
						if(Left_white_line <30 || Right_white_line < 30)     // detects when robot has crossed a node 
						{
							move_in_x_dir++;
							
							break;
						}
					}
				}
				if(move_in_x_dir==0)  // if required node traversed then stop
				{
					stop();
					break;
				}
			}
		}
		/* almost same this is happpening in remaining function(only direction is changing) 
		so they are self explanatory once above comments are understood.
		*/
		else
		{
			if(bot_dir==N)
			{
				right_degrees(102);
			}
			else if(bot_dir==S)
			{
				left_degrees(102);
			}
			else if(bot_dir==W)
			{
				left_degrees(180);
			}
			
			bot_dir=E;   // E for east
			while(1)
			{
				black_line_follower();
				if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)  
				{
					while(1)
					{
						black_line_follower();

						if(Left_white_line <30 || Right_white_line < 30)
						{
							move_in_x_dir--;
							
							
							break;
						}
					}
				}
				if(move_in_x_dir==-1) 
				{
					stop();
					break;
				}
				
			}
		}
	}
	if(move_in_y_dir!=0)
	{
		_delay_ms(40);
		velocity(115,125);
		forward_mm(45);  //adjusting the position of robot
		_delay_ms(20);
		if(move_in_y_dir > 0)
		{
			if(bot_dir==S)
			{
				left_degrees(180);
				bot_dir=N;
			}
			else if(bot_dir==E)
			{
				
				left_degrees(102);
				bot_dir=N;
			}
			else if(bot_dir==W)
			{
				right_degrees(102);
				bot_dir=N;
			}
		
			while(1)
			{
				black_line_follower();
				if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)  
				{
					while(1)
					{
						black_line_follower();
						if(Left_white_line <30 || Right_white_line < 30 )
						{
							move_in_y_dir--;
							
							break;
						}
					}
				}
				if(move_in_y_dir==-1)
				{
					stop();
					velocity(115,125);
					back_mm(75);
					_delay_ms(100);
					break;
				}
			}
		}
		else
		{
			
			
			if(bot_dir==N)
			{
				left_degrees(180);
			}
			else if(bot_dir==E)
			{
				right_degrees(102);
			}
			else if(bot_dir==W)
			{
				right_degrees(102);
			}

			bot_dir=S;  // S for south
			while(1)
			{
				black_line_follower(); 
				if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)  
				{
					while(1)
					{
						black_line_follower(); 
						if(Left_white_line <30 || Right_white_line < 30)
						{
							move_in_y_dir++;
							
							break;
						}
					}
				}
				if(move_in_y_dir==1)
				{
					stop();
					velocity(115,125);
					back_mm(75);
					break;
				}
			}
		}
	}
	//  if robot moved in X direction but not Y direction then executed
	else if((x2-x1)!=0)
	{
		_delay_ms(20);
		
		velocity(115,125);
		
		back_mm(75);
		
	}
}

/*
*
* Function Name: 	Select_font
* Input: 			font(number of font)
* Output: 		    returns the starting address of array of the matrix of number to be displayed in GLCD.
* Logic: 			selects the fonts from deposit.h file include with "glcd.c" library
* Example Call:		Select_font(5);
*
*/
uint8_t *Select_font(int font)
{
	if(font==0)
		return &font0; // font for number 0
	if(font==1)
		return &font1; // font for number 1
	if(font==2)
		return &font2; // font for number 2
	if(font==3)
		return &font3; // font for number 3
	if(font==4)
		return &font4; // font for number 4
	if(font==5)
		return &font5; // font for number 5
	if(font==6)
		return &font6; // font for number 6
	if(font==7)
		return &font7; // font for number 7
	if(font==8)
		return &font8; // font for number 8
	if(font==9)
		return &font9; // font for number 9
	if(font==10)
		return &Deposit; // font for message "DEPOSITED"
}

/*
*
* Function Name: 	Glcd_Display
* Input: 			font(font number to be displayed on GLCD)
* Output: 		    none
* Logic: 			Prints the number on GLCD using the data saved in "deposit.h" file in "glcd.c" library.
* Example Call:		Glcd_display(5);
*
*/
int Glcd_display(int font){
	GLCD_Init();
	/* ptr1 and ptr2 are pointers to point to arrays of font information stored in "deposit.h" while dat1 
	is used to  read data from where the pointer is pointing.
	*/
	uint8_t line,cursor,*ptr1,*ptr2,dat1;
	ptr1=Select_font(font);
	ptr2=ptr1;
    for(line=0;line<8;line++) {
         GLCD_GoToLine(line);  
        
        for(cursor=0;cursor<128;cursor++){
            if(cursor==64){
                GLCD_GoToPage(1); 
                
            }
			
			dat1=pgm_read_byte(ptr1);
			glcd_DataWrite(dat1);
			ptr1=ptr1+8; 
        }
		ptr1=++ptr2;
	}
	
}


/*
*
* Function Name: 	Led_on
* Input: 			pos->cell no. of the number that robot is going to pick from Grid 1.
* Output: 		    none
* Logic: 			simply "GLOW" the led facing the number that is going to be picked up(displayed on GLCD).Only execute once i.e. when robot picks first number.
* Example Call:		Led_on(4);
*
*/
void Led_on(int pos)
{
	if(pos==0 || pos==1 || pos==4 || pos==5 || pos==8 || pos==9) // led will glow according to which cell no. is being referred
	{
		Right_led_on();//right led
		LED_right=1;
		
	}
	else
	{
		Left_led_on();//left led
		LED_left=1;
		
	}
}

/*
*
* Function Name: 	Led_on1
* Input: 			pos->cell no. of the number that robot is going to pick from Grid 1.
* Output: 		    none
* Logic: 			simply "GLOW" the led facing the number that is going to be picked up(displayed on GLCD).
					Executes for the all numbers picked up excoet the first one because first number is reached according
					to move_y_first algorithm and rest of the numbers picked up from grid 1 are reached using move_x_first algo.
					So robot reaches a cell no. in different position for the same cell no. and hence this function is used.
* Example Call:		Led_on1(6);
*
*/
void Led_on1(int pos){
	if(pos==1 || pos==3 || pos==4 || pos==5 || pos==6 || pos==7){
		Right_led_on();//right led
		LED_right=1;
	}
	else{
		Left_led_on();//left led
		LED_left=1;
	}
}

/*
*
* Function Name: 	about_turn
* Input: 			none
* Output: 		    none
* Logic: 			Called if the robot reaches the target_number in Grid 2 but "glowing led" is not facing that number(glowing led is on opposite side).
					This function will turn the robot such as the led faces the number while deposition is done.
* Example Call:		about_turn();
*
*/
void about_turn() {
	int node=1;      	//robot searches for 1 node in the grid in forward direction,when found node becomes 0 and hence rotation of robot takes place.
	while(1) {
		black_line_follower();
		if(Center_white_line >=30 && Left_white_line >=30 && Right_white_line >=30)  {
			while(1) {
				black_line_follower(); 
				if(Left_white_line <30 || Right_white_line < 30){
					node--;	//node found so decrement node
					
					break;
				}
			}
		}
		//if node found then "about turn" the robot.
		if(node==0) {          
			velocity(115,125);//ensuring both wheels have same speed(in our case the two wheels had velocity difference of 10mm/sec so this command will equal their speed)
			forward_mm(20);  //before performing about turn move robot a little forward.(if this is not done,sharp sensor may land up on another node and that may cause problem later)
			if(bot_dir==N)    
				bot_dir=S;
			else if(bot_dir==S)
				bot_dir=N;
			else if(bot_dir==W)
				bot_dir=E;
			else
				bot_dir=W;
			_delay_ms(20);
			velocity(115,125);
			left_degrees(190);
			break;
		}
	}
}

/*
*
* Function Name: 	Start_solving
* Input: 			none
* Output: 		    none
* Logic: 		    Reads the output of the task 1 and 2 and accordingly sets the path for robot to move on.
					Output of python is serially transferred using serial cable and is saved in arrays which are read by
					this function to decide the path
					Also this function calls some important functions in between like LED ON or OFF,Buzzer ON or OFF and
					GLCD display functions.
					******************
					*imp-->(2,-1) is the cordinate of the start line. Other nodes have co-ordinates which can be referenced accordingly.
					******************
* Example Call:		Start_solving();
*
*/
void Start_solving()
{
	int nos_deposited=0;
	int nos_deposited_single=0; //keeps track of how many numbers has been deposited for a single number completion in Grid 2
	int i=0;
	int pos,ptr=0;    //these two variables are used as index holder for arrays transferred serially from python
	int ptr2=0;   // used to store index of num[] array
	int c,d;   // variable to store co-ordinates	
    int total_nos=num[0]+num[1]+num[2]+num[3];   // storing total numnbers required to sum up and making the numbers in Grid2
	
	
	while(nos_deposited!=total_nos)  // loop until all numnbers are not deposited
	{
		if(nos_deposited_single==num[ptr2])    //if one number in Grid2 is completed
		{	
			ptr2++;							//increment ptr2 by 1 to get total numbers required to solve next number in Grid2
			ptr+=2;							//increment ptr by 2 to get the next number to solve in Grid2
			nos_deposited_single=0;			//initialise nos_deposited_single to 0 as new number does not have any numbers deposited on it yet.
		}
		for(i=0;i<12;i++)					//loop to know the position of the number robot need to pick by searching it in Grid1 array.
		{
			if(Sol[ptr2][nos_deposited_single]==Grid1[i])
			{
				Grid1[i]=Grid1[i]+30;
				pos=i;
				break;			//if found,break from loop
			}	
		}
		if(nos_deposited==0)		//only evaluates to true once for first number to be deposited.
		{
			c=grid1array[pos][0];	//read x co-ordinate from co-ordinates array
			d=grid1array[pos][1];   //read y co-ordinate from co-ordinates array
			
			move_y_first(2,-1,c,d);   //traverse to the first number to be picked up
			Led_on(pos);			//turn ON LED according to the position of robot.
		}
		else
		{
			c=grid1array2[pos][0];   //read x co-ordinate from co-ordinates array
			d=grid1array2[pos][1];   //read y co-ordinate from co-ordinates array
			
			move_x_first(4,1,c,d);   //traverse from (4,1) to the number to be picked //[(2,-1) is the coordinate of the start line]
			Led_on1(pos);			//turn ON LED according to the position of robot.
		}		
		Glcd_display(Sol[ptr2][nos_deposited_single]);   //used to display required number on GLCD

		if((c==0 || c==1) && (d==1) && nos_deposited==0) //to correct the motion of robot as nodes may sometimes be needed to increased or decreased due to turning of robot
		{
			c++;
		}
		if(pos>=4 && pos<=11 && nos_deposited!=0)     //same as above
			c++;
		if((pos==6 || pos==7 || pos==10 || pos==11) && nos_deposited==0)    //similar
			c--;
		move_y_first(c,d,5,1);   //traverse
	    
		c=grid2array[Grid2[ptr]][0];   //read x co-ordinate from co-ordinates array
		d=grid2array[Grid2[ptr]][1];   //read y co-ordinate from co-ordinates array
		move_x_first(5,1,c,d);    //traverse

		//below if else code is used to deposit number from the side where LED is glowing
		if(LED_right==1)//if right led is glowing
		{
			if(((d-1)!=0) && (Grid2[ptr]==1 || Grid2[ptr]==3 || Grid2[ptr]==5 || Grid2[ptr]==18 || Grid2[ptr]==20 || Grid2[ptr]==22))
			{
				LED_right=0;
				Right_led_off(); //turn off right led
			}
			else if((d-1)!=0)
			{
				about_turn();  //if Glowing LED is on the opposite side of the target_number(number in Grid2) then moves robot 180 degrees.
				if(d>0)     //on turning the co-ordinates needs to be fixed
					d=d+1;
				else
					d=d-1;
				LED_right=0;
				Right_led_off();
			}
			else if ((d==1) && (Grid2[ptr]>=12 && Grid2[ptr]<=17))
			{
				Right_led_off();
				LED_right=0;
				c--;
			}
			else if((d==1) && (Grid2[ptr]>=6 && Grid2[ptr]<=11))
			{
				about_turn();
				LED_right=0;
				Right_led_off();
			}
			
				
		}
		if(LED_left==1)  //if left led is glowing
		{
			if(((d-1)!=0) && (Grid2[ptr]==0 || Grid2[ptr]==2 || Grid2[ptr]==4 || Grid2[ptr]==19 || Grid2[ptr]==21 || Grid2[ptr]==23))
			{
			
				LED_left=0;
				Left_led_off();  //turn off left led
			}
			else if((d-1)!=0)
			{
				about_turn();   //if Glowing LED is on the opposite side of the target_number(number in Grid2) then moves robot 180 degrees.
				if(d>0)       //on turning the co-ordinates needs to be fixed
					d=d+1;
				else
					d=d-1;
				LED_left=0;
				Left_led_off();
			}
			else if ((d==1) && (Grid2[ptr]>=6 && Grid2[ptr]<=11))
			{
				Left_led_off();
				LED_left=0;
				c--;
			}
			else if((d==1) && (Grid2[ptr]>=12 && Grid2[ptr]<=17))
			{
				about_turn();
				LED_left=0;
				Left_led_off();
			}
		}		
		Glcd_display(10);    // for displaying "DEPOSITED" on GLCD
		nos_deposited_single++;   //increment as number is deposited for single number
		if(nos_deposited_single==num[ptr2])  // if single number is completed then sound buzzer
		{
			if(nos_deposited==total_nos - 1)   //for all numbers deposited except last one
			{
				buzzer_on();
				_delay_ms(6000);
				buzzer_off();
			}
			else				// for the last number
			{
				buzzer_on();
				_delay_ms(1000);
				buzzer_off();
			}
		}
		if(nos_deposited==total_nos - 1)   //if all nos deposited then break from the loop
			break;
		move_y_first(c,d,4,1);			//return from grid 2 to grid 1 boundary using bridge
		nos_deposited++;			//increment the numbers deposited as a whole
		
		
	}
}

/*
*
* Function Name: 	main
* Input: 			none
* Output: 		    int to inform the caller that the program exited correctly or incorrectly.
* Logic: 			calls "init_devices()" to initailise all device and the while loop keep executing until INT7 interrupt(boot key press) has occured. 
* Example Call:		Called automatically by the Operating System.
*
*/
int main() {
	init_devices();   
	while(boot_key_press==0)   // loops until boot key is pressed
	{
	}
	Start_solving();  //starts solving the grid
	return 0;	
}			
