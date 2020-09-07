#include "uBMS_Firmware.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "Adafruit_MCP23008.h"
#include "Adafruit_MCP23017.h"


Adafruit_ADS1115 ads1115; // With default address of 0x48
Adafruit_MCP23008 mcp8;
Adafruit_MCP23017 mcp17;
//static TaskHandle_t xTaskToNotify = NULL;

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
    itoa(selectedPin, muxBuff, 2); // Convert the selected pin (0-15) into a array in base 2

    // Maybe I want to set them all to zero beforehand? or maybe not because I will get data from the zero element then...
    if(selectedMux == MX_voltage){
        for(i = 0; i < 4; i++){
            mcp8.digitalWrite(i, muxBuff[i]); // Write out the binary coded decimal to the pins for the voltage mux
        } 
    }else if(selectedMux == MX_temperature){
        for(i = 4; i < 8; i++){
            mcp8.digitalWrite(i, muxBuff[i-4]); // Write out the binary coded decimal to the pins for the temperature mux - notice we set higher pins, but access the same buffer so need to offset for that
        } 
    }
}


void update7Seg(int number){
    int i;
    //char segBuff[8]
    //char binBuff[8]; It crashes with Stack smashing protect failure! if reinitialising

    // This method uses more RAM - but easier to program than storing in a int
    char zero[8]  = {1, 1, 1, 1, 1, 1, 0, 0};
    char one[8]   = {0, 1, 1, 0, 0, 0, 0, 0};
    char two[8]   = {1, 1, 0, 1, 1, 0, 1, 0};
    char three[8] = {1, 1, 1, 1, 0, 0, 1, 0};
    char four[8]  = {0, 1, 1, 0, 0, 1, 1, 0};
    char five[8]  = {1, 0, 1, 1, 0, 1, 1, 0};
    char six[8]   = {1, 0, 1, 1, 1, 1, 1, 0};
    char seven[8] = {1, 1, 1, 0, 0, 0, 0, 0};
    char eight[8] = {1, 1, 1, 1, 1, 1, 1, 0};
    char nine[8]  = {1, 1, 1, 0, 0, 1, 1, 0};
    char A[8]     = {1, 1, 1, 0, 1, 1, 1, 0};
    char B[8]     = {0, 0, 1, 1, 1, 1, 1, 0};
    char C[8]     = {1, 0, 0, 1, 1, 1, 0, 0};
    char D[8]     = {0, 1, 1, 1, 1, 0, 1, 0};
    char E[8]     = {1, 0, 0, 1, 1, 1, 1, 0};
    char F[8]     = {1, 0, 0, 0, 1, 1, 1, 0};
    char dot[8]   = {0, 0, 0, 0, 0, 0, 0, 1};
    char all[8]   = {1, 1, 1, 1, 1, 1, 1, 1};

    // Set the binBuff to the correct output for the 7 segment display to then write to the display
    switch(number){
        case 0:
            memcpy(segBuff, zero, sizeof(segBuff));
            break;
        case 1:
            memcpy(segBuff, one, sizeof(segBuff));
            break;
        case 2:
            memcpy(segBuff, two, sizeof(segBuff));
            break;
        case 3:
            memcpy(segBuff, three, sizeof(segBuff));
            break;
        case 4:
            memcpy(segBuff, four, sizeof(segBuff));
            break;
        case 5:
            memcpy(segBuff, five, sizeof(segBuff));
            break;
        case 6:
            memcpy(segBuff, six, sizeof(segBuff));
            break;
        case 7:
            memcpy(segBuff, seven, sizeof(segBuff));
            break;
        case 8:
            memcpy(segBuff, eight, sizeof(segBuff));
            break;
        case 9:
            memcpy(segBuff, nine, sizeof(segBuff));
            break;
        case 10:
            memcpy(segBuff, A, sizeof(segBuff));
            break;
        case 11:
            memcpy(segBuff, B, sizeof(segBuff));
            break;
        case 12:
            memcpy(segBuff, C, sizeof(segBuff));
            break;
        case 13:
            memcpy(segBuff, D, sizeof(segBuff));
            break;
        case 14:
            memcpy(segBuff, E, sizeof(segBuff));
            break;
        case 15:
            memcpy(segBuff, F, sizeof(segBuff));
            break;
        case 16:
            memcpy(segBuff, dot, sizeof(segBuff));
            break;

        default:
            memcpy(segBuff, all, sizeof(segBuff));
            Serial.println("Error reading value for 7 seg");
    }

    // Write the digital outputs to the seven segment display
    for(i = 0; i < 8; i++){
        mcp17.digitalWrite(i, segBuff[i]);
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
    Serial.begin(115200);

    EXT_R_INF = R_ZERO*exp((-CELL_TEMP_B)/25+KELVIN);
    INT_R_INF = R_ZERO*exp((-BOARD_TEMP_B)/25+KELVIN);
    detectedCells = 0;
    sampleCounter = 0;
    voltageSampleCounter = 0;
    temperatureSampleCounter = 0;

    ads1115.begin();    // Initialise ads1115
    // TODO: Use Library Supporting different SPS as default Adafruit sets to 128SPS
    mcp8.begin(1);      // Initialise MCP23008 with address 0x21
    mcp17.begin();      // Initialise MCP23017 with address 0x20
    setupPinouts();     // Setup Pinouts of MCPs, etc...
    setupPWM();

    //configASSERT( xTaskToNotify == NULL );
    //xTaskToNotify = xTaskGetCurrentTaskHandle();
}


uint8_t testZero  = 252;

void loop() {

    for(int num = 0; num < 18; num++){
        update7Seg(num);
        delay(500);
    }

    delay(1000);
}
