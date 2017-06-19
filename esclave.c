#include <xc.h>
#include "i2c.h"
#include "file.h"
#include "esclave.h"

/**
 * Point d'entrée des interruptions pour l'esclave.
 */
void esclaveInterruptions() {

    if (PIR1bits.SSP1IF) {
        i2cEsclave();
    }
}
void CompleteCapture(unsigned int instant);

/**
 * Initialise le hardware pour l'émetteur.
 */
static void esclaveInitialiseHardware() {
    
    PIE1bits.TMR1IE = 1;    // Active les interruptions...
    IPR1bits.TMR1IP = 0;    // ... de basse priorité.

    // Active le MSSP1 en mode Esclave I2C:
    TRISCbits.RC3 = 1;          // RC3 comme entrée...
    ANSELCbits.ANSC3 = 0;       // ... digitale.
    TRISCbits.RC4 = 1;          // RC4 comme entrée...
    ANSELCbits.ANSC4 = 0;       // ... digitale.

    SSP1CON1bits.SSPEN = 1;     // Active le module SSP.    
    
    SSP1ADD = ECRITURE_SERVO_0;   // Adresse de l'esclave.
    SSP1MSK = I2C_MASQUE_ADRESSES_ESCLAVES;
    SSP1CON1bits.SSPM = 0b1110; // SSP1 en mode esclave I2C avec adresse de 7 bits et interruptions STOP et START.
        
    SSP1CON3bits.PCIE = 0;      // Désactive l'interruption en cas STOP.
    SSP1CON3bits.SCIE = 0;      // Désactive l'interruption en cas de START.
    SSP1CON3bits.SBCDE = 0;     // Désactive l'interruption en cas de collision.

    PIE1bits.SSP1IE = 1;        // Interruption en cas de transmission I2C...
    IPR1bits.SSP1IP = 0;        // ... de basse priorité.

    // Active les interruptions générales:
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
}

/**
 * Point d'entrée pour l'esclave.
 */
void esclaveMain(void) {
    esclaveInitialiseHardware();
    i2cReinitialise();
    i2cRappelCommande(CompleteCapture);
    while(1);
}
