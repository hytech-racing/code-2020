#include <AutoPID.h>
//the main issue with this implementation is the divide by zero, which interferes with the PID's operation at extreme low velocities, which may or may not interfere with the PID's effectiveness
  //General variables
  double frontTheta = 0; //radians per second for the front wheels; true velocity
  double rearTheta = ; // radians per second for the rear wheels; spin velocity
  
  //Tslip_command equation variables
  double halfMass; //half the car's mass
  double r; //radius of the wheel
  double inertia; //inertia of the wheel
  double uLast = 0; //the previous velocity reading
  double uCurrent; //the current velocity reading;
  double uDelta; //change in the velocity
  double Tslip_command; // the torque command that will be passed to the motor controller
  double gearRatio = 40/9; //gear ratio of the current vehicle; ig in the equation given by the paper
  
  //PID equation variables
  double optimalSlipRatio = .15; //the optimal slip ratio
  double slipRatio = optimalSlipRatio; // The initial slip ratio is set as the optimal slip ratio as outlined in equation 21 of the paper
  double lastTime = 0; //the time at the last PID run
  double currentTime; // the time at the current PID run;
  double PIDin; //this is the error = optimalSlipRatio - slipRatio
  double PIDout; //this is the target change in slip ratio
  double outputMin = 0;
  double outputMax = 1;
  double kp = 1;
  double ki = 0;

  AutoPID pid(&PIDin, &slipSetPoint, &PIDout, outputMin, outputMax, kp, ki, kd);
  
void setup() {
  // put your setup code here, to run once:
  pid.setTimeStep(10); //interval for PID to run at, in milliseconds
}

void loop() {
  //calculate the error in the change in slip ratio, which is the PIDin
  if (frontTheta) {slipRatio = rearTheta / frontTheta - 1;}
  PIDin = optimalSlipRatio - slipRatio; //error term
  pid.run();
  uCurrent = r*rearTheta;
  uDelta = uCurrent - uLast;
  Tslip_command = (halfMass * uDelta * r + (PIDout * uCurrent + uDelta) * inertia / r*(1 - slipRatio)) / gearRatio;
}
