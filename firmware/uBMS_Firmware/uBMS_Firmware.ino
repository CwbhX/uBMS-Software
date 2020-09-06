#include "uBMS_Firmware.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "Adafruit_MCP23008.h"
#include "Adafruit_MCP23017.h"


Adafruit_ADS1115 ads1115(); // With default address of 0x48
Adafruit_MCP23008 mcp8;
Adafruit_MCP23017 mcp17;

void setupPinouts(){
    int i;
    // Setup MCP23008 Outputs
    for (i = 0; i < 8; i++){
        mcp8.pinMode(i, OUTPUT);
    }

    // Setup MCP23017 Outputs
    for (i = 0; i < 16; i++){
        mcp17.pinMode(i, OUTPUT);
    }
}

void setupPWM(){
    int i;
    
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION); // Can use up to 8 hardware timers or 16 channels in total w/ software, but ignoring for now
    
    ledcAttachPin(32, PWM_CHANNEL);  // 1
    ledcAttachPin(33, PWM_CHANNEL);  // 2
    ledcAttachPin(26, PWM_CHANNEL);  // 3
    ledcAttachPin(27, PWM_CHANNEL);  // 4
    ledcAttachPin(14, PWM_CHANNEL);  // 5
    ledcAttachPin(12, PWM_CHANNEL);  // 6
    ledcAttachPin(13, PWM_CHANNEL);  // 7
    ledcAttachPin(15, PWM_CHANNEL);  // 8
    ledcAttachPin(2,  PWM_CHANNEL);  // 9
    ledcAttachPin(4,  PWM_CHANNEL);  // 10
    ledcAttachPin(16, PWM_CHANNEL);  // 11
    ledcAttachPin(17, PWM_CHANNEL);  // 12

    // Should I init them all to zero duty cycle?
}

void setMux(enum muxSelect selectedMux, int selectedPin){
    int i;                         // For for loops
    uint8_t binBuff[4];
    itoa(selectedPin, binBuff, 2); // Convert the selected pin (0-15) into a array in base 2

    // Maybe I want to set them all to zero beforehand? or maybe not because I will get data from the zero element then...
    if(selectedMux == voltage){
        for(i = 0; i < 4; i++){
            mcp8.digitalWrite(i, binBuff[i]); // Write out the binary coded decimal to the pins for the voltage mux
        } 
    }else if(selectedMux == temperature){
        for(i = 4; i < 8; i++){
            mcp8.digitalWrite(i, binBuff[i-4]); // Write out the binary coded decimal to the pins for the temperature mux - notice we set higher pins, but access the same buffer so need to offset for that
        } 
    }
}


void update7Seg(int number){
    int i;
    uint8_t binBuff[8];

    // 7 Segment segments for given hex value written in dec
    uint8_t zero  = 252;
    uint8_t one   = 96;
    uint8_t two   = 218;
    uint8_t three = 242;
    uint8_t four  = 102;
    uint8_t five  = 182;
    uint8_t six   = 190;
    uint8_t seven = 224;
    uint8_t eight = 254;
    uint8_t nine  = 114;
    uint8_t A     = 238;
    uint8_t B     = 62;
    uint8_t C     = 156;
    uint8_t D     = 122;
    uint8_t E     = 158;
    uint8_t F     = 142;
    uint8_t dot   = 1;

    // Set the binBuff to the correct output for the 7 segment display to then write to the display
    switch(number){
        case 0:
            itoa(zero, binBuff, 2);
            break;
        case 1:
            itoa(one, binBuff, 2);
            break;
        case 2:
            itoa(two, binBuff, 2);
            break;
        case 3:
            itoa(three, binBuff, 2);
            break;
        case 4:
            itoa(four, binBuff, 2);
            break;
        case 5:
            itoa(five, binBuff, 2);
            break;
        case 6:
            itoa(six, binBuff, 2);
            break;
        case 7:
            itoa(seven, binBuff, 2);
            break;
        case 8:
            itoa(eight, binBuff, 2);
            break;
        case 9:
            itoa(nine, binBuff, 2);
            break;
        case 10:
            itoa(A, binBuff, 2);
            break;
        case 11:
            itoa(B, binBuff, 2);
            break;
        case 12:
            itoa(C, binBuff, 2);
            break;
        case 13:
            itoa(D, binBuff, 2);
            break;
        case 14:
            itoa(E, binBuff, 2);
            break;
        case 15:
            itoa(F, binBuff, 2);
            break;
        case 16:
            itoa(dot, binBuff, 2);
            break;

        default:
            itoa(255, binBuff, 2);
            Serial.println("Error reading value for 7 seg");
    }

    // Write the digital outputs to the seven segment display
    for(i = 0; i < 8; i++){
        mcp17.digitalWrite(i, binBuff[i]);
    }
}

float readADCVoltage(enum ADCSelect selectedChannel){
    int16_t adcVal = ads1115.readADC_SingleEnded(selectedChannel); // Measures the voltage on the respective ADC Channel
    return (float) (adcVal/32768)*6.144;                           // ADC value / Total steps * max voltage for given PGA setting
}

float calculateCellVoltage(float voltage, int cellNum){
    /*
        NEED TO COMPENSATE FOR NON-PERFECT RESISTORS FOR EACH MEASUREMENT
    */
    return voltage;
}

float calculateCellTemp(float voltage){
    float R = 22000*((5/voltage) - 1);               // Voltage divider equation solving for R1 with a 22k for R2 - Ripple on the 5v rail might affect readings
    return (float)(CELL_TEMP_B/log(R/EXT_R_INF));    // Beta Parameter Equation: https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation
}

float calculateBoardTemp(float voltage){
    float R = 4700*((5/voltage) - 1);                // Voltage divider equation solving for R1 with a 4.7k for R2
    return (float)(BOARD_TEMP_B/log(R/INT_R_INF));   // Beta Parameter Equation: https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation
}

float calculateDCurrent(float voltage){
    return (float)(((voltage-2.5)/100)/0.001);       // Calculate Current being drain by subtracting offset, dividing out the gain, and then using V=IR to get I
}

void setup() {
    EXT_R_INF = R_ZERO*exp((-CELL_TEMP_B)/25+KELVIN);
    INT_R_INF = R_ZERO*exp((-BOARD_TEMP_B)/25+KELVIN);

    ads1115.begin();    // Initialise ads1115
    mcp8.begin(1);      // Initialise MCP23008 with address 0x21
    mcp17.begin();      // Initialise MCP23017 with address 0x20
    setupPinouts();     // Setup Pinouts of MCPs, etc...
    setupPWM();
}

void loop() {
    // put your main code here, to run repeatedly:

}
