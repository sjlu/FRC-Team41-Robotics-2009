/********************************************************************************
*  Project   		: FIRST Motor Controller
*  File Name  		: SimpleTracker.cpp        
*  Contributors 	: ELF
*  Creation Date 	: Oct 17, 2008
*  Revision History	: Source code & revision history maintained at sourceforge.WPI.edu   
*  File Description	: Demo program showing simple color tracking
*/
/*----------------------------------------------------------------------------*/
/*        Copyright (c) FIRST 2008.  All Rights Reserved.                     */
/*  Open Source Software - may be modified and shared by FRC teams. The code  */
/*  must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib. */
/*----------------------------------------------------------------------------*/

#include <iostream.h>
#include "math.h"
#include <taskLib.h>

#include "AxisCamera.h" 
#include "BaeUtilities.h"
#include "FrcError.h"
#include "TrackAPI.h" 
#include "WPILib.h"

#define MIN_PARTICLE_TO_IMAGE_PERCENT 0.25		// target is too small
#define MAX_PARTICLE_TO_IMAGE_PERCENT 10.0		// target is too close
/**
 * This is a demo program showing the use of the color tracking API. 
 * It uses the SimpleRobot class as a base of a robot application that will automatically call 
 * your Autonomous and OperatorControl methods at the right time as controlled by the switches
 * on the driver station or the field controls. Autonomous mode tracks color, assuming a 
 * camera on a fixed mount. 
 */
class SimpleTracker : public SimpleRobot {
	RobotDrive *myRobot; 		// robot drive system
	Joystick *rightStick; 		// joystick 1 (arcade stick or right tank stick)
	Joystick *leftStick; 		// joystick 2 (tank left stick)
	DriverStation *ds; 			// driver station object
	TrackingThreshold tdata; 	// image data for tracking
	Image * img;
	double ts;

	enum // Driver Station jumpers to control program operation
	{	ARCADE_MODE = 1, 		// Tank/Arcade jumper is on DS Input 1 (Jumper present is arcade)
		ENABLE_AUTONOMOUS = 2, 	// Autonomous/Teleop jumper is on DS Input 2 (Jumper present is autonomous)
	} jumpers;

public:
	/**
	 * Constructor for this robot subclass.
	 * Create an instance of a RobotDrive with left and right motors plugged into PWM
	 * ports 1 and 2 on the first digital module.
	 */
	SimpleTracker(void) {
		ds = DriverStation::GetInstance();		
		myRobot = new RobotDrive(1, 2, 0.5);	// robot will use PWM 1-2 for drive motors		
		rightStick = new Joystick(1); 			// create the joysticks
		leftStick = new Joystick(2);

		SetDebugFlag(DEBUG_SCREEN_ONLY);		// dprintf output goes to terminal/console
		
		/* start the CameraTask	 */
		if (StartCameraTask(10, 0, k160x120, ROT_0) == -1) {
			dprintf( LOG_ERROR,"Failed to spawn camera task; Error code %s", 
					GetVisionErrorText(GetLastVisionError()) );
		}

		// values for tracking a target - may need tweaking in your environment
		// see TrackAPI.cpp for threshold values
		tdata = GetTrackingData(GREEN, FLUORESCENT);
		tdata.luminance.maxValue = 255;
		tdata.luminance.minValue = 0;
		tdata.hue.minValue = 0;
		tdata.hue.maxValue = 255;
		tdata.saturation.minValue = 0;
		tdata.saturation.maxValue = 255;
		
		/* stop the watchdog if debugging  */
		//GetWatchdog().SetEnabled(false);
		GetWatchdog().SetExpiration(200);
	}

	void Autonomous(void) {
		GetWatchdog().Feed();
		
		ParticleAnalysisReport par; // particle analysis reports

		while (IsAutonomous()) {

			/* this simple test will drive toward GREEN 
			 * drive will last until autonomous terminates
			 */	
			// delay in loop. For frame rate of 10 fps, a new image is available every 100 ms.
			// put the Wait at the beginning vs end of loop because the first time through
			// the camera is still initializing.
			Wait(.05); 
			GetWatchdog().Feed();
			if (FindColor(IMAQ_HSL, &tdata.hue, &tdata.saturation, &tdata.luminance, &par)
					&& par.particleToImagePercent < MAX_PARTICLE_TO_IMAGE_PERCENT
					&& par.particleToImagePercent > MIN_PARTICLE_TO_IMAGE_PERCENT) {
				// drive toward color
				//myRobot->Drive((float)1.0, (float)par.center_mass_x_normalized);
				//ShowActivity("%s     found: follow x value: %f", tdata.name, par.center_mass_x_normalized);
			} else {
				//myRobot->Drive(0.0, 0.0); // stop robot
				//ShowActivity("%s NOT found. particleToImagePercent: %f",
				//				tdata.name, par.particleToImagePercent);
			}
			
			img = frcCreateImage (IMAQ_IMAGE_RGB);
			
			char buffer[256];
			
			if (GetImage (img, &ts)) {
		      ShowActivity ("Image get successful.  Timestamp: %d", (int)ts);
		      snprintf (buffer, 255, "/tmp/IMG%d", (int)ts);
		      frcWriteImage (img, buffer);
			}
			else
		      ShowActivity ("Image get failed: %s", imaqGetErrorText (imaqGetLastError()));			
			
			
		} // end while

		myRobot->Drive(0.0, 0.0); // stop robot
	} 

	/**
	 * Runs the motors under driver control with either tank or arcade steering selected
	 * by a jumper in DS Digin 0. 
	 */
	void OperatorControl(void) {
		while ( IsOperatorControl() )
		{
			GetWatchdog().Feed();
			myRobot->ArcadeDrive(rightStick); // drive with arcade style (use right stick)
		}
	} 
};

// entry point is FRC_UserProgram_StartupLibraryInit
START_ROBOT_CLASS(SimpleTracker)
;

