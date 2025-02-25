#include "knobs.hpp"

namespace knobs {

// Global variables definition
bool start_button_pressed = false;

void setup() {
    // Disable interrupts
    __disable_irq();

    // Power management ///////////////////////////////////////////////////////////////////////
    PM->APBCMASK.reg |= PM_APBAMASK_EIC;      // Activate the External Interrupt Controller peripheral

    // Generic clock //////////////////////////////////////////////////////////////////////////
    // Set a 100Hz clock for the external interrupts debounce -> 30ms debounce time

    // Configure the divider
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(328) |          // Divide the clock by 328 (32768Hz / 328 = 100Hz)
                       GCLK_GENDIV_ID(3);               // Select GCLK3
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization
    
    // Configure the generator
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN |         // Enable the clock generator
                        GCLK_GENCTRL_SRC_OSC32K |  // Select the 32.768kHz oscillator as source
                        GCLK_GENCTRL_ID(3);            // Select GCLK3
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronizations

    // Configure the clock
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Route the 100Hz GCLK2 to the External Interrupt Controller clock
                        GCLK_CLKCTRL_GEN_GCLK3 |     
                        GCLK_CLKCTRL_ID_EIC;   
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization


    // Port pins /////////////////////////////////////////////////////////////////////
    // Connect the START Pin to the external interrupt 0
    PORT->Group[g_APinDescription[START_PIN].ulPort].PINCFG[g_APinDescription[START_PIN].ulPin].bit.PMUXEN = 1;                   // Enable PORT multiplexer
    PORT->Group[g_APinDescription[START_PIN].ulPort].PINCFG[g_APinDescription[START_PIN].ulPin].bit.PULLEN = 1;                   // Enable pull-x resistor
    PORT->Group[g_APinDescription[START_PIN].ulPort].OUT.reg |= 1 << (g_APinDescription[START_PIN].ulPin);                        // Set pull-up resistor
    PORT->Group[g_APinDescription[START_PIN].ulPort].PMUX[g_APinDescription[START_PIN].ulPin >> 1].reg |= PORT_PMUX_PMUXO_A;      // Select channel A = EXTINT[0]

    // Set SPEED potentiometers as analog input
    PORT->Group[g_APinDescription[SPEED_PIN].ulPort].PINCFG[g_APinDescription[SPEED_PIN].ulPin].bit.PMUXEN = 0;     // Disable PORT multiplexer
    PORT->Group[g_APinDescription[SPEED_PIN].ulPort].PINCFG[g_APinDescription[SPEED_PIN].ulPin].bit.PULLEN = 0;     // Disable pull-x resistor
    PORT->Group[g_APinDescription[SPEED_PIN].ulPort].DIRCLR.reg |= 1 << g_APinDescription[SPEED_PIN].ulPin;         // Set as input
    
    // External interrupts ///////////////////////////////////////////////////////
    EIC->CTRL.bit.ENABLE = 0;                       // Disable the EIC peripheral
    while (EIC->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    // Setup the external interrupt for the MODE pushbutton
    EIC->CONFIG[g_APinDescription[START_PIN].ulPort].reg |= EIC_CONFIG_SENSE0_FALL |     // Detect falling edges
                                                           EIC_CONFIG_FILTEN0;          // Enable filter (debounger)
    while (EIC->STATUS.bit.SYNCBUSY);              // Wait for synchronization
    EIC->INTENSET.reg |= EIC_INTENSET_EXTINT0;   // Enable the interrupt
    while (EIC->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    // Enable the EIC peripheral
    EIC->CTRL.bit.ENABLE = 1;
    while (EIC->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    // Enable the interrupt in the NVIC
    NVIC_SetPriority(EIC_IRQn, 1);       // Set EIC interrupt priority to second highest
    NVIC_EnableIRQ(EIC_IRQn);            // Enable Interrupts for EIC at NVIC

    // Enable interrupts globally
    __enable_irq();
}

int get_speed_correction() {
    /**
     * @brief Get the value of the speed correction according to potentiometer.
     * 
     * @return int The value of the speed correction, in RPM. Range: MIN_SPEED_CORR to MAX_SPEED_CORR
    */
    int tmp = analogRead(SPEED_PIN)/1023.0 * (MAX_SPEED_CORR - MIN_SPEED_CORR) + MIN_SPEED_CORR;
    if(tmp < MIN_SPEED_CORR) {
        return MIN_SPEED_CORR;
    }
    if(tmp > MAX_SPEED_CORR) {
        return MAX_SPEED_CORR;
    }
    return tmp;
}

void start_handler() {
    // Mode button was pressed
    start_button_pressed = true;
}

} // namespace knobs

// Interrupt handler for the pushbuttons. Must be outside of any namespace
void EIC_Handler(void) {
    // Check which button was pressed
    if(EIC->INTFLAG.reg & EIC_INTFLAG_EXTINT0) {
        // Mode button was pressed
        knobs::start_handler();

        // Reset interrupt flag
        EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT0;
    }
}
