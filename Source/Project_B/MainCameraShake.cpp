// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCameraShake.h"

UMainCameraShake::UMainCameraShake()
{
	OscillationDuration = 0.5f;
	LocOscillation.Y.Amplitude = 1.f;
	LocOscillation.Y.Frequency = 100.f;
	LocOscillation.Z.Amplitude = 1.f;
	LocOscillation.Z.Frequency = 100.f;

	bSingleInstance = true;
}
