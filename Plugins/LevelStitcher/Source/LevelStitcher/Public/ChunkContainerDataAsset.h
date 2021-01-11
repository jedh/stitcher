// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ChunkContainerDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LEVELSTITCHER_API UChunkContainerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Stats")
	FVector SublevelExtents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sublevels")
	TArray<TSoftObjectPtr<UWorld>> SublevelMaps;
};
