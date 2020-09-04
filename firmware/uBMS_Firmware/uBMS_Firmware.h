#ifndef uBMS_Firmware
#define uBMS_Firmware

#define MAX_BAL_CURRENT = 750;      // In MilliAmperes
#define CELL_TEMP_B = 1;
#define BOARD_TEMP_B = 1;

#define INIT_VOLT_OFFSET = 0.00;
#define INIT_BAL_C_SLOPE = 1.00;
#define INIT_BAL_C_INTER = 0.00;

struct GlobalCalibrationParams{
    float voltageOffset;            // For general ADC calibration
    float voltageMultiplier;        // Can be calculated from offset or vice-versa
    float balanceCurrentSlope;      // Slope for the linear equation relating duty cycle to current
    float balanceCurrentIntercept;  // Intercept for the linear equation relating duty cycle to current
    float currentOffset;
    float currentMutliplier;
    float cellTempOffset;           // Temp offset for cell temperature sensors in deg C
    float boardTempOffset;          // Temp offset for board temeprature sensors in deg C
}

struct CellCalibrationParams{
    float voltageOffset;
    float seriesResistance;
    float parallelResistance;
    float parallelCapacitance;
}

struct UserSettings{
    float balancePower;
    float balanceCurrent;
    float balanceVoltage;
    float currentCutOff;
    float minCellTemp;
}

#endif