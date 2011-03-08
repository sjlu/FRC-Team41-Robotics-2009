#include "WPILib.h"

class DefaultRobot : public IterativeRobot
{
	RobotDrive *robotDrive;
									// DRIVE TRAIN MOTORS
									// Ports: 4DSP01, 4DSP02, 6DSP01, 6DSP02
									// Target: J1, J2, J3, J4
									// Index: 16, 17, 18, 19
	
									// ROTARY ENCODERS
									// Ports: 4DSG03-06, 6DSG03-06
									// Target: J1EA/B, J3EA/B, J2EA/B, J4EA/B
									// Index: 38-45
	
									// CURRENT SENSORS
									// Ports: 1A01-04
									// Target: J1-J4
									// Index: 23-26
	
	Joystick* leftStick;			// joystick 1 (one-stick tank drive or two stick tank drive)
	Joystick* rightStick;			// joystick 2 (second stick tank drive)
									// Ports: DSUSB01, DSUSB02
	

	SpeedController* mLF;
	SpeedController* mRF;
	SpeedController* mLR;
	SpeedController* mRR;
	
    void dashboard_data_send ()
    {
    }

public:

	void RobotInit(void)
	{
		/*
		 * Device Initializations
		 */
		
		leftStick = new Joystick(1);				// initialize the the joysticks
		rightStick = new Joystick(2);
		
		mLF = new Jaguar (4,1);						// establishes the Jaguars
		mLR = new Jaguar (4,2);
		mRF = new Jaguar (6,1);
		mRR = new Jaguar (6,2);
		
		robotDrive = new RobotDrive(mLF, mLR, mRF, mRR);	// Initiallizing drivetrain
		
		GetWatchdog().SetExpiration(1);
	}

	void AutonomousInit(void)
	{

	}
	
	void AutonomousPeriodic(void)
	{	
	
	}
	
	void TeleopInit(void) 
	{

	}
	
	void TeleopPeriodic(void)
	{
		GetWatchdog().Feed(); // you really need this feed or things won't go well.
		
		// the iterative robot sampling rate is default at 0.005 seconds
		
		/*
		 * Drive train adjustment buttons
		 */
		
		/*
		 *float eLF, eLR, eRF, eRR;
		 *eLF = DriveTrain->GetEncoderValue(1);
		 *eLR = DriveTrain->GetEncoderValue(2);
		 *eRF = DriveTrain->GetEncoderValue(3);
		 *eRR = DriveTrain->GetEncoderValue(4);
		 */
		
		
		/*
		 * Sending values of joysticks to the Jaguars
		 */
	
		robotDrive->TankDrive(-(leftStick->GetY()), -(rightStick->GetY())); //Dont ask why its negative, anti-inverted

    }
};

START_ROBOT_CLASS(DefaultRobot);


