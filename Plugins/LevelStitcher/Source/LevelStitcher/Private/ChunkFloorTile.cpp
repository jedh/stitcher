// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkFloorTile.h"
#include "Components/BoxComponent.h"

// Sets default values
AChunkFloorTile::AChunkFloorTile()
{
	//Set this actor to call Tick() every frame.You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));	
	RootComponent = CollisionComp;
}

void AChunkFloorTile::SetFloorExtent(FVector InExtent)
{
	CollisionComp->SetBoxExtent(InExtent);
}

void AChunkFloorTile::SetFloorCollisionChannel(ECollisionChannel CollisionChannel)
{
	CollisionComp->SetCollisionObjectType(CollisionChannel);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
}

