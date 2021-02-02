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
	OnSublevelLoaded.Broadcast(SublevelInstanceName, SublevelIndex);
}

void UChunkWorldSubsystem::NotifySublevelUnloaded(FString SublevelInstanceName, int SublevelIndex)
{
	OnSublevelUnloaded.Broadcast(SublevelInstanceName, SublevelIndex);
}
