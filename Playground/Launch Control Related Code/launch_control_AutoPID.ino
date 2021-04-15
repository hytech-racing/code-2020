#include <AutoPID.h>
//the main issue with this implementation is the divide by zero, which interferes with the PID's operation at extreme low velocities, which may or may not interfere with the PID's effectiveness

double rpsFront; //radians per second for the front wheels; true velocity
  double rpsRear; // radians per second for the rear wheels; spin velocity
  double slipSetPoint = 0.07; //setpoint for the PID controller to attempt to match
  double PIDin; //current slip ratio
  double PIDout; //"necessary" slip ratio; in actuality represents the amount of torque necessary to drive the wheels toward the setpoint
  double outputMin = 0;
  double outputMax = 0;
  double kp = 1;
  double ki = 0;
  double kd = 0;
  double optimalRps;
  double currentPower;
  double newTorque;
  AutoPID pid(&PIDin, &slipSetPoint, &PIDout, outputMin, outputMax, kp, ki, kd);
  
void setup() {
  // put your setup code here, to run once:
  pid.setTimeStep(10); //interval for PID to run at, in milliseconds
}

void loop() {
  //rpsRear and rpsFront enter PID loop from the wheel speed sensors; currentPower enters the loop from the motor controller
  if (rpsRear == 0) {
    PIDin = 0;
  } else { 
    PIDin = rpsRear / rpsFront - 1;
  }
  pid.run();
  //Optimal angular velocity = (slip ratio + 1) * base angular velocity
  optimalRps = (PIDout + 1) * rpsFront;
  //Power = Torque * angular velocity -> divide by zero becomes necessary when formula is rearranged
  if (optimalRps == 0) {
    newTorque = currentPower / optimalRps;
  }
  //newTorque is sent to the torque controller for appropriate processing
}
