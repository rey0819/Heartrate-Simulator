#include <MK64F12.h>

/* BELOW USER DEFINES:
 * sex: For man: 'M' or 'm' , For woman: 'W' or 'w'
 * age: Input an age above 10 years old 
 * BPM: Resting heart rate in beats per minute (BPM)
 *
 * Healthy: green, Poor: yellow, Dangerously Slow: blue, Dangerously Fast: red
 */
char sex='m';
int age=36;
int BPM = 100;



//Create functions to turn LEDs on and off
void LEDRed_On(){
	PTB->PCOR = 1<<22;
}
void LEDRed_Off(){
	PTB->PSOR = 1<<22;
}
void LEDGreen_Off(){
	PTE->PSOR = 1<<26;
}
void LEDGreen_On(){
	PTE->PCOR = 1<<26;
}
void LEDBlue_Off(){
	PTB->PSOR = 1<<21;
}
void LEDBlue_On(){
	PTB->PCOR = 1<<21;
}

int main (void)
{
	  NVIC_EnableIRQ(PIT0_IRQn); //enable PIT0 Interrupts
	
	//setup green	LED 
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //Enable the clock to port E 
	PORTE->PCR[26] = PORT_PCR_MUX(001); //Set up PTE26 as GPIO 
	
	//setup blue and red LED
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //Enable the clock to port B 
	PORTB->PCR[21] = PORT_PCR_MUX(001); //Set up PTB21 as GPIO 
	PORTB->PCR[22] = PORT_PCR_MUX(001); //Set up PTB22 as GPIO 
	PTB->PDOR = (1 << 21 | 1 << 22 ); //Allow port B to use red and blue LED
  PTB->PDDR = (1 << 21 | 1 << 22 );
	
	PTE->PDOR = 1 << 26;            // switch green LED off  
  PTE->PDDR = 1 << 26;            // enable PTE26 as Output 
	
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module 
	PIT->CHANNEL[0].LDVAL = 0; // Set load value of zeroth PIT
	PIT->MCR = 0;
	PIT->CHANNEL[0].TCTRL = 11;
	
}


/* 
     PIT Interrupt Handler
*/
//Create variables to tell when LED's are on and off
unsigned int redLED = 0;
unsigned int blueLED = 0;
unsigned int greenLED = 0;
unsigned int LoadValue = 20970000; //From manual: this gives a 1 second long delay
void PIT0_IRQHandler(void)
{	
	if(sex=='M' || sex=='m'){ //For men's heartrates
		if((age>=10 && age<=35 && BPM>=49 && BPM<=81) //Check for healthy BPM for age ranges(use green LED)
			|| (age>=36 && age<=55 && BPM>=50 && BPM<=83)
			|| (age>=56 && BPM>=51 && BPM<=81)){ 
			if(greenLED==0){
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000; //Math will give the correct load value for the BPM
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=1;
				LEDGreen_On();
			}
			else{
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL=400000;
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=0;
				LEDGreen_Off();
			}
		}
		else if((age>=10 && age<=35 && BPM>0 && BPM<=48) //Check for BPM that is dangerously slow for age ranges (use blue LED)
			|| (age>=36 && age<=55 && BPM>0 && BPM<=49)
			|| (age>=56 && BPM>0 && BPM<=50)){ 
				if(blueLED==0){
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000;
					PIT->CHANNEL[0].TCTRL=11;
					blueLED=1;
					LEDBlue_On();
				}
				else{
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL=400000;
					PIT->CHANNEL[0].TCTRL=11;
					blueLED=0;
					LEDBlue_Off();
				}
		}
		else if((age>=10 && age<=35 && BPM>=82 && BPM<=99) //Check for BPM that is poor for age ranges (use red&green LED)
			|| (age>=36 && age<=55 && BPM>=84 && BPM<=99)
			|| (age>=56 && BPM>=82 && BPM<=99)){ 
				if(greenLED==0){
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000;
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=1;
				redLED=1;
				LEDGreen_On();
				LEDRed_On();
			}
			else{
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL=400000;
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=0;
				redLED=0;
				LEDGreen_Off();
				LEDRed_Off();
			}
		}
		else if(age>=10 && BPM>=100){ //Check for BPM that is dangerously fast for age ranges(use red LED)
				if(redLED==0){ 
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000;
					PIT->CHANNEL[0].TCTRL=11;
					redLED=1;
					LEDRed_On();
				}
				else{
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL=400000;
					PIT->CHANNEL[0].TCTRL=11;
					redLED=0;
					LEDRed_Off();
				}
		}
		else if(age<10){ //If age is less than 10 (or negative), show that there is an error by a constant red LED
			redLED=1;
			LEDRed_On();
			}
	//Otherwise, if BPM is negative or 0, no LEDs will blink since there is no heart beat
	}
	
	if(sex=='W' || sex=='w'){ //For women's heartrates
		if((age>=10 && age<=35 && BPM>=54 && BPM<=83) //Check for healthy BPM for each age range(use green LED)
			|| (age>=36 && age<=55 && BPM>=54 && BPM<=84)
			|| (age>=56 && BPM>=54 && BPM<=84)){ 
			if(greenLED==0){
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000;
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=1;
				LEDGreen_On();
			}
			else{
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL=400000;
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=0;
				LEDGreen_Off();
			}
		}
		else if((age>=10 && age<=35 && BPM>0 && BPM<=53) //Check for BPM that is dangerously slow for age ranges (use blue LED)
			|| (age>=36 && age<=55 && BPM>0 && BPM<=53)
			|| (age>=56 && BPM>0 && BPM<=53)){ 
				if(blueLED==0){
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000;
					PIT->CHANNEL[0].TCTRL=11;
					blueLED=1;
					LEDBlue_On();
				}
				else{
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL=400000;
					PIT->CHANNEL[0].TCTRL=11;
					blueLED=0;
					LEDBlue_Off();
				}
		}
		else if((age>=10 && age<=35 && BPM>=84 && BPM<=99) //Check for BPM that is poor for age ranges (use red&green LED)
			|| (age>=36 && age<=55 && BPM>=85 && BPM<=99)
			|| (age>=56 && BPM>=84 && BPM<=99)){ 
				if(greenLED==0){
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000;
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=1;
				redLED=1;
				LEDGreen_On();
				LEDRed_On();
			}
			else{
				PIT->CHANNEL[0].TFLG=1;
				PIT->CHANNEL[0].LDVAL=400000;
				PIT->CHANNEL[0].TCTRL=11;
				greenLED=0;
				redLED=0;
				LEDGreen_Off();
				LEDRed_Off();
			}
		}
		else if(age>=10 && BPM>=100){ //Check for BPM that is dangerously fast for age ranges(use red LED)
				if(redLED==0){ 
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL= (LoadValue*60/BPM)-400000;
					PIT->CHANNEL[0].TCTRL=11;
					redLED=1;
					LEDRed_On();
				}
				else{
					PIT->CHANNEL[0].TFLG=1;
					PIT->CHANNEL[0].LDVAL=400000;
					PIT->CHANNEL[0].TCTRL=11;
					redLED=0;
					LEDRed_Off();
				}
		}
		else if(age<10){ //If age is less than 10 (or negative), show that there is an error by a constant red LED
			redLED=1;
			LEDRed_On();
			}
	//Otherwise, if BPM is negative or 0, no LEDs will blink since there is no heart beat
	}
	
//If the sex is not specified correctly ('M', 'm', 'W', or 'w'), show an error with a constant red LED
else if(sex!='M' && sex!= 'm' && sex!= 'W' && sex!= 'w'){ 
		redLED=1;
		LEDRed_On();
}

}
