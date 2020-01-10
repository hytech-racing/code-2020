int wb_pin = A0;
int sg1_pin = A3;
int sg2_pin = A6;

int wb,sg2,sg1;

double A = .05112942;
double E = 2.97*pow(10,7);
double G = 2.0;
double R = 350;
double Vs = 3.3;

void setup() {
  // put your setup code here, to run once:
  pinMode(wb_pin,INPUT);
  pinMode(sg1_pin,INPUT);
  pinMode(sg2_pin,INPUT);
}

void loop() {
  // read voltage levels
  wb = analogRead(wb_pin);
  sg1 = analogRead(sg1_pin);
  sg2 = analogRead(sg2_pin);

  //convert scale from 1023 to voltage across wheatstone bridge
  double wbV, sg1V,sg2V;
  wbV = 3.3*wb/1023/1001;
  sg1V = 3.3*sg1/1023/1001;
  sg2V = 3.3*sg2/1023/1001;

//  //convert voltage for single strain gauge to resistance
//  double sg1r, sg2r, wbr;
//  sg1r = 350.0*3.3/(sg1V+1.65)-350.0;
//  sg2r = 350.0*3.3/(sg2V+1.65)-350.0;

  //change in resistance
  double sg1dr, sg2dr, wbdr;
  sg1dr = (-2.0*R*sg1V)/(sg1V+Vs/2);
  sg2dr = (-2.0*R*sg2V)/(sg2V+Vs/2);
  wbdr  = (2.0 *R*wbV)/(Vs-wbV);


  //convert change in resistance to force
  double sg1F, sg2F, wbF;
  sg1F = E*A*sg1dr/R/G;
  sg2F = E*A*sg2dr/R/G;
  wbF  = E*A*wbdr /R/G;

  

}
