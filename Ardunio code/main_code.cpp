/***************************************HEADER COMMENT***************************************

   Program: main_code.cpp
   Author:  Kareem Badreldin
   Created on: 15th of July, 2023
   Last modified: 24th of August,2023
   Programming language: C++
   Purpose:  This Arduino program controls the ball and plate system using two servos and a
             a touchscreen. The program reads the ball's position, and then utilizes a PID 
             controller to generate a control signal that guides the ball to the reference
             point. The program also boasts the following features:
             - Noise filtering for touchscreen readings.
             - Kinematic model used to translate touchscreen angles to servo positions.
             - Control signal saturation to prevent servo angle limits violation.
             
********************************************************************************************/

#include <Arduino.h>
#include <Servo.h>

#define Servo1PIN 9
#define Servo2PIN 11
#define Xp A0
#define Xn A1
#define Yp A2
#define Yn A3

Servo Servo1; // X axis servo
Servo Servo2; // Y axis servo
double X_coor,X_coor_prev,X_disp; // X position variables
double Y_coor,Y_coor_prev,Y_disp; // Y position variables
float Xtheta1,Ytheta1; // Touchscreen angles
double Xtheta2,Ytheta2; // Servo angles 
double Xerror_curr,Xerror_prev, Xerror_total,Xde ; // X Error variables
double Yerror_curr,Yerror_prev, Yerror_total,Yde ; // Y Error variables

// time variables
unsigned long Time_prev,Time_curr ; 
double dt; 

int X_level_angle = 83; // servo angle required to level the touchscreen in the X axis
int Y_level_angle = 106;// servo angle reuired to level the touchscreen in the Y axis
double Xref = 79./1000.; // X reference coordinate
double Yref = 45./1000.; // Y reference coordinate
float x_distance=81./1000.; // pivot to end distance in the X axis
float y_distance= 45./1000.; // pivot to end distance in the Y axis
double HornLen = 30./1000.; // Servo horn length (screw to screw)
double Xupperbound = 6; // touchscreen X axis angle upper limit
double Xlowerbound = -6; // touchscreen X axis angle lower limit
double Yupperbound = 6; // touchscreen Y axis angle upper limit
double Ylowerbound = -8; // touchscreen Y axis angle lower limit

// X axis PID gains 
double X_Kp = 50;
float X_Ki = 0.01;
int X_Kd = 21000;

// Y axis PID gains
double Y_Kp = 60;
float Y_Ki = 0.001;
int Y_Kd = 28000;

// functions to read the positions
double ReadXco();
double ReadYco();

void setup()
{
    Xerror_prev = 0 ;
    Yerror_prev = 0;
    Time_prev = 0 ;
    Servo1.attach(Servo1PIN);
    Servo2.attach(Servo2PIN);
   
    // Keep checking if the ball is on the touchscreen
    // only start the loop function when you can sense the ball
   do{
    X_coor_prev = ReadXco(); 
    Y_coor_prev = ReadYco(); 
   }while((X_coor_prev == 0) || (Y_coor_prev ==0)); 
}

void loop()
{
    // read the position
    X_coor = ReadXco();
    Y_coor = ReadYco();
    
    // calculate the ball's displacement
    X_disp =abs(X_coor - X_coor_prev);
    Y_disp =abs(Y_coor - Y_coor_prev);
   
    // check whether the ball is touching the touchscreen and whether the displacement is less than 5mm
    if(X_coor!=0 && Y_coor !=0 && X_disp <=0.005 && Y_disp <= 0.005){
   
    // calculating the current time and delta time
    Time_curr = millis();
    dt = Time_curr - Time_prev;
    Time_prev = Time_curr;
 
    // calculating the current error, delta error, and error total
    Xerror_curr = Xref - X_coor;
    Yerror_curr = Yref - Y_coor;
    Xde = Xerror_curr - Xerror_prev;
    Yde = Yerror_curr - Yerror_prev;
    Xerror_total = Xerror_curr + Xerror_prev;
    Yerror_total = Yerror_curr + Yerror_prev;
    Xerror_prev = Xerror_curr ;
    Yerror_prev = Yerror_curr;

    // X and Y axes discretized control signals
    Xtheta2 = (X_Kp*Xerror_curr) + (X_Ki * Xerror_total*dt) + (X_Kd * (Xde/dt));
    Ytheta2 = (Y_Kp*Yerror_curr) + (Y_Ki *Yerror_total * dt) + (Y_Kd * (Yde/dt));

    // Saturating the control signal 
    if(Xtheta2>=Xupperbound){

        Xtheta2 = Xupperbound;

    }
    else if(Xtheta2 <=Xlowerbound){

        Xtheta2 = Xlowerbound;
        
    }
    
     if(Ytheta2>=Yupperbound){

        Ytheta2 = Yupperbound;

    }
    else if(Ytheta2 <=Ylowerbound){

        Ytheta2 = Ylowerbound;
       
    }


    // Using the kinematic model to translate the touchscreen angles to servo positions
    Xtheta1 = X_level_angle + ((x_distance/HornLen)*Xtheta2);
    Ytheta1 = Y_level_angle + ((y_distance/HornLen)*Ytheta2);
    Servo1.write(Xtheta1);
    Servo2.write(Ytheta1);
    
    X_coor_prev = X_coor ;
    Y_coor_prev = Y_coor ;

    }
    else{
        // if the ball is not touching the touchscreen, or if ball's displacement is
        // greater than 5mm (indicating a false reading since the ball cannot travel
        // this distance in this very short period of time), then read the X and Y coordinates
        // again 
        X_coor_prev = ReadXco();
        Y_coor_prev = ReadYco();
    }
    
}


// This function obtains the X-coordinate from the touchscreen
double ReadXco()
{
    int xmeas;
    double Xdist;
    // Obtaining the X-axis touch point reading
    pinMode(Xp, OUTPUT);
    pinMode(Xn, OUTPUT);
    pinMode(Yp, INPUT);
    pinMode(Yn, INPUT);
    digitalWrite(Xp, HIGH);
    digitalWrite(Xn, LOW);
    digitalWrite(Yn, LOW);
    delay(5);
    xmeas = analogRead(Yp);

    // converting the readings to coordinate units in mm
    Xdist = (xmeas - 100) * (156. / 835.);

    // Saturating the X-coordinates to a min of 0 and a max of 156 mm
    if (Xdist < 0)
    {

        Xdist = 0;
    }
    else if (Xdist > 156)
    {

        Xdist = 156;
    }
    else
    {
    }

    Xdist = Xdist/1000.; //Changing the units to m

    return (Xdist);
}

// This function obtains the Y-coordinate from the touchscreen
double ReadYco()
{
    int ymeas;
    double Ydist;
    // Obtaining the Y-axis touch point reading
    pinMode(Yp, OUTPUT);
    pinMode(Yn, OUTPUT);
    pinMode(Xp, INPUT);
    pinMode(Xn, INPUT);
    digitalWrite(Yp, HIGH);
    digitalWrite(Yn, LOW);
    digitalWrite(Xn, LOW);
    delay(5);
    ymeas = analogRead(Xp);

    // converting the readings to coordinate units in mm
    Ydist = (ymeas - 70) * (95. / 1000.);

    // Saturating the Y-coordinates to a min of 0 and a max of 95 mm
    if (Ydist < 0)
    {

        Ydist = 0;
    }
    else if (Ydist > 95)
    {

        Ydist = 95;
    }

    else
    {
    }

     Ydist = Ydist/1000.; // changing the units to m

    return (Ydist);
}
