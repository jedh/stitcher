// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkManager.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "LevelTriggerBox.h"
#include "ChunkContainerDataAsset.h"
#include "ChunkFloorTile.h"
#include "Components/BoxComponent.h"
#include "ChunkBoundCollider.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
//#include "Kismet/GameplayStatics.h"

#define COLLISION_FLOOR		ECC_GameTraceChannel1

// Sets default values
AChunkManager::AChunkManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TopSphereDetector = CreateDefaultSubobject<USphereComponent>(TEXT("TopSphereDetector"));
	//TopSphereDetector->BodyInstance.SetCollisionProfileName("DetectLevels");
	TopSphereDetector->SetupAttachment(RootComponent);
	TopSphereDetector->OnComponentBeginOverlap.AddDynamic(this, &AChunkManager::OnTopDetectorBeginOverlap);
	TopSphereDetector->OnComponentEndOverlap.AddDynamic(this, &AChunkManager::OnTopDetectorEndOverlap);
	
	BottomSphereDetector = CreateDefaultSubobject<USphereComponent>(TEXT("BottomSphereDetector"));
	//BottomSphereDetector->BodyInstance.SetCollisionProfileName("DetectLevels");
	BottomSphereDetector->SetupAttachment(RootComponent);
	BottomSphereDetector->OnComponentBeginOverlap.AddDynamic(this, &AChunkManager::OnBottomDetectorBeginOverlap);
	BottomSphereDetector->OnComponentEndOverlap.AddDynamic(this, &AChunkManager::OnBottomDetectorEndOverlap);
}

// Called when the game starts or when spawned
void AChunkManager::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, [this] () {
		//UGameplayStatics::GetPlayerController(this, 0);
		PlayerRef = GEngine->GetFirstLocalPlayerController(GetWorld());

		if (PlayerRef != nullptr)
		{
			// Because a lot of this relies on cameras being active we need to delay setup by a frame or two.
			// A more proper approach could be to have this class spawn in the level after a delay via some other code/BP.
			LoadLevelTriggerVolumes(LevelChunkContainerDataAsset);
			SetDetectorProjectionDistances(PlayerRef);
			SetDetectorLocations(PlayerRef);

			// Check if the player is currently overlapping with a LevelTriggerBox and spawn it in.
			TArray<AActor*> OverlappingActors;			
			PlayerRef->GetPawn()->GetOverlappingActors(OverlappingActors, ALevelTriggerBox::StaticClass());			
			for (int i = 0; i < OverlappingActors.Num(); i++)
			{
				ALevelTriggerBox* LevelTrigger = Cast<ALevelTriggerBox>(OverlappingActors[i]);
				if (LevelTrigger != nullptr)
				{
					TrySpawnSublevel(LevelTrigger);
				}
			}

			// For whatever reason, setting this in the constructor messes with GetOverlappingActors.
			TopSphereDetector->BodyInstance.SetCollisionProfileName("DetectLevels");
			BottomSphereDetector->BodyInstance.SetCollisionProfileName("DetectLevels");

			//PrevCameraLocation = PlayerRef->GetPawn()->GetActorLocation();
		}
	}, 0.33f, false);	
}

// Called every frame
void AChunkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: Check if the camera's location has changed since the previous frame before updating.

	if (PlayerRef != nullptr)
	{
		SetDetectorLocations(PlayerRef);
	}

}

void AChunkManager::OnTopDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALevelTriggerBox* LevelTrigger = Cast<ALevelTriggerBox>(OtherActor);
	if (LevelTrigger != nullptr)
	{
		UBoxComponent* BoxCollsion = Cast<UBoxComponent>(LevelTrigger->GetCollisionComponent());
		if (BoxCollsion != nullptr &&
			TopSphereDetector->GetComponentLocation().X < (BoxCollsion->GetComponentLocation().X + BoxCollsion->GetScaledBoxExtent().X))
		{
			TrySpawnSublevel(LevelTrigger);
		}		
	}
}

void AChunkManager::OnTopDetectorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALevelTriggerBox* LevelTrigger = Cast<ALevelTriggerBox>(OtherActor);
	if (LevelTrigger != nullptr)
	{
		UBoxComponent* BoxCollsion = Cast<UBoxComponent>(LevelTrigger->GetCollisionComponent());
		if (BoxCollsion != nullptr &&
			TopSphereDetector->GetComponentLocation().X <= (BoxCollsion->GetComponentLocation().X - BoxCollsion->GetScaledBoxExtent().X))
		{
			FLatentActionInfo Info;			
			UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(LevelTrigger->SublevelInstanceName), Info, false);
		}
	}
}

void AChunkManager::OnBottomDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALevelTriggerBox* LevelTrigger = Cast<ALevelTriggerBox>(OtherActor);
	if (LevelTrigger != nullptr)
	{
		UBoxComponent* BoxCollsion = Cast<UBoxComponent>(LevelTrigger->GetCollisionComponent());
		if (BoxCollsion != nullptr &&
			BottomSphereDetector->GetComponentLocation().X > (BoxCollsion->GetComponentLocation().X - BoxCollsion->GetScaledBoxExtent().X))
		{
			TrySpawnSublevel(LevelTrigger);
		}
	}
}

void AChunkManager::OnBottomDetectorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALevelTriggerBox* LevelTrigger = Cast<ALevelTriggerBox>(OtherActor);
	if (LevelTrigger != nullptr)
	{
		UBoxComponent* BoxCollsion = Cast<UBoxComponent>(LevelTrigger->GetCollisionComponent());
		if (BoxCollsion != nullptr &&
			BottomSphereDetector->GetComponentLocation().X >= (BoxCollsion->GetComponentLocation().X + BoxCollsion->GetScaledBoxExtent().X))
		{
			FLatentActionInfo Info;
			UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(LevelTrigger->SublevelInstanceName), Info, false);
		}
	}
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
	
	// Spawn top collider for the level.
	float TopColliderPositionX = (ChunkContainerDataAsset->SublevelExtents.X * 2.0f) * ChunkContainerDataAsset->SublevelMaps.Num();
	TopColliderPositionX = TopColliderPositionX - ChunkContainerDataAsset->SublevelExtents.X;
	FVector TopColliderPosition = FVector(TopColliderPositionX, 0.0f, 0.0f);
	AChunkBoundCollider* TopCollider = World->SpawnActor<AChunkBoundCollider>(TopColliderPosition, FRotator::ZeroRotator);
	TopCollider->SetBoundExtent(EndColliderExtent);

	// Spawn bottom collider for the level.
	float BottomColliderPositionX = -ChunkContainerDataAsset->SublevelExtents.X;
	FVector BottomColliderPosition = FVector(BottomColliderPositionX, 0.0f, 0.0f);
	AChunkBoundCollider* BottomCollider = World->SpawnActor<AChunkBoundCollider>(BottomColliderPosition, FRotator::ZeroRotator);
	BottomCollider->SetBoundExtent(EndColliderExtent);	
}

void AChunkManager::SetDetectorProjectionDistances(const APlayerController* Player)
{
	FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();

	// Get top distance.
	FVector TopWorldPosition;
	FVector TopWorldDirection;
	Player->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, 0.0f, TopWorldPosition, TopWorldDirection);
	FHitResult TopHitResult;	
	if (GetWorld()->LineTraceSingleByChannel(
		TopHitResult,
		TopWorldPosition,
		TopWorldPosition + (TopWorldDirection * 2000.0f),
		COLLISION_FLOOR))
	{
		ProjectionDistanceTop = TopHitResult.Distance;
	}

	// Get bottom distance.
	FVector BottomWorldPosition;
	FVector BottomWorldDirection;
	Player->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, ViewportSize.Y, BottomWorldPosition, BottomWorldDirection);	
	FHitResult BottomHitResult;
	if (GetWorld()->LineTraceSingleByChannel(
		BottomHitResult,
		BottomWorldPosition,
		BottomWorldPosition + (BottomWorldDirection * 2000.0f),
		COLLISION_FLOOR))
	{
		ProjectionDistanceBottom = BottomHitResult.Distance;
	}
}

void AChunkManager::SetDetectorLocations(const APlayerController* Player)
{
	FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	
	FVector TopWorldPosition;
	FVector TopWorldDirection;
	Player->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, 0.0f, TopWorldPosition, TopWorldDirection);
	FVector TopSphereLocation = TopWorldPosition + (TopWorldDirection * ProjectionDistanceTop);
	TopSphereDetector->SetWorldLocation(TopSphereLocation, true);

	FVector BottomWorldPosition;
	FVector BottomWorldDirection;
	Player->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, ViewportSize.Y, BottomWorldPosition, BottomWorldDirection);
	FVector BottomSphereLocation = BottomWorldPosition + (BottomWorldDirection * ProjectionDistanceBottom);
	BottomSphereDetector->SetWorldLocation(BottomSphereLocation, true);
}

void AChunkManager::TrySpawnSublevel(ALevelTriggerBox* LevelTrigger)
{	
	if (LevelTrigger->SublevelInstanceName.IsEmpty())
	{
		// Generate sublevel name.
		FString LevelName = "Sublevel";
		LevelName += FString::FromInt(LevelTrigger->SublevelIndex);
		LevelName += "_";
		LevelName += FGuid::NewGuid().ToString();
		LevelTrigger->SublevelInstanceName = LevelName;

		// Load level instance as this is the first time loading it..	
		bool IsLoaded;
		ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
			GetWorld(), 
			LevelTrigger->Sublevel, 
			LevelTrigger->GetActorLocation(), 
			FRotator::ZeroRotator, 
			IsLoaded,
			LevelTrigger->SublevelInstanceName);
	}
	else
	{
		// Stream the existing sublevel.
		FLatentActionInfo Info;
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName(LevelTrigger->SublevelInstanceName), true, false, Info);
	}
}

