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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions")
	FVector FloorDetectionOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TEnumAsByte<ECollisionChannel> DetectorCollisionChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TEnumAsByte<ECollisionChannel> LevelTriggerCollisionChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TEnumAsByte<ECollisionChannel> FloorCollisionChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TEnumAsByte<ECollisionChannel> LevelBoundsCollisionChannel;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection", meta = (AllowPrivateAccess = "true"))
	class UChunkSphereDetector* TopSphereDetector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection", meta = (AllowPrivateAccess = "true"))
	class UChunkSphereDetector* BottomSphereDetector;

	class UChunkWorldSubsystem* ChunkSubsystem;

	float ProjectionDistanceTop;

	float ProjectionDistanceBottom;	

	class APlayerController* PlayerRef;

	FVector PrevCameraLocation;

	int LevelInstansiatedCounter = 0;

	TArray<class ULevelStreaming*> LoadingLevelsArray;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetProjectionDistanceTop() const { return ProjectionDistanceTop; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetProjectionDistanceBottom() const { return ProjectionDistanceBottom; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTopDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTopDetectorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnBottomDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBottomDetectorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void LoadLevelTriggerVolumes(const class UChunkContainerDataAsset* ChunkContainerDataAsset);

	void SetDetectorProjectionDistances(const class APlayerController* Player);

	void SetDetectorLocations(const class APlayerController* Player);

	void TrySpawnSublevel(class ALevelTriggerBox* LevelTrigger);
};
