// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "EDetectorLoadBehaviour.h"
#include "ChunkSphereDetector.generated.h"

/**
 * 
 */
UCLASS()
class LEVELSTITCHER_API UChunkSphereDetector : public USphereComponent
{
	GENERATED_BODY()
public:
	UChunkSphereDetector();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	TEnumAsByte<EDetectorLoadBehaviour> LoadBehaviour;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Behaviour")
	EDetectorLoadBehaviour GetLoadBehaviour() const { return LoadBehaviour; }
};
