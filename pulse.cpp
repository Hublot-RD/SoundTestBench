/*
Script inspired from https://forum.arduino.cc/t/samd21-mkrzero-analog-comparator/1194763
*/

#include "pulse.hpp"

namespace pulse {

void setup() {
    // Disable interrupts
    __disable_irq();

    // Power management ///////////////////////////////////////////////////////////////////////

    PM->APBCMASK.reg |= PM_APBCMASK_EVSYS;      // Activate the event system peripheral
    PM->APBCMASK.reg |= PM_APBCMASK_AC;         // Activate the analog comparator peripheral
    PM->APBCMASK.reg |= PM_APBCMASK_TCC0;       // Activate the timer counter 0 peripheral
    PM->APBCMASK.reg |= PM_APBCMASK_TC4;        // Activate the timer counter 4 peripheral
    PM->APBCMASK.reg |= PM_APBCMASK_DAC;        // Activate the digital to analog converter peripheral


    // Generic clock ///////////////////////////////////////////////////////////////////////////

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Route the 32.768kHz GCLK1 to the Analog Comparator analog clock
                        GCLK_CLKCTRL_GEN_GCLK1 |     
                        GCLK_CLKCTRL_ID_AC_ANA;   
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Route the 48MHz GCLK0 to the Analog Comparator digital clock
                        GCLK_CLKCTRL_GEN_GCLK0 |     
                        GCLK_CLKCTRL_ID_AC_DIG;   
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Route the 48MHz GCLK0 to the Timer Counter 0 & 1 clock
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_ID_TCC0_TCC1;
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Route the 48MHz GCLK0 to the Timer Counter 4 & 5 clock
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_ID_TC4_TC5;
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Route the 48MHz GCLK0 to the digital to analog converter clock
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_ID_DAC;
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization


    // Configure port pins /////////////////////////////////////////////////////////////////////
    pinMode(COILSELA_PIN, OUTPUT);
    pinMode(COILSELB_PIN, OUTPUT);
    pinMode(COILSELC_PIN, OUTPUT);
    digitalWrite(COILSELA_PIN, LOW);
    digitalWrite(COILSELB_PIN, LOW);
    digitalWrite(COILSELC_PIN, LOW);

    // Connect the AC channel 1 input to PIN A4
    PORT->Group[g_APinDescription[SIGNAL_PIN].ulPort].PINCFG[g_APinDescription[SIGNAL_PIN].ulPin].bit.PMUXEN = 1;                   // Enable PORT multiplexer
    PORT->Group[g_APinDescription[SIGNAL_PIN].ulPort].PMUX[g_APinDescription[SIGNAL_PIN].ulPin >> 1].reg |= PORT_PMUX_PMUXO_B;      // Select channel B = AC channel 1 input = AIN[1]

    if(DEBUG) {
    // Connect the AC channel 1 output to PIN D10
    PORT->Group[g_APinDescription[AC_OUT_PIN].ulPort].PINCFG[g_APinDescription[AC_OUT_PIN].ulPin].bit.PMUXEN = 1;                 // Enable PORT multiplexer
    PORT->Group[g_APinDescription[AC_OUT_PIN].ulPort].PMUX[g_APinDescription[AC_OUT_PIN].ulPin >> 1].reg |= PORT_PMUX_PMUXO_H;    // Select channel H = AC channel 1 output = AC/CMP[1]
    }

    // Connect the TCC0 waveform output to PIN D7
    PORT->Group[g_APinDescription[PULSE_PIN].ulPort].PINCFG[g_APinDescription[PULSE_PIN].ulPin].bit.PMUXEN = 1;                     // Enable PORT multiplexer
    PORT->Group[g_APinDescription[PULSE_PIN].ulPort].PMUX[g_APinDescription[PULSE_PIN].ulPin >> 1].reg |= PORT_PMUX_PMUXO_F;        // Select channel F = Timer TCC0 waveform output = TCC0/WO[4]

    if(DEBUG) {
    // Connect the DAC output to PIN A0. Useful for DEBUGGING
    PORT->Group[g_APinDescription[REF_PIN].ulPort].PINCFG[g_APinDescription[REF_PIN].ulPin].bit.PMUXEN = 1;                       // Enable PORT multiplexer
    PORT->Group[g_APinDescription[REF_PIN].ulPort].PMUX[g_APinDescription[REF_PIN].ulPin].bit.PMUXO = PORT_PMUX_PMUXO_B_Val;      // Select channel B = DAC output = VOUT
    }


    // Event system ////////////////////////////////////////////////////////////////////////////

    EVSYS->USER.reg = EVSYS_USER_CHANNEL(1) |                                // Attach the event user (receiver) to channel 0 (n + 1)
                    EVSYS_USER_USER(EVSYS_ID_USER_TC4_EVU);                // Set the event user (receiver) as timer TC4

    EVSYS->CHANNEL.reg = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT |                // No event edge detection (because asynchronous)
                        EVSYS_CHANNEL_PATH_ASYNCHRONOUS |                   // Set event path as asynchronous
                        EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TCC0_MCX_3) |        // Set event generator (sender) as TCC 0
                        EVSYS_CHANNEL_CHANNEL(0);                           // Attach the generator (sender) to channel 0 


    // TC4 Timer ///////////////////////////////////////////////////////////////////////////////

    TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16;   // Configure TC4/TC5 timers for 16-bit mode
    while(TC4->COUNT16.STATUS.bit.SYNCBUSY);          // Wait for synchronization

    TC4->COUNT16.CTRLC.bit.CPTEN0 = 1;          // Enable capture on CC0
    while(TC4->COUNT16.STATUS.bit.SYNCBUSY);    // Wait for synchronization

    TC4->COUNT16.EVCTRL.reg |= TC_EVCTRL_TCEI |       // Enable input event
                                TC_EVCTRL_EVACT_RETRIGGER;   // Select event action
    while(TC4->COUNT16.STATUS.bit.SYNCBUSY);          // Wait for synchronization

    TC4->COUNT16.CTRLBSET.bit.ONESHOT |= 1;     // Enable oneshot mode
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);   // Wait for synchronization

    TC4->COUNT16.READREQ.reg = TC_READREQ_RCONT |                               // Enable a continuous read request
                                TC_READREQ_ADDR(TC_COUNT16_COUNT_OFFSET);        // Offset of the 32-bit COUNT register
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);                                   // Wait for synchronization

    TC4->COUNT16.CTRLA.bit.ENABLE = 1;          // Enable timer TC4
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);   // Wait for synchronization


    // Analog Comparator ///////////////////////////////////////////////////////////////////////

    AC->COMPCTRL[1].reg = AC_COMPCTRL_OUT_ASYNC |       // Enable comparator output in asynchronous mode
                        AC_COMPCTRL_MUXPOS_PIN1 |     // Set the positive input multiplexer to pin 1
                        AC_COMPCTRL_MUXNEG_DAC |      // Set the negative input multiplexer to the voltage scaler
                        AC_COMPCTRL_INTSEL_FALLING |  // Generate interrupts only on rising edge of AC output
                        AC_COMPCTRL_SPEED_HIGH;       // Place the comparator into high speed mode
    while (AC->STATUSB.bit.SYNCBUSY);                   // Wait for synchronization

    AC->INTENSET.bit.COMP1 |= 1;        // Enable interrupt for AC1
    while (AC->STATUSB.bit.SYNCBUSY);   // Wait for synchronization
    NVIC_SetPriority(AC_IRQn, 0);       // Set AC interrupt priority to highest
    NVIC_EnableIRQ(AC_IRQn);            // Enable Interrupts for AC at NVIC

    AC->CTRLA.bit.ENABLE = 1;                // Enable the analog comparator peripheral
    while (AC->STATUSB.bit.SYNCBUSY);        // Wait for synchronization

    AC->COMPCTRL[1].bit.ENABLE = 1;          // Enable the analog comparator channel 1
    while (AC->STATUSB.bit.SYNCBUSY);        // Wait for synchronization


    // TCC0 Timer //////////////////////////////////////////////////////////////////////////////

    TCC0->CTRLA.bit.PRESCALER |= TCC_CTRLA_PRESCALER_DIV64_Val;   // Divide 48MHz clock by 64 => 750KHz

    // Normal (single slope) PWM operation: timer countinuouslys count up to PER register value and then is reset to 0
    TCC0->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM |     // Setup single slope PWM on TCC0
                    TCC_WAVE_POL3;              // Invert output polarity
    while (TCC0->SYNCBUSY.bit.WAVE);              // Wait for synchronization

    TCC0->PER.reg = MAIN_CLK_FREQ_MHZ*1000000/(64 * PULSE_FREQ_HZ)-1;   // Set the frequency of the PWM on TCC0
    while (TCC0->SYNCBUSY.bit.PER);                                     // Wait for synchronization

    TCC0->CC[3].reg = (PULSE_WIDTH_US * MAIN_CLK_FREQ_MHZ)/64;          // Set the pulsewidth of the PWM on TCC0
    while (TCC0->SYNCBUSY.bit.CC3);                                     // Wait for synchronization

    TCC0->EVCTRL.reg |= TCC_EVCTRL_MCEO3;           // Enable Match/Capture event output

    TCC0->CTRLA.bit.ENABLE = 1;                     // Enable the TCC0 counter
    while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization


    // Digital to Analog Converter (DAC) ///////////////////////////////////////////////////////

    DAC->CTRLB.reg =  DAC_CTRLB_REFSEL_INT1V |    // Set 1V as reference voltage
                    DAC_CTRLB_IOEN |            // Enable output to internal reference
                    DAC_CTRLB_EOEN;             // Enable output on VOUT pin. Useful for DEBUGGING
    while(DAC->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    DAC->CTRLA.bit.ENABLE = 1;                    // Enable the DAC before writing data
    while(DAC->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    DAC->DATA.reg = DAC_DATA_DATA(100);           // 10 bit resolution on 1000mV. 100 => ~100 mv
    while(DAC->STATUS.bit.SYNCBUSY);              // Wait for synchronization


    // Enable interrupts
    __enable_irq();
}

void select(uint8_t channel) {
    if(channel & 0b100) {digitalWrite(COILSELC_PIN, HIGH);}
    else {digitalWrite(COILSELC_PIN, LOW);}
    if(channel & 0b010) {digitalWrite(COILSELB_PIN, HIGH);}
    else {digitalWrite(COILSELB_PIN, LOW);}
    if(channel & 0b001) {digitalWrite(COILSELA_PIN, HIGH);}
    else {digitalWrite(COILSELA_PIN, LOW);}
}

} // namespace pulse

// Interrupt handler for the Analog Comparator. Must be outside of any namespace
void AC_Handler(void) {
  // Check if compare interrupt
  if(AC->INTFLAG.bit.COMP1 && AC->INTENSET.bit.COMP1) {
    // Read timer value
    captured_value = TC4->COUNT16.COUNT.reg;
    
    // Clear interrupt flag by writing '1' to it
    AC->INTFLAG.reg = AC_INTFLAG_COMP1;
  }
}
