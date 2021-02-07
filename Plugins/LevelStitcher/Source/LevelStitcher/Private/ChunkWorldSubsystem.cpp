// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkWorldSubsystem.h"
#include "Engine/Level.h"

void UChunkWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UChunkWorldSubsystem::Deinitialize()
{

}

void UChunkWorldSubsystem::NotifySublevelLoaded(ULevel* Sublevel, FString SublevelInstanceName, int SublevelIndex)
{	
	TArray<AActor*> SublevelActors = Sublevel->Actors;
	OnSublevelLoaded.Broadcast(SublevelInstanceName, SublevelIndex, SublevelActors);
}

void UChunkWorldSubsystem::NotifySublevelUnloaded(FString SublevelInstanceName, int SublevelIndex)
{	
	OnSublevelUnloaded.Broadcast(SublevelInstanceName, SublevelIndex);
}
