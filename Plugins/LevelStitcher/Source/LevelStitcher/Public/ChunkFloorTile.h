// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkFloorTile.generated.h"

UCLASS()
class LEVELSTITCHER_API AChunkFloorTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkFloorTile();	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* CollisionComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Size")
	void SetFloorExtent(FVector InExtent);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
