// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkFloorTile.h"
#include "Components/BoxComponent.h"

// Sets default values
AChunkFloorTile::AChunkFloorTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->BodyInstance.SetCollisionProfileName("Floor");
	RootComponent = CollisionComp;	
}

// Called when the game starts or when spawned	
void AChunkFloorTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChunkFloorTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunkFloorTile::SetFloorExtent(FVector InExtent)
{
	CollisionComp->SetBoxExtent(InExtent);
}

