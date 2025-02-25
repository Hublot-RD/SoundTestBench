#include "motor.hpp"

namespace motor {

void setup(void) {
    /**
     * @brief Setup the motor library
    */
    // Setup the motors pins
    pinMode(A_DIR_PIN, OUTPUT);
    pinMode(A_PWM_PIN, OUTPUT);
    pinMode(A_BRK_PIN, OUTPUT);
    pinMode(B_DIR_PIN, OUTPUT);
    pinMode(B_PWM_PIN, OUTPUT);
    pinMode(B_BRK_PIN, OUTPUT);

    // Stop the motors
    stop(true); stop(false);

    // Disengage brakes
    digitalWrite(A_BRK_PIN, LOW); digitalWrite(B_BRK_PIN, LOW);


}

void set_speed(uint32_t speed, bool motor_A) {
    /**
     * @brief Set the speed of a motor
     * 
     * @param speed: Desired speed to set, in mRPM [0, MAX_SPEED]
     * @param motor_A: True for motor A, false for motor B
    */
    // Limit the speed
    if(speed > MAX_SPEED) {speed = MAX_SPEED;}

    // Disengage brake if desired speed is over 0
    if (speed > 0) {
        if(motor_A) {digitalWrite(A_BRK_PIN, LOW);}
        else {digitalWrite(B_BRK_PIN, LOW);}
    }

    // Convert speed to PWM value
    uint16_t PWM_speed = map(speed, 0, MAX_SPEED, 0, 255);

    // Apply desired speed
    if(motor_A) {analogWrite(A_PWM_PIN, PWM_speed);}
    else {analogWrite(B_PWM_PIN, PWM_speed);}
}

void set_direction(bool forward, bool motor_A) {
    /**
     * @brief Set the direction of a motor
     * 
     * @param forward: True for forward, false for backward
     * @param motor_A: True for motor A, false for motor B
    */
    if(motor_A) {digitalWrite(A_DIR_PIN, forward);}
    else{digitalWrite(B_DIR_PIN, forward);}
}

void stop(bool motor_A) {
    /**
     * @brief Stop a motor
     * 
     * @param motor_A: True for motor A, false for motor B
    */
    // Apply brake
    if(motor_A) {digitalWrite(A_BRK_PIN, HIGH);}
    else {digitalWrite(B_BRK_PIN, HIGH);}

    // Set desired speed to 0
    set_speed(0, motor_A);
}

} // namespace motor