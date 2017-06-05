/*
 * File:   main.c
 * Author: Ritier
 *
 * Created on 15 mai 2017, 21:07
 */
static int pulse = 0;
static int temp = 0;

#include <xc.h>

#define CAPTURE_FLANC_MONTANT 0b101
#define CAPTURE_FLANC_DESCENDANT 0b100

/**
 * Bits de configuration:
 */
#pragma config FOSC = INTIO67   // Osc. interne, A6 et A7 comme IO.
#pragma config IESO = OFF       // Pas d'osc. au démarrage.
#pragma config FCMEN = OFF      // Pas de monitorage de l'oscillateur.

// Nécessaires pour ICSP / ICD:
#pragma config MCLRE = EXTMCLR  // RE3 est actif comme master reset.
#pragma config WDTEN = OFF      // Watchdog inactif.
#pragma config LVP = OFF        // Single Supply Enable bits off.


typedef enum {
    TRIGGER_ON,
    TRIGGER_OFF
} Trigger;

Trigger trigger;

unsigned int capture;
unsigned int distance;

void CompleteCapture(unsigned int instant);

void initialisationHardware(){
    
   //OSCCONbits.IRCF = 0b111;
   //OSCCONbits.SCS = 11;
	
 // ANSELB=0;
	ANSELA=0;
    TRISAbits.RA4=1;   //Configure RA4 comme entrée 
	TRISAbits.RA3=0;   //Configure RA3 comme sortie 
    TRISAbits.RA5=0;   //Configure RA5 comme sortie
    
    // Configure les interruptions
    RCONbits.IPEN = 1;      // Active le mode Haute / Basse priorité.
    INTCONbits.GIEH = 1;    // Active les interr. haute priorité.
    INTCONbits.GIEL = 1;    // Active les interr. basse priorité.
    
	
	// Config pour le trigger --------------------------------------------------
	
    // Activer les interruptions du temporisateur 2:
    PIE1bits.TMR2IE = 1;      // Active les interruptions du TMR2.
    IPR1bits.TMR2IP = 1;     // Interruptions de haute priorité
  
    //Timer2 Registers Prescaler= 1 - TMR2 PostScaler = 1 - PR2 = 3 - Freq = 83333.33 Hz - Period = 0.000012 seconds
    T2CON |= 0;        // bits 6-3 Post scaler 1:1 thru 1:16
    T2CONbits.TMR2ON = 1;  // bit 2 turn timer2 on;
    T2CONbits.T2CKPS1 = 0; // bits 1-0  Prescaler Rate Select bits
    T2CONbits.T2CKPS0 = 0;
    PR2 = 1;         // PR2 (Timer2 Match value)
    
	
	// Config pour la capture de l'echo ---------------------------------

	// Activer la capture sur CCP5
    CCP5CONbits.CCP5M = CAPTURE_FLANC_MONTANT;
    CCPTMRS1bits.C5TSEL = 0; // CCP5 branché sur tmr1
    PIE4bits.CCP5IE = 1;     // Active les interruptions...
    IPR4bits.CCP5IP = 0;     // ... de basse priorité.
	
	   //Config Timer1
    
    // Activer les interruptions du temporisateur 1:
    PIE1bits.TMR1IE = 1;      // Active les interruptions du TMR1.
    IPR1bits.TMR1IP = 1;     // Interruptions de haute priorité
    
   T1CONbits.TMR1ON = 1;    // Active le tmr1
   T1CONbits.T1CKPS = 0b00; // Diviseur de fréquence (comme tmr2): 1/4
   T1CONbits.TMR1CS = 0;    // La source est l'horloge interne Fosc/4
   T1CONbits.T1RD16 = 1;    // Active l'accès simultané à TMRxH et TMRxL (16 bits)
	
	
/*	//Timer1 Registers Prescaler= 1 - TMR1 Preset = 0 - Freq = 3.81 Hz - Period = 0.262144 seconds
	T1CONbits.T1CKPS1 = 0;   // bits 5-4  Prescaler Rate Select bits
	T1CONbits.T1CKPS0 = 0;   // bit 4
	T1CONbits.T1OSCEN = 1;   // bit 3 Timer1 Oscillator Enable Control bit 1 = on
	T1CONbits.T1SYNC = 1;    // bit 2 Timer1 External Clock Input Synchronization Control bit...1 = Do not synchronize external clock input
	T1CONbits.TMR1CS = 01;    // bit 1 Timer1 Clock Source Select bit...0 = Internal clock (FOSC/4)
	T1CONbits.TMR1ON = 1;    // bit 0 enables timer
	T1CONbits.T1RD16 = 1;   //Active l'accès simultané à TMRxH et TMRxL (16 bits
	TMR1H = 0;             // preset for timer1 MSB register
	TMR1L = 0;             // preset for timer1 LSB register
*/
}

void interrupt interruptions()
{
   // if (INTCONbits.TMR0IF) {
        
        // Baisse le drapeau d'interruption pour la prochaine fois.
  //      INTCONbits.TMR0IF = 0;
   // }
    
    if (PIR1bits.TMR2IF) {
        switch (trigger){
			case TRIGGER_ON:
				PR2 = 255;   //pour 1ms
				PORTAbits.RA5 = 0;
				temp ++;
			if (temp < 50){ // 50 fois la boucle de 1ms
				temp ++;
			} else {
				trigger = TRIGGER_OFF;
				temp = 0;
			}
				break;
			case TRIGGER_OFF:
				PR2 = 3; //pour 12us
				PORTAbits.RA5 = 1;
				trigger = TRIGGER_ON;
				break;
		}
        PIR1bits.TMR2IF = 0;
    }
  
  if (PIR1bits.TMR1IF) {
		int val;
		val=0; 
  }
  
  if (PIR4bits.CCP5IF) {
        if (PORTAbits.RA4) {
            capture = CCPR1;
            CCP5CONbits.CCP5M = CAPTURE_FLANC_DESCENDANT;
        } else {
            CompleteCapture(CCPR5);            
            CCP5CONbits.CCP5M = CAPTURE_FLANC_MONTANT;
        }
        PIR4bits.CCP5IF = 0;        
    }
}

/**
 * Complète une capture PWM, et met à jour le canal indiqué.
 * @param canal Le numéro de canal.
 * @param instant L'instant de finalisation de la capture.
 */
void CompleteCapture(unsigned int instant) {
        if (instant >= capture){
           capture = (instant - capture); 
        } else {
            capture = (65536 - capture);
            capture = (instant + capture); 
        }       
        distance = capture;
}

void main(void) {
    
    initialisationHardware();
    PORTAbits.RA3 = 1;
    while (1);
}