#ifndef uBMS_Firmware
#define uBMS_Firmware

#define MAX_BAL_CURRENT = 750;      // In MilliAmperes
#define CELL_TEMP_B = 3950;
#define BOARD_TEMP_B = 3380;
#define R_ZERO = 10000;

#define INIT_VOLT_OFFSET = 0.00;
#define INIT_BAL_C_SLOPE = 1.00;
#define INIT_BAL_C_INTER = 0.00;

#define PWM_FREQUENCY = 5000;
#define PWM_RESOLUTION = 12;
#define PWM_CHANNEL = 0;

#define MOSFET_T_LOC = 12;
#define BAL_T_LOC = 13;
#define POWER_T_LOC = 14;
#define REG_T_LOC = 15;

enum ThermistorLocation{MOSFET, Balance, Regulator, PowerSupply};
enum muxSelect{voltage, temperature};
enum ADCSelect{voltage, temperature, current, external};

// Board Configuration Parameters
struct GlobalCalibrationParams{
    float voltageOffset;            // For general ADC calibration
    float voltageMultiplier;        // Can be calculated from offset or vice-versa
    float balanceCurrentSlope;      // Slope for the linear equation relating duty cycle to current
    float balanceCurrentIntercept;  // Intercept for the linear equation relating duty cycle to current
    float currentOffset;
    float currentMutliplier;
    float cellTempOffset;           // Temp offset for cell temperature sensors in deg C
    float boardTempOffset;          // Temp offset for board temeprature sensors in deg C
};

// Cell Specific Calibration Parameters
struct CellCalibrationParams{
    float voltageOffset;            // Voltage offset from actual voltage in millivolts
    float seriesResistance;         // Series resistance for the cell in an RRC model
    float parallelResistance;       // Parallel resistance for the cell in an RRC model
    float parallelCapacitance;      // Parallel capacitance for the cell in an RRC model
};

// Board User Settings
struct UserSettings{
    float balancePower;             // The set balance power the user wants to target in Watts
    float balanceCurrent;           // The set balance current the user want to target in milliamperes (can be calculated)
    float balanceVoltage;           // The set voltage the user wants to target for balancing to begin for a cell in volts
    float currentCutOff;            // The set discharge current the user wants to target for disconnecting the power in amperes
    float minCellTemp;              // The minimum allowable cell temperature to allow discharge and or balancing in deg C
};

// Data for a cell
struct cellData{
    bool detected;                  // Is this a valid cell - Has it been detected - Allows
    float cellVoltage;              // Cell's voltage in volts
    float cellTemperature;          // Cell's temperature in deg C
    float coulombsIn;               // Cell's charge gone into the cell in C
    float coulombsOut;              // Cell's charge left the cell in C
    float coulombsBalanced;         // Cell's charge left due to balancing in C
    int balanceTally;               // Number of times balancing has been activated on this cell
    int SOC;                        // Estimated State of Charge for the cell in %
    struct CellCalibrationParams calibrationParams; // Cell Specific Calibrations
};

// Data for an on-board thermistor
struct thermistorData{
    enum ThermistorLocation location; // Location of the thermistor on the PCB
    int ADCval;                       // Raw ADC value of the thermistor
    float degC;                       // Calculated temperature from ADC value in deg C
};


const float EXT_R_INF;
const float INT_R_INF;

int detectedCells;                    // Number of detected cells by the BMS
bool balancing[12];                   // Boolean array of which cells are currently being balanced (useful for the GUI)

struct cellData cells[12];            // Array for all the cells and their respective data
struct thermistorData boardTemps[4];  // Array for the temperature sensors on the board

void setupPinouts();
void setupPWM();

void setMux(enum muxSelect selectedMux, int selectedPin);
void update7Seg(int number);

float readADCVoltage(enum ADCSelect selectedChannel);
float calculateCellVoltage(float voltage, int cellNum);
float calculateCellTemp(float voltage);
float calculateBoardTemp(float voltage);
float calculateDCurrent(float voltage);


#endif