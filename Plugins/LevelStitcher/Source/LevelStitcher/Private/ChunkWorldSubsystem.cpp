// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkWorldSubsystem.h"

void UChunkWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UChunkWorldSubsystem::Deinitialize()
{

}

void UChunkWorldSubsystem::NotifySublevelLoaded(FString SublevelInstanceName, int SublevelIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("NOTIFY LOADED %i %s"), SublevelIndex, *SublevelInstanceName);
	OnSublevelLoaded.Broadcast(SublevelInstanceName, SublevelIndex);
}

void UChunkWorldSubsystem::NotifySublevelUnloaded(FString SublevelInstanceName, int SublevelIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("NOTIFY UNLOADED %i %s"), SublevelIndex, *SublevelInstanceName);
	OnSublevelUnloaded.Broadcast(SublevelInstanceName, SublevelIndex);
}
