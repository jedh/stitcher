// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkManager.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "LevelTriggerBox.h"
#include "ChunkContainerDataAsset.h"
#include "ChunkFloorTile.h"
#include "Components/BoxComponent.h"
#include "ChunkBoundCollider.h"
//#include "Kismet/GameplayStatics.h"

// Sets default values
AChunkManager::AChunkManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TopSphereDetector = CreateDefaultSubobject<USphereComponent>(TEXT("TopSphereDetector"));
	TopSphereDetector->SetupAttachment(RootComponent);

	BottomSphereDetector = CreateDefaultSubobject<USphereComponent>(TEXT("BottomSphereDetector"));
	BottomSphereDetector->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AChunkManager::BeginPlay()
{
	Super::BeginPlay();
		
	//UGameplayStatics::GetPlayerController(this, 0);
	PlayerRef = GEngine->GetFirstLocalPlayerController(GetWorld());

	if (PlayerRef != nullptr)
	{
		LoadLevelTriggerVolumes(LevelChunkContainerDataAsset);
		SetDetectorProjectionDistances(PlayerRef);
		SetDetectorLocations(PlayerRef);

		// Check if the player is currently overlapping with a LevelTriggerBox.

		PrevPlayerLocation = PlayerRef->GetPawn()->GetActorLocation();
	}	
}

// Called every frame
void AChunkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if the player's location has changed since the previous frame before updating.
}

void AChunkManager::LoadLevelTriggerVolumes(const UChunkContainerDataAsset* ChunkContainerDataAsset)
{
	UWorld* const World = GetWorld();
	float SpawnOffsetZ = 16.0f;

	for (int i = 0; i < ChunkContainerDataAsset->SublevelMaps.Num(); i++)
	{		
		float SpawnPositionX = ChunkContainerDataAsset->SublevelExtents.X * 2 * i;
		FVector SpawnLocation = FVector(SpawnPositionX, 0.0f, -SpawnOffsetZ);

		// Spawn chunk floor tiles.
		AChunkFloorTile* FloorTile = World->SpawnActor<AChunkFloorTile>(SpawnLocation, FRotator::ZeroRotator);

		FVector FloorExtents = ChunkContainerDataAsset->SublevelExtents;
		FloorExtents.Z = SpawnOffsetZ;
		FloorTile->SetFloorExtent(FloorExtents);

		// Spawn the chunk trigger box. 
		SpawnLocation.Z = 0.0f;
		ALevelTriggerBox* ChunkTriggerBox = World->SpawnActor<ALevelTriggerBox>(SpawnLocation, FRotator::ZeroRotator);
		ChunkTriggerBox->Sublevel = ChunkContainerDataAsset->SublevelMaps[i];
		ChunkTriggerBox->SublevelIndex = i;
		UBoxComponent* CollisionComponent = Cast<UBoxComponent>(ChunkTriggerBox->GetCollisionComponent());
		if (CollisionComponent != nullptr)
		{
			CollisionComponent->SetBoxExtent(ChunkContainerDataAsset->SublevelExtents);
		}

		FVector SideColliderExtent = ChunkContainerDataAsset->SublevelExtents;
		SideColliderExtent.Y = WallBoundWidth;

		// Spawn left level bounds collider.
		float LeftColliderPositionY = -(WallBoundWidth + ChunkContainerDataAsset->SublevelExtents.Y);
		FVector LeftColliderPosition = FVector(SpawnPositionX, LeftColliderPositionY, 0.0f);
		AChunkBoundCollider* LeftCollider = World->SpawnActor<AChunkBoundCollider>(LeftColliderPosition, FRotator::ZeroRotator);
		LeftCollider->SetBoundExtent(SideColliderExtent);

		// Spawn right level bounds collider.
		float RightColliderPositionY = WallBoundWidth + ChunkContainerDataAsset->SublevelExtents.Y;
		FVector RightColliderPosition = FVector(SpawnPositionX, RightColliderPositionY, 0.0f);
		AChunkBoundCollider* RightCollider = World->SpawnActor<AChunkBoundCollider>(RightColliderPosition, FRotator::ZeroRotator);
		RightCollider->SetBoundExtent(SideColliderExtent);
	}

	FVector EndColliderExtent = ChunkContainerDataAsset->SublevelExtents;
	EndColliderExtent.X = WallBoundWidth;
	
	// Spawn bottom collider for the level.
	float BottomColliderPositionX = -ChunkContainerDataAsset->SublevelExtents.X;
	FVector BottomColliderPosition = FVector(BottomColliderPositionX, 0.0f, 0.0f);
	AChunkBoundCollider* BottomCollider = World->SpawnActor<AChunkBoundCollider>(BottomColliderPosition, FRotator::ZeroRotator);
	BottomCollider->SetBoundExtent(EndColliderExtent);

	// Spawn top collider for the level.
	float TopColliderPositionX = (ChunkContainerDataAsset->SublevelExtents.X * 2.0f) * ChunkContainerDataAsset->SublevelMaps.Num();
	TopColliderPositionX = TopColliderPositionX - ChunkContainerDataAsset->SublevelExtents.X;
	FVector TopColliderPosition = FVector(TopColliderPositionX, 0.0f, 0.0f);
	AChunkBoundCollider* TopCollider = World->SpawnActor<AChunkBoundCollider>(TopColliderPosition, FRotator::ZeroRotator);
	TopCollider->SetBoundExtent(EndColliderExtent);
}

void AChunkManager::SetDetectorProjectionDistances(const APlayerController* Player)
{

}

void AChunkManager::SetDetectorLocations(const APlayerController* Player)
{

}

void AChunkManager::TrySpawnSublevel(const ALevelTriggerBox* LevelTrigger)
{

}

