// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkBoundCollider.generated.h"

UCLASS()
class LEVELSTITCHER_API AChunkBoundCollider : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkBoundCollider();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* CollisionComp;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Size")
	void SetBoundExtent(FVector InExtent);

	UFUNCTION(BlueprintCallable, Category = "Collision")
	void SetCollisionChannel(ECollisionChannel CollisionChannel);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
