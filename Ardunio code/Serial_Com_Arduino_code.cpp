/***************************************HEADER COMMENT***************************************

   Program: Serial_Com_Arduino_code.cpp
   Author:  Kareem Badreldin
   Created on: 20th of July, 2023
   Last modified: 4th of August,2023
   Programming language: C++
   Purpose:  This program allows the Arduino Uno to communicate with MATLAB over the serial
             port. The program reads the ball's X coordinate, Y coordinate, and dt. It then
             sends this data to MATLAB, which plots the ball's real-time X and Y coordinates.

********************************************************************************************/

#include <Arduino.h>

#define Xp A0
#define Xn A1
#define Yp A2
#define Yn A3

double X_coor, Y_coor; // Coordinate variables

// Time variables
unsigned long Time_prev, Time_curr;
double dt;

// Functions to read the X and Y coordinates
double ReadXco();
double ReadYco();

void setup()
{
    Serial.begin(9600);
    Time_prev = 0;
}

void loop()
{
    // Reading the X and Y coordinates
    X_coor = ReadXco();
    Y_coor = ReadYco();

    Time_curr = millis();
    dt = Time_curr - Time_prev;
    Time_prev = Time_curr;

    dt = dt / 1000.; // Changing the units to seconds

    // Printing the X coordinate, Y coordinate, and dt to the serial monitor
    // The terminator here is the new line just like in the MATLAB code so that MATLAB can identify
    // when to start reading a new data set
    Serial.print(X_coor, 3);
    Serial.print(" ");
    Serial.print(Y_coor, 3);
    Serial.print(" ");
    Serial.println(dt);

    delay(100);
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

    return (Ydist);
}