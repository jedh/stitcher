// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ChunkWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInitializationCompleteSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSublevelLoadedSignature, FString, SublevelInstanceName, int, SublevelIndex, const TArray<AActor*>&, SublevelActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSublevelUnloadedSignature, FString, SublevelInstanceName, int, SublevelIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerEnterSublevelSignature, int, PlayerIndex, APawn*, PlayerPawn, FString, SublevelInstanceName, int, SublevelIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerExitSublevelSignature, int, PlayerIndex, APawn*, PlayerPawn, FString, SublevelInstanceName, int, SublevelIndex);

UCLASS()
class LEVELSTITCHER_API UChunkWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable)
	FInitializationCompleteSignature OnInitializationComplete;

	UPROPERTY(BlueprintAssignable)
	FSublevelLoadedSignature OnSublevelLoaded;

	UPROPERTY(BlueprintAssignable)
	FSublevelUnloadedSignature OnSublevelUnloaded;

	UPROPERTY(BlueprintAssignable)
	FPlayerEnterSublevelSignature OnPlayerEnterSublevel;

	UPROPERTY(BlueprintAssignable)
	FPlayerExitSublevelSignature OnPlayerExitSublevel;

	void NotifyInitializationComplete();
	
	void NotifySublevelLoaded(class ULevel* Sublevel, FString SublevelInstanceName, int SublevelIndex);

	void NotifySublevelUnloaded(FString SublevelInstanceName, int SublevelIndex);
};
