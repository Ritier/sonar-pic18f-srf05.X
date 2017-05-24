/*
 * File:   main.c
 * Author: Ritier
 *
 * Created on 15 mai 2017, 21:07
 */


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





void initialisationHardware(){
    
   OSCCONbits.IRCF = 0b111;
    
    
    ANSELA=0;
    ANSELB=0;
    
    TRISAbits.RA4=1;   //Configure RA4 comme entrée    
    
    //configure le port RB2 comme sortie digitale
    TRISBbits.RB2=0;
    
    // Activer les interruptions du temporisateur 2:
    PIE1bits.TMR2IE = 1;      // Active les interruptions du TMR2.
    IPR1bits.TMR2IP = 1;     // Interruptions de haute priorité
    
    //Config Timer2
	//T2CONbits.TMR2ON = 1; // Active le tmr2
   // T2CONbits.T2OUTPS = 0;   // pas de division en sortie pour trm2
   // T2CONbits.T2CKPS = 0;   // dision du prescaler par 4, 0-125 en 2 ms
	//PR2 = 0; // interruption lorsque le timer dépasse 2 
    
    //Timer2 Registers Prescaler= 1 - TMR2 PostScaler = 1 - PR2 = 3 - Freq = 83333.33 Hz - Period = 0.000012 seconds
    //T2CON |= 0;        // bits 6-3 Post scaler 1:1 thru 1:16
    T2CONbits.TMR2ON = 1;  // bit 2 turn timer2 on;
    T2CONbits.T2CKPS1 = 0; // bits 1-0  Prescaler Rate Select bits
    T2CONbits.T2CKPS0 = 0;
    PR2 = 1;         // PR2 (Timer2 Match value)
    
    /*
    //Config Timer1
	T1CONbits.TMR1ON = 1;    // Active le tmr1
	T1CONbits.T1CKPS = 0b10; // Diviseur de fréquence (comme tmr2): 1/4
    T1CONbits.TMR1CS = 0;    // La source est l'horloge interne Fosc/4
	T1CONbits.T1RD16 = 1;    // Active l'accès simultané à TMRxH et TMRxL (16 bits)
     * 
     * 
    // Configure le timer0 pour obtenir 150 interruptions par seconde,
    // en assumant que le microprocesseur est cadencé à 1MHz
    T0CONbits.TMR0ON = 1;  // Active le timer 0.
    T0CONbits.T08BIT = 0;  // 16 bits pour compter jusqu'à 3125.
*/
    
    // Activer le PWM sur CCP5
    CCP5CONbits.CCP5M = CAPTURE_FLANC_MONTANT;
    CCPTMRS1bits.C5TSEL = 0; // CCP5 branché sur tmr1
    PIE4bits.CCP5IE = 1;     // Active les interruptions...
    IPR4bits.CCP5IP = 0;     // ... de basse priorité.
    
   
    
    //T0CONbits.T0CS = 1;    // On utilise Fosc/4 comme source.
    //T0CONbits.PSA = 1;     // Pas de diviseur de fréquence.
    
    // Configure les interruptions
    RCONbits.IPEN = 1;      // Active le mode Haute / Basse priorité.
    INTCONbits.GIEH = 1;    // Active les interr. haute priorité.
    INTCONbits.GIEL = 1;    // Active les interr. basse priorité.
    
    INTCONbits.TMR0IE = 1;  // Active les interr. timer0
    PIE1bits.ADIE = 1;      // Active les interr. A/D
    IPR1bits.ADIP = 1;      // Interr. A/D sont de haute priorité.

}

void interrupt interruptions()
{
    if (INTCONbits.TMR0IF) {
        
        // Baisse le drapeau d'interruption pour la prochaine fois.
        INTCONbits.TMR0IF = 0;
    }
    
    if (PIR1bits.TMR2IF) {
        
        PIR1bits.TMR2IF = 0;
    }
}


void main(void) {
    
    initialisationHardware();
    while (1);
}
