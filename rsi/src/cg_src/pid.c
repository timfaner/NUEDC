#include "stdio.h"
#include "stdlib.h"
#include "pid.h"
#include "r_cg_macrodriver.h"
#include "r_cg_sci.h"
#include "r_cg_cmt.h"

struct PID myPid;

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up 
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
void PID(double* Input, double* Output, double* Setpoint,
        double Kp, double Ki, double Kd, int ControllerDirection)
{
	
    myPid.myOutput = Output;
    myPid.myInput = Input;
    myPid.mySetpoint = Setpoint;
	myPid.inAuto = false;
	
	SetOutputLimits(-255, 255);				//default output limit corresponds to
												//the arduino pwm limits

    myPid.SampleTime = 100;							//default Controller Sample Time is 0.1 seconds

    SetControllerDirection(ControllerDirection);
    SetTunings(Kp, Ki, Kd);

    myPid.lastTime = millis()-myPid.SampleTime;
	
}

/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   myPid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/ 
int Compute(double * Input)
{
	unsigned long now,timeChange;
   if(!myPid.inAuto) return false;
   myPid.myInput = Input;
   now = millis();
   timeChange = (now - myPid.lastTime);
   if(timeChange>=myPid.SampleTime)
   {
      /*Compute all the working error variables*/
	  double input, error, dInput, output;
	  input = *myPid.myInput;
      error = *myPid.mySetpoint - input;
      myPid.ITerm+= (myPid.ki * error);
      if(myPid.ITerm > myPid.outMax) myPid.ITerm= myPid.outMax;
      else if(myPid.ITerm < myPid.outMin) myPid.ITerm= myPid.outMin;
      dInput = (input - myPid.lastInput);
 
      /*Compute PID Output*/
      output = myPid.kp * error + myPid.ITerm- myPid.kd * dInput;
      
	  if(output > myPid.outMax) output = myPid.outMax;
      else if(output < myPid.outMin) output = myPid.outMin;
	  output = output *0.7/255;
	  *myPid.myOutput = output;
	  
      /*Remember some variables for next time*/
      myPid.lastInput = input;
      myPid.lastTime = now;
	  return true;
   }
   else return false;
}


int Comput(void)
{
	union{
	signed int all;
	unsigned char s[2];
	}data;
	char i, sci_send = 10;
	double input, error, dInput, output;
	unsigned long now, timeChange;
    if(!myPid.inAuto) return false;
    now = millis();
    timeChange = (now - myPid.lastTime);
    if(timeChange>=myPid.SampleTime)
    {
      /*Compute all the working error variables*/
//		SCI5_Serial_Send(&sci_send, 1);
    while(sci5_receive_available() > 0)
    {
    SCI5_Serial_Receive(&data.s[i], 1);
    i++;
    }
   	  i=0;
	  input = (double)(data.all);
      error = *myPid.mySetpoint - input;
      myPid.ITerm+= (myPid.ki * error);
      if(myPid.ITerm > myPid.outMax) myPid.ITerm= myPid.outMax;
      else if(myPid.ITerm < myPid.outMin)myPid.ITerm= myPid.outMin;
      dInput = (input - myPid.lastInput);
 
      /*Compute PID Output*/
      output = myPid.kp * error + myPid.ITerm- myPid.kd * dInput;
      
	  if(output > myPid.outMax) output = myPid.outMax;
      else if(output < myPid.outMin) output = myPid.outMin;
	  *myPid.myOutput = output;
	  
      /*Remember some variables for next time*/
      myPid.lastInput = input;
      myPid.lastTime = now;
	//  Serial.print(*myPid.myOutput);
	//  Serial.print(",");
	//  Serial.println(*myInput);
	  return true;
   }
   else return false;
}


/* SetTunings(...)*************************************************************
 * This function allows the controller's dynamic performance to be adjusted. 
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 ******************************************************************************/ 
void SetTunings(double Kp, double Ki, double Kd)
{
	double SampleTimeInSec;
   if (Kp<0 || Ki<0 || Kd<0) return;
 
   myPid.dispKp = Kp; myPid.dispKi = Ki; myPid.dispKd = Kd;
   
   SampleTimeInSec = ((double)myPid.SampleTime)/1000;
   myPid.kp = Kp;
   myPid.ki = Ki * SampleTimeInSec;
   myPid.kd = Kd / SampleTimeInSec;
 
  if(myPid.controllerDirection ==REVERSE)
   {
      myPid.kp = (0 - myPid.kp);
      myPid.ki = (0 - myPid.ki);
      myPid.kd = (0 - myPid.kd);
   }
}
  
/* SetSampleTime(...) *********************************************************
 * sets the period, in Milliseconds, at which the calculation is performed	
 ******************************************************************************/
void SetSampleTime(int NewSampleTime)
{
   if (NewSampleTime > 0)
   {
      double ratio  = (double)NewSampleTime
                      / (double)myPid.SampleTime;
      myPid.ki *= ratio;
      myPid.kd /= ratio;
      myPid.SampleTime = (unsigned long)NewSampleTime;
   }
}
 
/* SetOutputLimits(...)****************************************************
 *     This function will be used far more often than SetInputLimits.  while
 *  the input to the controller will generally be in the 0-1023 range (which is
 *  the default already,)  the output will be a little different.  maybe they'll
 *  be doing a time window and will need 0-8000 or something.  or maybe they'll
 *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 *  here.
 **************************************************************************/
void SetOutputLimits(double Min, double Max)
{
   if(Min >= Max) return;
   myPid.outMin = Min;
   myPid.outMax = Max;
 
   if(myPid.inAuto)
   {
	   if(*myPid.myOutput > myPid.outMax) *myPid.myOutput = myPid.outMax;
	   else if(*myPid.myOutput < myPid.outMin) *myPid.myOutput = myPid.outMin;
	 
	   if(myPid.ITerm > myPid.outMax) myPid.ITerm= myPid.outMax;
	   else if(myPid.ITerm < myPid.outMin) myPid.ITerm= myPid.outMin;
   }
}

/* SetMode(...)****************************************************************
 * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 ******************************************************************************/ 
void SetMode(int Mode)
{
    int newAuto = (Mode == AUTOMATIC);
    if(newAuto == !myPid.inAuto)
    {  /*we just went from manual to auto*/
        Initialize();
    }
    myPid.inAuto = newAuto;
}
 
/* Initialize()****************************************************************
 *	does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/ 
void Initialize(void)
{
   myPid.ITerm = *myPid.myOutput;
   myPid.lastInput = *myPid.myInput;
   if(myPid.ITerm > myPid.outMax) myPid.ITerm = myPid.outMax;
   else if(myPid.ITerm < myPid.outMin) myPid.ITerm = myPid.outMin;
}

/* SetControllerDirection(...)*************************************************
 * The PID will either be connected to a DIRECT acting process (+Output leads
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 ******************************************************************************/
void SetControllerDirection(int Direction)
{
   if(myPid.inAuto && Direction !=myPid.controllerDirection)
   {
	  myPid.kp = (0 - myPid.kp);
      myPid.ki = (0 - myPid.ki);
      myPid.kd = (0 - myPid.kd);
   }   
   myPid.controllerDirection = Direction;
}

/* Status Funcions*************************************************************
 * Just because you set the Kp=-1 doesn't mean it actually happened.  these
 * functions query the internal state of the PID.  they're here for display
 * purposes.  this are the functions the PID Front-end uses for example
 ******************************************************************************/
