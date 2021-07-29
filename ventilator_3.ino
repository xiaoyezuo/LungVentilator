#include <math.h>
#include <ArduinoJson.h>
#include <ADC.h>
#include <ADC_util.h>
#include <PID_v1.h>
# define M_PI           3.14159265358979323846
ADC *adc = new ADC(); // adc object

const int SAMPLE_TIME = 50;
double measured_pressure;
double control = 0;
double target_pressure = 505;
double start_time = 0;
double elapsed_time = 0;
double target = 505;
String werd;
String command;

double Kp=4, Ki=0.4, Kd=0.5;
//double Kp=1, Ki=0.1, Kd=0.25; //PID constants, 4, 0.4, 0.5
//double Kp=0.25, Ki=0.025, Kd=0.06; //PID constants, 4, 0.4, 0.5

PID myPID(&measured_pressure, &control, &target_pressure, Kp, Ki, Kd, DIRECT);
elapsedMillis t;

void setup()
{     
  pinMode(A1, INPUT);
  pinMode(14, OUTPUT);      
  Serial.begin(115200); //auto set for teensy to something fast?
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(SAMPLE_TIME);
  
  //// ADC ////
  adc->adc0->setAveraging(100);                                    // set number of averages
  adc->adc0->setResolution(10);                                   // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);     // change the sampling speed
  delay(2000);
}

void loop()                     
{
  elapsedMillis t0;
  //---------------------------------------------------------
  //shutdown after 60 seconds 
  //---------------------------------------------------------
  elapsed_time = (millis() - start_time)/1000;
  //508
  if((elapsed_time > 60)||(target_pressure <= 508)||(target <= 508)){
    control = 0;
  }

  //---------------------------------------------------------
  //update actuators / fan
  //---------------------------------------------------------
    analogWrite(14, control);//pwm 0-255
    
  //---------------------------------------------------------
  //get command from serial port
  //---------------------------------------------------------
    while(Serial.available() > 0){
      //delay(5);
      werd = Serial.read();
      command += werd;
    }
    Serial.flush();
    if(command.length() > 0){
      target = command.toInt();
      start_time = millis();//start counting time after last command
    }
    
  //---------------------------------------------------------
  //target pressure setpoints 
  //---------------------------------------------------------
    target_pressure = target - 80 + 80*sin(M_PI/5 * millis()/1000); //range reduce by 20 each side 
    //target_pressure = target;
    target_pressure = constrain(target_pressure, 0, 1024); //clip pressure setpoints to ADC range

  //---------------------------------------------------------
  //read sensors
  //---------------------------------------------------------
      measured_pressure = adc->analogRead(A1);
          
  //---------------------------------------------------------
  //calculate control inputs
  //---------------------------------------------------------
    myPID.Compute();


    //---------------------------------------------------------
    //print to serial
    //---------------------------------------------------------
//    Serial.println(target_pressure - measured_pressure, t);
    Serial.println(measured_pressure, t);

    if(t0 < SAMPLE_TIME){
      delay(SAMPLE_TIME - t0);
    }
//    Serial.println(t0);

    command = "";

}
