// Fill out your copyright notice in the Description page of Project Settings.


#include "../Subsystems/LevelStitchingWorldSubsystem.h"

void ULevelStitchingWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize"));
}

void ULevelStitchingWorldSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize"));
}
