// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkBoundCollider.h"
#include "Components/BoxComponent.h"

// Sets default values
AChunkBoundCollider::AChunkBoundCollider()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));

	// TODO: Set this via a parameter in case a game wants to override this.
	//CollisionComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	RootComponent = CollisionComp;
}

// Called when the game starts or when spawned
void AChunkBoundCollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChunkBoundCollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

void AChunkBoundCollider::SetBoundExtent(FVector InExtent)
{
	CollisionComp->SetBoxExtent(InExtent);
}

void AChunkBoundCollider::SetCollisionChannel(ECollisionChannel CollisionChannel)
{	
	CollisionComp->SetCollisionObjectType(CollisionChannel);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
}

