// Fill out your copyright notice in the Description page of Project Settings.

#include "KitesurfingSimulator.h"
#include "WiimoteBlueprintLibrary.h"

wiimote UWiimoteBlueprintLibrary::_remote;
bool UWiimoteBlueprintLibrary::_bIsConnected = false;

UWiimoteBlueprintLibrary::~UWiimoteBlueprintLibrary()
{
	Disconnect();
}

bool UWiimoteBlueprintLibrary::Connect()
{
	if (_bIsConnected)
	{
		UE_LOG(Wiimote, Warning, TEXT("Wiimote is already connected"));
		return true;
	}

	_remote.ChangedCallback = on_state_change;
	_remote.CallbackTriggerFlags = (state_change_flags)(CONNECTED |
		EXTENSION_CHANGED |
		MOTIONPLUS_CHANGED);
	_bIsConnected = _remote.Connect(wiimote::FIRST_AVAILABLE);
	if (_bIsConnected)
	{
		_remote.SetLEDs(0x01);
		UE_LOG(Wiimote, Log, TEXT("Wiimote connected"));
	}
	else
	{
		UE_LOG(Wiimote, Error, TEXT("Couldn't connect wiimote"));
	}

	return _bIsConnected;
}

void UWiimoteBlueprintLibrary::Disconnect()
{
	if (!_bIsConnected)
	{
		return;
	}

	UE_LOG(Wiimote, Log, TEXT("Disconnecting Wiimote"));
	_remote.SetLEDs(0x00);
	_remote.Disconnect();
	_bIsConnected = false;
}

FVector UWiimoteBlueprintLibrary::GetAcceleration()
{
	if (!_bIsConnected)
	{
		return FVector::ZeroVector;
	}

	_remote.RefreshState();
	return FVector(_remote.Acceleration.X, _remote.Acceleration.Y, _remote.Acceleration.Z);
}

FVector UWiimoteBlueprintLibrary::GetOrientation()
{
	if (!_bIsConnected)
	{
		return FVector::ZeroVector;
	}

	_remote.RefreshState();
	return FVector(_remote.Acceleration.Orientation.Pitch, 0.0f, _remote.Acceleration.Orientation.Roll);
}

FRotator UWiimoteBlueprintLibrary::GetMotionPlusRaw()
{
	if (!_bIsConnected)
	{
		return FRotator::ZeroRotator;
	}

	_remote.RefreshState();
	return FRotator(_remote.MotionPlus.Raw.Pitch, _remote.MotionPlus.Raw.Yaw, _remote.MotionPlus.Raw.Roll);
}

FRotator UWiimoteBlueprintLibrary::GetMotionPlusSpeed()
{
	if (!_bIsConnected)
	{
		return FRotator::ZeroRotator;
	}

	_remote.RefreshState();
	FRotator speed = FRotator(_remote.MotionPlus.Speed.Pitch, _remote.MotionPlus.Speed.Yaw, _remote.MotionPlus.Speed.Roll);
	/*if (FMath::Abs(speed.Pitch) < 10.0f)
	{
		speed.Pitch = 0.0f;
	}
	if (FMath::Abs(speed.Roll) < 10.0f)
	{
		speed.Roll = 0.0f;
	}
	if (FMath::Abs(speed.Yaw) < 10.0f)
	{
		speed.Yaw = 0.0f;
	}*/

	return speed;
}

// Callback
void on_state_change(wiimote& remote,
	state_change_flags changed,
	const wiimote_state &new_state)
{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).

	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if (changed & CONNECTED)
	{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.

		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if (new_state.ExtensionType != wiimote::BALANCE_BOARD)
		{
			if (new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
		}
	}
	// a MotionPlus was detected
	if (changed & MOTIONPLUS_DETECTED)
	{
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if (remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
		}
	}
	// an extension is connected to the MotionPlus
	else if (changed & MOTIONPLUS_EXTENSION_CONNECTED)
	{
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if (remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
	}
	// an extension disconnected from the MotionPlus
	else if (changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
	{
		// enable the MotionPlus data again:
		if (remote.MotionPlusConnected())
			remote.EnableMotionPlus();
	}
	// another extension was just connected:
	else if (changed & EXTENSION_CONNECTED)
	{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(1000, 200);
#endif
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if (!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
	}
	// extension was just disconnected:
	else if (changed & EXTENSION_DISCONNECTED)
	{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(200, 300);
#endif
		// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
	}
}