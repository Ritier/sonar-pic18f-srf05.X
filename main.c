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
    
    ANSELA=0;
    ANSELB=0;
    
    TRISAbits.RA4=1;            //Configure RA4 comme entrée    
    
    //configure le port RB2 comme sortie digitale
    TRISBbits.RB2=0;
    
    //Config Timer1
	T1CONbits.TMR1ON = 1;    // Active le tmr1
	T1CONbits.T1CKPS = 0b10; // Diviseur de fréquence (comme tmr2): 1/4
    T1CONbits.TMR1CS = 0;    // La source est l'horloge interne Fosc/4
	T1CONbits.T1RD16 = 1;    // Active l'accès simultané à TMRxH et TMRxL (16 bits)

    
    // Activer le PWM sur CCP5
    CCP5CONbits.CCP5M = CAPTURE_FLANC_MONTANT;
    CCPTMRS1bits.C5TSEL = 0; // CCP5 branché sur tmr1
    PIE4bits.CCP5IE = 1;     // Active les interruptions...
    IPR4bits.CCP5IP = 0;     // ... de basse priorité.

}



void main(void) {
    
    void initialisationHardware();
    
    while (1);
    
    return;
}
