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
#include "ChunkWorldSubsystem.h"
#include "ChunkSphereDetector.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/Level.h"
//#include "Kismet/GameplayStatics.h"

// Sets default values
AChunkManager::AChunkManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TopSphereDetector = CreateDefaultSubobject<UChunkSphereDetector>(TEXT("TopSphereDetector"));
	TopSphereDetector->SetCollisionObjectType(DetectorCollisionChannel);
	TopSphereDetector->SetupAttachment(RootComponent);
	TopSphereDetector->OnComponentBeginOverlap.AddDynamic(this, &AChunkManager::OnTopDetectorBeginOverlap);
	TopSphereDetector->OnComponentEndOverlap.AddDynamic(this, &AChunkManager::OnTopDetectorEndOverlap);

	BottomSphereDetector = CreateDefaultSubobject<UChunkSphereDetector>(TEXT("BottomSphereDetector"));
	BottomSphereDetector->SetCollisionObjectType(DetectorCollisionChannel);
	BottomSphereDetector->SetupAttachment(RootComponent);
	BottomSphereDetector->OnComponentBeginOverlap.AddDynamic(this, &AChunkManager::OnBottomDetectorBeginOverlap);
	BottomSphereDetector->OnComponentEndOverlap.AddDynamic(this, &AChunkManager::OnBottomDetectorEndOverlap);	
}

// Called when the game starts or when spawned
void AChunkManager::BeginPlay()
{
	Super::BeginPlay();

	int32 LevelArraySize = 0;

	if (LevelChunkContainerDataAsset != nullptr)
	{
		LevelArraySize = LevelChunkContainerDataAsset->SublevelMaps.Num();
	}

	LoadingLevelsArray.Init(nullptr, LevelArraySize);	

	ChunkSubsystem = GetWorld()->GetSubsystem<UChunkWorldSubsystem>();

	FTimerHandle Handle;
	// We need a slight delay in order for the camera to be ready for projections.
	GetWorldTimerManager().SetTimer(Handle, [this]() {		
		PlayerRef = GEngine->GetFirstLocalPlayerController(GetWorld());

		if (PlayerRef != nullptr)
		{
			// Because a lot of this relies on cameras being active we need to delay setup by a frame or two.
			// A more proper approach could be to have this class spawn in the level after a delay via some other code/BP.
			LoadLevelTriggerVolumes(LevelChunkContainerDataAsset);
			SetDetectorProjectionDistances(PlayerRef);
			SetDetectorLocations(PlayerRef);

			//Check for all triggers between the detectors and spawn them.
			for (TActorIterator<ALevelTriggerBox> TriggerItr(GetWorld()); TriggerItr; ++TriggerItr)
			{
				ALevelTriggerBox* LevelTrigger = *TriggerItr;
				if (TopSphereDetector->IsOverlappingActor(LevelTrigger) || BottomSphereDetector->IsOverlappingActor(LevelTrigger))
				{					
					TrySpawnSublevel(LevelTrigger);
				}
				else
				{
					UBoxComponent* BoxCollsion = Cast<UBoxComponent>(LevelTrigger->GetCollisionComponent());
					if (BoxCollsion != nullptr)
					{
						float LevelTriggerLocationX = LevelTrigger->GetActorLocation().X;
						float LevelTriggerExtentX = BoxCollsion->GetScaledBoxExtent().X;
						float LevelTriggerTop = LevelTriggerLocationX + LevelTriggerExtentX;
						float LevelTriggerBottom = LevelTriggerLocationX - LevelTriggerExtentX;
						float TopDetectorLocationX = TopSphereDetector->GetComponentLocation().X;
						float BottomDetectorLocationX = BottomSphereDetector->GetComponentLocation().X;
						if ((LevelTriggerTop <= TopDetectorLocationX && LevelTriggerTop > BottomDetectorLocationX) ||
							(LevelTriggerBottom >= BottomDetectorLocationX && LevelTriggerBottom < TopDetectorLocationX))
						{							
							TrySpawnSublevel(LevelTrigger);
						}
					}
				}
			}			

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

	// Update the loading and streaming status of levels.
	for (int32 i = 0; i != LoadingLevelsArray.Num(); ++i)
	{
		if (LoadingLevelsArray[i] != nullptr)
		{
			if (LoadingLevelsArray[i]->IsLevelLoaded() && ChunkSubsystem != nullptr)
			{
				// TODO: Get and transmit actors to subsystems.
				ULevel* Level = LoadingLevelsArray[i]->GetLoadedLevel();
				if (Level != nullptr)
				{														
					ChunkSubsystem->NotifySublevelLoaded(Level, LoadingLevelsArray[i]->GetFName().ToString(), i);
					LoadingLevelsArray[i] = nullptr;
				}								
			}
		}
	}
}

void AChunkManager::OnTopDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TopSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::UNLOAD_ONLY ||
		TopSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::NONE)
	{
		return;
	}

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
	if (TopSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::LOAD_ONLY ||
		TopSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::NONE)
	{
		return;
	}

	ALevelTriggerBox* LevelTrigger = Cast<ALevelTriggerBox>(OtherActor);
	if (LevelTrigger != nullptr)
	{
		UBoxComponent* BoxCollsion = Cast<UBoxComponent>(LevelTrigger->GetCollisionComponent());
		if (BoxCollsion != nullptr &&
			TopSphereDetector->GetComponentLocation().X <= (BoxCollsion->GetComponentLocation().X - BoxCollsion->GetScaledBoxExtent().X))
		{
			FLatentActionInfo Info;
			UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(LevelTrigger->SublevelInstanceName), Info, false);
			if (ChunkSubsystem != nullptr)
			{
				ChunkSubsystem->NotifySublevelUnloaded(LevelTrigger->SublevelInstanceName, LevelTrigger->SublevelIndex);
			}
		}
	}
}

void AChunkManager::OnBottomDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (BottomSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::UNLOAD_ONLY ||
		BottomSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::NONE)
	{
		return;
	}

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
	if (BottomSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::LOAD_ONLY ||
		BottomSphereDetector->GetLoadBehaviour() == EDetectorLoadBehaviour::NONE)
	{
		return;
	}

	ALevelTriggerBox* LevelTrigger = Cast<ALevelTriggerBox>(OtherActor);
	if (LevelTrigger != nullptr)
	{
		UBoxComponent* BoxCollsion = Cast<UBoxComponent>(LevelTrigger->GetCollisionComponent());
		if (BoxCollsion != nullptr &&
			BottomSphereDetector->GetComponentLocation().X >= (BoxCollsion->GetComponentLocation().X + BoxCollsion->GetScaledBoxExtent().X))
		{
			FLatentActionInfo Info;
			UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(LevelTrigger->SublevelInstanceName), Info, false);
			if (ChunkSubsystem != nullptr)
			{
				ChunkSubsystem->NotifySublevelUnloaded(LevelTrigger->SublevelInstanceName, LevelTrigger->SublevelIndex);
			}
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
		FTransform FloorSpawnTransform(FRotator::ZeroRotator, SpawnLocation);
		AChunkFloorTile* FloorTile = World->SpawnActorDeferred<AChunkFloorTile>(AChunkFloorTile::StaticClass(), FloorSpawnTransform);
		if (FloorTile != nullptr)
		{
			FloorTile->SetFloorCollisionChannel(FloorCollisionChannel);
			FVector FloorExtents = ChunkContainerDataAsset->SublevelExtents;
			FloorExtents.Z = SpawnOffsetZ;
			FloorTile->SetFloorExtent(FloorExtents);

			UGameplayStatics::FinishSpawningActor(FloorTile, FloorSpawnTransform);
		}

		// Spawn the chunk trigger box. 
		SpawnLocation.Z = 0.0f;
		FTransform LevelTriggerSpawnTransform(FRotator::ZeroRotator, SpawnLocation);		
		ALevelTriggerBox* ChunkTriggerBox = World->SpawnActorDeferred<ALevelTriggerBox>(ALevelTriggerBox::StaticClass(), LevelTriggerSpawnTransform);
		if (ChunkTriggerBox != nullptr)
		{
			ChunkTriggerBox->GetCollisionComponent()->SetCollisionObjectType(LevelTriggerCollisionChannel);
			ChunkTriggerBox->Sublevel = ChunkContainerDataAsset->SublevelMaps[i];
			ChunkTriggerBox->SublevelIndex = i;
			UBoxComponent* CollisionComponent = Cast<UBoxComponent>(ChunkTriggerBox->GetCollisionComponent());
			if (CollisionComponent != nullptr)
			{
				CollisionComponent->SetBoxExtent(ChunkContainerDataAsset->SublevelExtents);
			}

			UGameplayStatics::FinishSpawningActor(ChunkTriggerBox, LevelTriggerSpawnTransform);
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
	FVector TopLineEnd = TopWorldPosition + (TopWorldDirection * 2000.0f);
	FVector TopIntersection = FMath::LinePlaneIntersection(TopWorldPosition, TopLineEnd, FloorDetectionOffset, FVector(0.f, 0.f, 1.f));
	ProjectionDistanceTop = FVector::Distance(TopWorldPosition, TopIntersection);	

	// Get bottom distance.
	FVector BottomWorldPosition;
	FVector BottomWorldDirection;
	Player->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, ViewportSize.Y, BottomWorldPosition, BottomWorldDirection);
	FVector BottomLineEnd = BottomWorldPosition + (BottomWorldDirection * 2000.0f);
	FVector BottomIntersection = FMath::LinePlaneIntersection(BottomWorldPosition, BottomLineEnd, FloorDetectionOffset, FVector(0.f, 0.f, 1.f));
	ProjectionDistanceBottom = FVector::Distance(BottomWorldPosition, BottomIntersection);	
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
	bool IsLoaded = false;

	// If there is no name, it's our first time spawning.
	if (LevelTrigger->SublevelInstanceName.IsEmpty())
	{
		// This naming is very specific to the UE4 level system, and shouldn't be modified.
		FString LevelName = "LevelStreamingDynamic_";
		LevelName += FString::FromInt(LevelInstansiatedCounter);

		// Load level instance as this is the first time loading it..			
		ULevelStreamingDynamic* LoadedLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
			GetWorld(),
			LevelTrigger->Sublevel,
			LevelTrigger->GetActorLocation(),
			FRotator::ZeroRotator,
			IsLoaded,
			LevelName);

		if (IsLoaded && LoadedLevel != nullptr)
		{
			LevelInstansiatedCounter++;
			LevelTrigger->SublevelInstanceName = LoadedLevel->GetFName().ToString();
			LoadingLevelsArray[LevelTrigger->SublevelIndex] = LoadedLevel;						
		}
	}
	else
	{
		// Make sure the level isn't streaming or loading.
		if (LoadingLevelsArray[LevelTrigger->SublevelIndex] == nullptr)
		{
			ULevelStreaming* LevelStreaming = UGameplayStatics::GetStreamingLevel(this, FName(LevelTrigger->SublevelInstanceName));
			if (LevelStreaming != nullptr && !LevelStreaming->IsLevelLoaded() && !LevelStreaming->HasLoadRequestPending())
			{								
				FLatentActionInfo Info;				
				UGameplayStatics::LoadStreamLevel(GetWorld(), FName(LevelTrigger->SublevelInstanceName), true, false, Info);
				LoadingLevelsArray[LevelTrigger->SublevelIndex] = LevelStreaming;
			}
		}
	}
}