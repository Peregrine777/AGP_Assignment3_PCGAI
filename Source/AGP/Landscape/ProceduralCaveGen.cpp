﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralCaveGen.h"


#include "Algo/RandomShuffle.h"
#include "Kismet/GameplayStatics.h"
#include "Voxels/MarchingChunkTerrain.h"


// Sets default values
AProceduralCaveGen::AProceduralCaveGen()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool AProceduralCaveGen::ShouldTickIfViewportsOnly() const
{
	return true;
}

// Called when the game starts or when spawned
void AProceduralCaveGen::BeginPlay()
{
	Super::BeginPlay();
	//Boxes = GenerateGuaranteedPathBoxes(NumBoxes, MinSize, MaxSize);
	//Tunnels = GenerateTunnels(Boxes);
}

//ToDo: Tidy up second path by generalizing the function with for loop
TArray<FLevelBox> AProceduralCaveGen::GenerateGuaranteedPathBoxes(int NumBoxesToGenerate, FVector BoxMinSize,
                                                                  FVector BoxMaxSize)
{
	//Start and End Locations
	FVector Start = FVector(GetActorLocation());
	FVector End = FVector(LevelSize, LevelSize, FMath::RandRange(-HeightDifference, HeightDifference));
	TArray<FLevelBox> boxes;

	//Start Box
	FLevelBox StartBox;
	StartBox.Position = Start;
	StartBox.Size = FVector
	(
		FMath::RandRange(BoxMinSize.X, BoxMaxSize.X),
		FMath::RandRange(BoxMinSize.Y, BoxMaxSize.Y),
        FMath::RandRange(BoxMinSize.Z, BoxMaxSize.Z));
	StartBox.Type = EBoxType::Start;
	boxes.Add(StartBox);
	

	//Find paths up to number of paths
	FVector LastPosition = Start;
	for (int i = 0 ; i < NumPaths ; i++)
	{
		FInnerArray Path;
		Paths.Add(Path);
		LastPosition = Start;
		for (int j = 0 ; j < NumBoxesPerPath ; j++)
		{
			FLevelBox box;

			// Get a direction biased towards the destination
			FVector BiasedDirection = (End - LastPosition).GetSafeNormal();

			// Randomize within a hemisphere towards the destination
			FVector RandomDirection = FMath::VRandCone(BiasedDirection, FMath::DegreesToRadians(30.f));

			// Constrain the vertical movement
			float MaxVerticalOffset = 200.0f;  // Set this to the max vertical difference you want between rooms
			float verticalComponent = FMath::Clamp(RandomDirection.Z, -MaxVerticalOffset, MaxVerticalOffset);
			RandomDirection.Z = verticalComponent;

			FVector newPosition = LastPosition + RandomDirection * FMath::RandRange(
				0.5f * MaxConnectionDistance, MaxConnectionDistance);
			box.Position = newPosition;
			box.Size = FVector(FMath::RandRange(BoxMinSize.X, BoxMaxSize.X), FMath::RandRange(BoxMinSize.Y, BoxMaxSize.Y),
							   FMath::RandRange(BoxMinSize.Z, BoxMaxSize.Z));
			box.Type = EBoxType::Normal;

			
			//If box doesn't collide with any other boxes, add it to the list
			if (BoxPositionValid(box, boxes))
			{
				FLevelBox lastBox;
				j == 0 ? lastBox = StartBox : lastBox = Paths[i].Path[j - 1];
				boxes.Add(box);
				Paths[i].Path.Add(box);
				CreateTunnel(lastBox, box);
				LastPosition = newPosition;
			}
			else
			{
				j--;
			}
		}
	}

	//End Box
	FLevelBox EndBox{End, FVector(0, 0, 0), EBoxType::End};
	EndBox.Size = FVector
	(
		FMath::RandRange(BoxMinSize.X, BoxMaxSize.X),
		FMath::RandRange(BoxMinSize.Y, BoxMaxSize.Y),
		FMath::RandRange(BoxMinSize.Z, BoxMaxSize.Z)
	);
	EndBox.Type = EBoxType::End;
	boxes.Add(EndBox);

	
	return boxes;
}

void AProceduralCaveGen::GenerateInterconnects()
{
	//ToDo: Randomly choose interconnects
	const int numInterconnects = Paths[0].Path.Num()/2;
	TArray<int> InterconnectIndices;
	for (int i = 0; i < Paths[0].Path.Num(); i++)
	{
		InterconnectIndices.Add(i);
	}
	Algo::RandomShuffle(InterconnectIndices);
	//std::shuffle(InterconnectIndices.begin(), InterconnectIndices.end());
	for (int i = 0 ; i < numInterconnects; i++)
	{
		FLevelBox& BoxA = Paths[0].Path[InterconnectIndices[i]];
		FLevelBox& BoxB = Paths[1].Path[InterconnectIndices[i]];
		CreateTunnel(BoxA, BoxB);
	}
}

void AProceduralCaveGen::CreateTunnel(const FLevelBox& StartBox, const FLevelBox& TargetBox)
{
	FTunnel Tunnel;
	Tunnel.StartBox = &StartBox;
	Tunnel.EndBox = &TargetBox;

	FVector Start = StartBox.Position;
	FVector End = TargetBox.Position;
	Tunnel.Position = (Start + End) / 2;
	FVector Direction = (End - Start).GetSafeNormal();
	Tunnel.Size = FVector((End - Start).Size(), 200, 200);
	Tunnel.Rotation = FQuat::FindBetweenNormals(FVector::ForwardVector, Direction);

	Tunnels.Add(Tunnel);
}

bool AProceduralCaveGen::BoxesIntersect(const FLevelBox& BoxA, const FLevelBox& BoxB)
{
	// Check for gap along X axis
	if (BoxA.Position.X + BoxA.Size.X < BoxB.Position.X ||
		BoxB.Position.X + BoxB.Size.X < BoxA.Position.X)
	{
		return false;
	}

	// Check for gap along Y axis
	if (BoxA.Position.Y + BoxA.Size.Y < BoxB.Position.Y ||
		BoxB.Position.Y + BoxB.Size.Y < BoxA.Position.Y)
	{
		return false;
	}

	// Check for gap along Z axis
	/*if (BoxA.Position.Z + BoxA.Size.Z < BoxB.Position.Z ||
		BoxB.Position.Z + BoxB.Size.Z < BoxA.Position.Z)
	{
		return false;
	}*/

	// If there's no gap along any axis, then the boxes intersect
	return true;
}

void AProceduralCaveGen::GenerateMesh()
{
	//For loop for X/Y/Z chunks within levelSize

	if (bDebugOnly1Chunk)
	{
		for (int i = 0 ; i < 1; i++)
		{
			FVector ChunkPosition = FVector( ChunkSize/2, i*ChunkSize + ChunkSize/2, ChunkSize/2);
			FLevelBox ChunkBox{ChunkPosition, FVector(ChunkSize*2, ChunkSize*2, ChunkSize*2), EBoxType::Normal};
			for (FLevelBox box : Boxes)
			{
				if (BoxesIntersect(ChunkBox, box))
				{
					const auto Chunk = GetWorld()->SpawnActorDeferred<AMarchingChunkTerrain>
					(
						AMarchingChunkTerrain::StaticClass(),
						FTransform(), //Replace this with a transform that places the chunk in the correct location
						this
					);
					Chunk->ChunkPosition = ChunkPosition;
					Chunk->ChunkSize = ChunkSize;
					Chunk->VoxelsPerSide = VoxelDensity;
					Chunk->Boxes = Boxes;
					Chunk->Tunnels = Tunnels;
					Chunk->Material = Material;
					Chunk->LevelSize = static_cast<int>(LevelSize);
					Chunk->bUpdateMesh = bUpdateMesh;
					Chunk->DebugChunk = DebugChunk;
					Chunk->DebugVoxels = DebugVoxels;
					Chunk->DebugInverted = DebugInvertSolids;
						
					UGameplayStatics::FinishSpawningActor(Chunk, FTransform());

					Chunks.Add(Chunk);
					break;
				}
			}
		}
		
	}
	else
	{
		for (int x = -1 ; x <= LevelSize/ChunkSize ; x++)
		{
			for (int y = -1 ; y <= LevelSize/ChunkSize ; y++)
			{
				for (int z = -6*HeightDifference/ChunkSize ; z <= 6 * HeightDifference/ChunkSize ; z++)
				{
					FVector ChunkPosition = FVector(x * ChunkSize + ChunkSize/2, y * ChunkSize + ChunkSize/2, z * ChunkSize + ChunkSize/2);
					//Check if chunk intersects with any boxes - if not, skip it
					//ChunkSize is doubled to be safe
					FLevelBox ChunkBox{ChunkPosition, FVector(ChunkSize*2, ChunkSize*2, ChunkSize*2), EBoxType::Normal};
					for (FLevelBox box : Boxes)
					{
						if (BoxesIntersect(ChunkBox, box))
						{
							const auto Chunk = GetWorld()->SpawnActorDeferred<AMarchingChunkTerrain>
							(
								AMarchingChunkTerrain::StaticClass(),
								FTransform(), //Replace this with a transform that places the chunk in the correct location
								this
							);
							Chunk->ChunkPosition = ChunkPosition;
							Chunk->ChunkSize = ChunkSize;
							Chunk->VoxelsPerSide = VoxelDensity;
							Chunk->Boxes = Boxes;
							Chunk->Tunnels = Tunnels;
							Chunk->Material = Material;
							Chunk->LevelSize = static_cast<int>(LevelSize);
							Chunk->bUpdateMesh = bUpdateMesh;
							Chunk->DebugChunk = DebugChunk;
							Chunk->DebugVoxels = DebugVoxels;
						
							UGameplayStatics::FinishSpawningActor(Chunk, FTransform());

							Chunks.Add(Chunk);
							break;
						}
					}
				}
			}
		}
	}
	

}

bool AProceduralCaveGen::BoxPositionValid(const FLevelBox& NewBox, const TArray<FLevelBox>& AllBoxes)
{
	for (const FLevelBox& Box : AllBoxes)
	{
		if (BoxesIntersect(Box, NewBox))
		{
			return false;
		}
	}
	return true;
}

// Called every frame
void AProceduralCaveGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShouldRegenerate)
	{
		Boxes.Empty();
		Tunnels.Empty();
		if (!Paths.IsEmpty())
		{
			for (FInnerArray& Path : Paths)
			{

				Path.Path.Empty();

				
			}
		}
		//If the marched terrain chunk has been spawned, destroy it
		if (!Chunks.IsEmpty())
		{
			for (AMarchingChunkTerrain* Chunk : Chunks)
			{
				if (Chunk)
				{
					Chunk->ClearMesh();
					Chunk->Destroy();
				}
			}
			Chunks.Empty();
		}

		//If level has only just been initialized iterate through marching chunks and destroy them

		Boxes = GenerateGuaranteedPathBoxes(NumBoxesPerPath, MinSize, MaxSize);
		GenerateInterconnects();

		GenerateMesh();
		//SpawnPickups(); //Now implemented in PickupManagerSubsystem
		bShouldRegenerate = false;
	}

	//Debug to visualize boxes
	if (!Boxes.IsEmpty() && bDebugView)
	{
		for (const FLevelBox& box : Boxes)
		{
			// Box center
			FVector center = box.Position;

			// Half of the box size in each dimension
			FVector halfSize = box.Size * 0.5;

			FColor color = FColor::Red;
			switch (box.Type)
			{
			case EBoxType::Start:
				color = FColor::Blue;
				break;
			case EBoxType::End:
				color = FColor::Yellow;
				break;
			default:
				color = FColor::Red;
				break;
			}
			// Draw the box every frame
			DrawDebugBox(GetWorld(), center, halfSize, color, false, -1, 0, 2.0f);
		}
		if (!Tunnels.IsEmpty())
		{
			for (const FTunnel& tunnel : Tunnels)
			{
				DrawDebugBox(GetWorld(), tunnel.Position, tunnel.Size / 2, tunnel.Rotation, FColor::Green, false, -1.0,
				             0, 10.0f);
			}
		}
	}
}
