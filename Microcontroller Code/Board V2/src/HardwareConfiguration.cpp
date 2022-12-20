#include "HardwareConfiguration.h"

HardwareConfig::HardwareConfig(){}

// TODO:Fix the difference between the chipPin and the actual arduino referece pin number

void HardwareConfig::setup()
{
    Pins[0] = Pin(1, "3.3V");
    Pins[1] = Pin(1, "Ground");
    Pins[2] = Pin(2, "Ground");
    Pins[3] = Pin(3, "Ground");
    Pins[4] = Pin(4, "Ground");
    Pins[5] = Pin(5, "Ground");
    Pins[6] = Pin(6, "Ground");
    Pins[7] = Pin(7, "Ground");
    Pins[8] = Pin(8, "Ground");
    Pins[9] = Pin(7, "5V");
    int pinStartingID = setupJacks(9);

    // Add the pins that arn't on a jack but we still need them
    Pins[pinStartingID+1] = Pin(12, 2, 12, "MOSI");
    Pins[pinStartingID+2] = Pin(13, 2, 13, "MISO");
    Pins[pinStartingID+3] = Pin(11, 2, 11, "SCK");
    Pins[pinStartingID+4] = Pin(10, 2, 10, "ADC Chip Select");
    Pins[pinStartingID+5] = Pin(20, 2, 0, "ADC Ready"); // this is set to 0? not pin 20?
    Pins[pinStartingID+6] = Pin(21, 2, 1, "ADC Str"); // thi is set to 1? not pin 21?
    Pins[pinStartingID+7] = Pin(21, 2, A4, "ADC Reset");

    // I2C set in the screen setup

    SPIBus.MISO = &Pins[pinStartingID+2];
    SPIBus.MOSI = &Pins[pinStartingID+1];
    SPIBus.SCK = &Pins[pinStartingID+3];

    ADCPin.CS = &Pins[pinStartingID+4];
    ADCPin.STR = &Pins[pinStartingID+6];
    ADCPin.RST = &Pins[pinStartingID+4];
    ADCPin.RDY = &Pins[pinStartingID+5];

}

void HardwareConfig::setupPins()
{
    pinMode(EncoderPin.A->ChipPin, INPUT);
    pinMode(EncoderPin.B->ChipPin, INPUT);
    pinMode(EncoderPin.SW_NO->ChipPin, INPUT);

    pinMode(MotorPin.DIR->ChipPin, OUTPUT);
    pinMode(MotorPin.SLP->ChipPin, OUTPUT);
    pinMode(MotorPin.CS->ChipPin, INPUT); // pin conflict

    pinMode(Max.NC->ChipPin, INPUT);
    pinMode(Min.NC->ChipPin, INPUT);
    pinMode(Lid.NC->ChipPin, INPUT);

    // these were not included in the last version?
    pinMode(Stop.NC->ChipPin, INPUT);
    pinMode(Start.NO->ChipPin, INPUT);


    pinMode(ADCPin.STR->ChipPin, INPUT);
    pinMode(ADCPin.RDY->ChipPin, INPUT);
    pinMode(ADCPin.CS->ChipPin, INPUT); // pin conflict
    pinMode(ADCPin.RST->ChipPin, OUTPUT);
    
    pinMode(Dist_VAL, INPUT);
    
}

int HardwareConfig::setupJacks(int pinStartingID)
{
    pinStartingID = setupStart(0,++pinStartingID);
    pinStartingID = setupMin(1,++pinStartingID);
    pinStartingID = setupMotor(2,++pinStartingID);
    pinStartingID = setupDistance(3,++pinStartingID);
    pinStartingID = setupStop(4, ++pinStartingID);
    pinStartingID = setupMax(5, ++pinStartingID);
    pinStartingID = setupScreen(6, ++pinStartingID);
    pinStartingID = setupEncoder(7, ++pinStartingID);
    pinStartingID = setupLid(8, ++pinStartingID);
    pinStartingID = setupSensor(9, ++pinStartingID);
    return pinStartingID;
    
}

int HardwareConfig::setupStart(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Start");
    //Pins[pinStartingID]   = Pin(1, "3.3V");
    //Pins[pinStartingID+1] = Pin(2, "Ground");
    Pins[pinStartingID] = Pin(3, "Start NC");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+1] = Pin(5, 2, 30,"Start NO");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    Pins[pinStartingID+2] = Pin(7, "Start LED + (5V)");
    Pins[pinStartingID+3] = Pin(8, "Start LED - (0V)");
    Jacks[jackID].setPins(
        &Pins[0], // 3.3V
        &Pins[2], // ground pin 2
        &Pins[pinStartingID], // start nc
        &Pins[4], // ground pin 4
        &Pins[pinStartingID+1], // start no
        &Pins[6], // ground pin 6
        &Pins[pinStartingID+2], // start + led
        &Pins[pinStartingID+3] // start - led
    );
    Start.NO = &Pins[pinStartingID+1];
    return pinStartingID+3;
}

int HardwareConfig::setupMin(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Min");
    //Pins[pinStartingID]   = Pin(1, "Min Common (3.3V)");
    //Pins[pinStartingID+1] = Pin(2, "Ground");
    Pins[pinStartingID] = Pin(3, 2, 14, "Min NC");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+1] = Pin(5, "Min NO");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    //Pins[pinStartingID+6] = Pin(7, "Ground");
    //Pins[pinStartingID+7] = Pin(8, "Ground");
    Jacks[jackID].setPins(
        &Pins[0],
        &Pins[2],
        &Pins[pinStartingID], // min nc
        &Pins[4],
        &Pins[pinStartingID+1], // min no
        &Pins[6],
        &Pins[7],
        &Pins[8]
    );
    Min.NC = &Pins[pinStartingID];
    return pinStartingID + 1;
}

int HardwareConfig::setupMotor(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Motor");
    //Pins[pinStartingID]   = Pin(1, "Ground");
    Pins[pinStartingID] = Pin(2, 2, A0, "Motor Current Sense"); // Pin 5
    Pins[pinStartingID+1] = Pin(3, 2, 14, "Motor Direction");
    Pins[pinStartingID+2] = Pin(4, 2, A2, "Motor Sleep"); // pin 7
    Pins[pinStartingID+3] = Pin(5, 2, 24, "Motor PWM");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    //Pins[pinStartingID+6] = Pin(7, "Ground");
    //Pins[pinStartingID+7] = Pin(8, "Ground");
    Jacks[jackID].setPins(
        &Pins[1],
        &Pins[pinStartingID], // current sense
        &Pins[pinStartingID+1], // dir
        &Pins[pinStartingID+2], // sleep
        &Pins[pinStartingID+3], // pwm
        &Pins[6],
        &Pins[7],
        &Pins[8]
    );
    MotorPin.CS = &Pins[pinStartingID];
    MotorPin.DIR = &Pins[pinStartingID+1];
    MotorPin.PWM = &Pins[pinStartingID+3];
    MotorPin.SLP = &Pins[pinStartingID+2];
    return pinStartingID + 3;
}

int HardwareConfig::setupDistance(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Distance");
    //Pins[pinStartingID]   = Pin(1, "Ground");
    Pins[pinStartingID] = Pin(2, 2, 6, "Distance Value");
    //Pins[pinStartingID+2] = Pin(3, "Ground");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    //Pins[pinStartingID+4] = Pin(5, "Ground");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    //Pins[pinStartingID+6] = Pin(7, "5V");
    //Pins[pinStartingID+7] = Pin(8, "Ground");
    Jacks[jackID].setPins(
        &Pins[1],
        &Pins[pinStartingID],// distance
        &Pins[3],
        &Pins[4],
        &Pins[5],
        &Pins[6],
        &Pins[9],
        &Pins[8]
    );
    return pinStartingID;
}

int HardwareConfig::setupStop(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Stop");
    //Pins[pinStartingID]   = Pin(1, "Stop Common (3.3V)");
    //Pins[pinStartingID+1] = Pin(2, "Ground");
    Pins[pinStartingID] = Pin(3, 2, 29, "Stop NC");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+1] = Pin(5, "Stop NO");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    Pins[pinStartingID+2] = Pin(7, "Stop LED + (5V)");
    Pins[pinStartingID+3] = Pin(8, "Stop LED - (0V)");
    Jacks[jackID].setPins(
        &Pins[0],
        &Pins[2],
        &Pins[pinStartingID], // stop nc
        &Pins[4],
        &Pins[pinStartingID+1], // stop no
        &Pins[6],
        &Pins[pinStartingID+2], // stop led +
        &Pins[pinStartingID+3] // stop led -
    );
    Stop.NC = &Pins[pinStartingID];
    return pinStartingID + 3;
}

int HardwareConfig::setupMax(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Max");
    //Pins[pinStartingID]   = Pin(1, "Max Common (3.3V)");
    //Pins[pinStartingID+1] = Pin(2, "Ground");
    Pins[pinStartingID] = Pin(3, 2, 19, "Max NC");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+1] = Pin(5, "Max NO");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    //Pins[pinStartingID+6] = Pin(7, "Ground");
    //Pins[pinStartingID+7] = Pin(8, "Ground");
    Jacks[jackID].setPins(
        &Pins[0],
        &Pins[2],
        &Pins[pinStartingID], // max nc
        &Pins[4],
        &Pins[pinStartingID+1], // max no
        &Pins[6],
        &Pins[7],
        &Pins[8]
    );
    Max.NC = &Pins[pinStartingID];
    return pinStartingID + 1;
}

int HardwareConfig::setupScreen(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Screen");
    //Pins[pinStartingID]   = Pin(1, "Ground");
    //Pins[pinStartingID+1] = Pin(2, "Ground");
    Pins[pinStartingID] = Pin(3, 2, 23, "SCL");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+1] = Pin(5, 2, 22,"SDA");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    //Pins[pinStartingID+6] = Pin(7, "5V");
    //Pins[pinStartingID+7] = Pin(8, "Ground");
    Jacks[jackID].setPins(
        &Pins[1],
        &Pins[2],
        &Pins[pinStartingID],
        &Pins[4],
        &Pins[pinStartingID+1],
        &Pins[6],
        &Pins[9],
        &Pins[8]
    );
    I2CBus.SCL = &Pins[pinStartingID];
    I2CBus.SDA = &Pins[pinStartingID+1];
    return pinStartingID + 1;
}

int HardwareConfig::setupEncoder(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Encoder");
    //Pins[pinStartingID] = Pin(1,"Encoder Common Rotary (3.3V)");
    Pins[pinStartingID] = Pin(2,"3.3V");
    Pins[pinStartingID+1] = Pin(3, 2, 28, "Encoder A");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+2] = Pin(5, 2, 27,"Encoder B");
    Pins[pinStartingID+3] = Pin(6, 2, 26, "Encoder Switch NO");
    //Pins[pinStartingID+6] = Pin(7,"Ground");
    //Pins[pinStartingID+7] = Pin(8,"Ground");
    Jacks[jackID].setPins(
        &Pins[0],
        &Pins[pinStartingID], // 3.3 V but on pin 2
        &Pins[pinStartingID+1], // enc a
        &Pins[4],
        &Pins[pinStartingID+2], // enc b
        &Pins[pinStartingID+3], // enc no
        &Pins[7],
        &Pins[8]
    );
    EncoderPin.A = &Pins[pinStartingID+1];
    EncoderPin.B = &Pins[pinStartingID+2];
    EncoderPin.SW_NO = &Pins[pinStartingID+3];
    return pinStartingID + 3;
}

int HardwareConfig::setupLid(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Lid");
    //Pins[pinStartingID]   = Pin(1, "Lid Common (3.3V)");
    //Pins[pinStartingID+1] = Pin(2, "Ground");
    Pins[pinStartingID] = Pin(3, 2, 18, "Lid NC");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+1] = Pin(5, "Max NO");
    //Pins[pinStartingID+5] = Pin(6, "Ground");
    //Pins[pinStartingID+6] = Pin(7, "Ground");
    //Pins[pinStartingID+7] = Pin(8, "Ground");
    Jacks[jackID].setPins(
        &Pins[0],
        &Pins[3],
        &Pins[pinStartingID], // lid nc
        &Pins[4],
        &Pins[pinStartingID+1], // lid no
        &Pins[6],
        &Pins[7],
        &Pins[8]
    );
    Lid.NC = &Pins[pinStartingID];
    return pinStartingID + 1;
}

int HardwareConfig::setupSensor(int jackID, int pinStartingID)
{
    Jacks[jackID] = RJ45("Encoder");
    //Pins[pinStartingID] = Pin(1,"Ground");
    //Pins[pinStartingID+1] = Pin(2,"Ground");
    Pins[pinStartingID] = Pin(3, 5, 8, "Sensor S-");
    //Pins[pinStartingID+3] = Pin(4, "Ground");
    Pins[pinStartingID+1] = Pin(5, 5, 7,"Sensor S+");
    //Pins[pinStartingID+5] = Pin(6,"Ground");
    //Pins[pinStartingID+6] = Pin(7,"Sensor Power");
    //Pins[pinStartingID+7] = Pin(8,"Ground");
    Jacks[jackID].setPins(
        &Pins[1],
        &Pins[2],
        &Pins[pinStartingID], // S-
        &Pins[4],
        &Pins[pinStartingID+1], // S+
        &Pins[6],
        &Pins[9], // 5V
        &Pins[8]
    );
    return pinStartingID + 1;
}

