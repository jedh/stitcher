// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkManager.generated.h"

UCLASS()
class LEVELSTITCHER_API AChunkManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkManager();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	class UChunkContainerDataAsset* LevelChunkContainerDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions")
	FVector InitialLevelOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions")
	float WallBoundWidth;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TopSphereDetector;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* BottomSphereDetector;

	float ProjectionDistanceTop;

	float ProjectionDistanceBottom;	

	class APlayerController* PlayerRef;

	FVector PrevCameraLocation;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void LoadLevelTriggerVolumes(const class UChunkContainerDataAsset* ChunkContainerDataAsset);

	void SetDetectorProjectionDistances(const class APlayerController* Player);

	void SetDetectorLocations(const class APlayerController* Player);

	void TrySpawnSublevel(const class ALevelTriggerBox* LevelTrigger);
};
