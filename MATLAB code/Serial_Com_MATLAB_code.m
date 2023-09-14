%{
***************************************HEADER COMMENT****************************************
   
   Program:  Serial_Com_MATLAB_code.m
   Author:  Kareem Badreldin
   Created on: 23th of July, 2023
   Last modified: 6th of August, 2023
   Programming language: MATLAB
   Purpose:  This program allows MATLAB to communicate with the Arduino UNO over the serial
             port. MATLAB receives the position data from the arduino and then uses it to 
             plot the ball's real-time X and Y coordinates. This program can help with PID
             tuning, performance analysis, and troubleshooting.

******************************************************************************************** 
%}


arduino = serialport("COM3",9600); % creates an object to communicate with the arduino using COM3 at a baud rate of 9600

% creates a figure, an empty plot with red line for X and blue line for Y, and a plot handle
figure
LivePlot = plot(0,0, 'r-',0,0, 'b-');
xlabel 'Time (s)';
ylabel 'Distance (mm)';
title 'Ball Real-time Position' ;
legend ( 'X','Y' );
grid on;
grid minor;

% creating a loop that is exited when the matlab figure is closed
while ishandle(LivePlot)
    if (arduino.BytesAvailable > 0) % check for data in the serial port
        arduino_input = readline(arduino); % read everthing until the terminator (new line)
        str_arduino_input = num2str(arduino_input); % converting the numerical input to a string
        X_Y_dt = split(str_arduino_input); % splitting the string into three cells with each carrying the X coordinate, Y coordinate, and dt, respectively

        % changing to doubles and assigning them to new variables
        X_coor = str2double(X_Y_dt(1));
        Y_coor = str2double(X_Y_dt(2));
        dt = str2double(X_Y_dt(3));

        % updating the data arrays for the red line (X_angle)
        x1Data = get(LivePlot(1), 'XData');
        y1Data = get(LivePlot(1), 'YData');
        x1Data = [x1Data,numel(x1Data)*dt]; % incrementing the x-coordinates with delta time to make sure that the MATLAB plot is synchronous with the arduino data
        y1Data = [y1Data,X_coor];

        % updating the data arrays for the blue line (Y_angle)
        x2Data = get(LivePlot(2), 'XData');
        y2Data = get(LivePlot(2), 'YData');
        x2Data = [x2Data,numel(x2Data)*dt]; % incrementing the x-coordinates with delta time to make sure that the MATLAB plot is synchronous with the arduino data
        y2Data = [y2Data,Y_coor];

        % updating the plot
        set(LivePlot(1),'XData',x1Data,'YData',y1Data);
        set(LivePlot(2),'XData',x2Data,'YData',y2Data);


        drawnow % applying the plot updates immediately
    end
end


clear