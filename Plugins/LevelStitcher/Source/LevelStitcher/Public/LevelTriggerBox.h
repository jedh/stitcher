// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "LevelTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class LEVELSTITCHER_API ALevelTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	TSoftObjectPtr<UWorld> Sublevel;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Levels")
	FString SublevelInstanceName;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Levels")
	int SublevelIndex;

protected:	
	// We don't currently use this, as we load by soft references currently,
	// although we may want to leverage this if we start loading too much up front.
	FName SublevelStreamName;	
};
