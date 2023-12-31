// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralLandscape.h"
#include "../Pickups/PickupBase.h"
#include "../Pickups/WeaponPickup.h"
#include "EngineUtils.h"


// Sets default values
AProceduralLandscape::AProceduralLandscape()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	SetRootComponent(ProceduralMesh);
}

bool AProceduralLandscape::ShouldTickIfViewportsOnly() const
{
	return true;
}

// Called when the game starts or when spawned
void AProceduralLandscape::BeginPlay()
{
	Super::BeginPlay();
}

void AProceduralLandscape::DebugShowNavNodes()
{
	//Call the DebugShowNavNodes function on each NavNode
	for (ANavigationNode* Node : Nodes)
	{
		Node->DebugSetVisibility(bDebugShowNavNodes);
	}
	
}

void AProceduralLandscape::GenerateLandscape()
{
	//Add the vertices, UV Coords and Tris to the arrays
	for (int32 Y = 0; Y < Height; Y++)
	{
		for (int32 X = 0; X < Width; X++)
		{
			Vertices.Add(FVector(X * VertexSpacing, Y * VertexSpacing, 0.0f));
	
			UVCoords.Add(FVector2d(static_cast<float>(X), static_cast<float>(Y)));
		}
	}

	//Add NavNodes
	/*for (FVector& Vertex : Vertices)
	{
		if (ANavigationNode* NavNode = GetWorld()->SpawnActor<ANavigationNode>(
			ANavigationNode::StaticClass(), Vertex, FRotator::ZeroRotator))
		{
			Nodes.Add(NavNode);
			NavNode->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}*/

	// Define the triangles and NavNode Connections
	for (int32 Y = 0; Y < Height - 1; Y++)
	{
		for (int32 X = 0; X < Width - 1; X++)
		{
			// Define the indices of the four vertices of the current quad
			int32 BottomRight = X + Y * Width;
			int32 BottomLeft = (X + 1) + Y * Width;
			int32 TopRight = X + (Y + 1) * Width;
			int32 TopLeft = (X + 1) + (Y + 1) * Width;

			//Define the 4 NavNode vertices
			/*ANavigationNode* BottomRightNode = Nodes[BottomRight];
			ANavigationNode* BottomLeftNode = Nodes[BottomLeft];
			ANavigationNode* TopRightNode = Nodes[TopRight];
			ANavigationNode* TopLeftNode = Nodes[TopLeft];*/

			//TArray<ANavigationNode*> QuadNodes = {BottomRightNode, BottomLeftNode, TopRightNode, TopLeftNode};

			/*for (ANavigationNode* Node : QuadNodes)
			{
				for (ANavigationNode* OtherNode : QuadNodes)
				{
					if (Node != OtherNode && !Node->GetConnectedNodes().Contains(OtherNode))
					{
						Node->SetConnectedNodes(OtherNode);
					}
				}
			}*/
			// Define the two triangles for this quad
			// Triangle 1: 
			Triangles.Add(BottomRight);
			Triangles.Add(TopRight);
			Triangles.Add(BottomLeft);

			// Triangle 2: 
			Triangles.Add(BottomLeft);
			Triangles.Add(TopRight);
			Triangles.Add(TopLeft);
		}
	}


	// Draw debug spheres at each vertex location with a radius of 50.0f
	/*for (const FVector& Vertex : Vertices)
	{
		DrawDebugSphere(GetWorld(), Vertex, 50.0f, 12, FColor::Green, true, -1.0f, 0, 1.0f);
	}

	//Print triangles array as a formatted string
	FString TrianglesString = "[";
	for (int32 Index = 0; Index < Triangles.Num(); ++Index)
	{
		TrianglesString += FString::Printf(TEXT("%d"), Triangles[Index]);

		if (Index < Triangles.Num() - 1)
		{
			TrianglesString += TEXT(", ");
		}
	}
	TrianglesString += TEXT("]");

	// Log the Triangles array as a formatted string
	UE_LOG(LogTemp, Warning, TEXT("Triangles: %s"), *TrianglesString);
	*/
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVCoords, Normals, Tangents);

	if (ProceduralMesh)
	{
		ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVCoords,
		                                  TArray<FColor>(), Tangents, true);
	}
}

void AProceduralLandscape::CreateSimplePlane()
{
	// Clear the existing vertices if needed
	Vertices.Empty();

	// Add the specified vectors to the Vertices array
	FVector Vector1(0.0f, 0.0f, 0.0f);
	FVector Vector2(1000.0f, 0.0f, 0.0f);
	FVector Vector3(0.0f, 1000.0f, 0.0f);
	FVector Vector4(1000.0f, 1000.0f, 0.0f);

	Vertices.Add(Vector1);
	Vertices.Add(Vector2);
	Vertices.Add(Vector3);
	Vertices.Add(Vector4);

	// Add the specified integers to the Triangles array
	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(1);
	Triangles.Add(2);
	Triangles.Add(3);

	// Add the specified FVector2D to the UVCoords array
	FVector2D UV1(0.0f, 0.0f);
	FVector2D UV2(1.0f, 0.0f);
	FVector2D UV3(0.0f, 1.0f);
	FVector2D UV4(1.0f, 1.0f);

	UVCoords.Add(UV1);
	UVCoords.Add(UV2);
	UVCoords.Add(UV3);
	UVCoords.Add(UV4);

	// Draw debug spheres at each vertex location with a radius of 50.0f
	for (const FVector& Vertex : Vertices)
	{
		DrawDebugSphere(GetWorld(), Vertex, 50.0f, 12, FColor::Green, true, -1.0f, 0, 1.0f);
	}

	if (ProceduralMesh)
	{
		ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UVCoords,
		                                  TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}
}

void AProceduralLandscape::ClearLandscape()
{
	Vertices.Empty();
	Triangles.Empty();
	UVCoords.Empty();
	
	//Iterate through all APickupBase actors in the world and destroy them
	for (TActorIterator<APickupBase> It(GetWorld()); It; ++It)
	{
		if (*It)
		{
			(*It)->Destroy();
		}
	}

	//Destroy the nodes and clear the array
	for (ANavigationNode* NavNode : Nodes)
	{
		NavNode->DestroyNode();
	}
	Nodes.Empty();
	ProceduralMesh->ClearMeshSection(0);
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
}

void AProceduralLandscape::SpawnPickups()
{
	//If a pickup type exists, spawn 5 at random nodes
	if (PickupBlueprint)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			for (int32 i = 0; i < 5; ++i)
			{
				int32 RandomIndex = FMath::RandRange(0, Nodes.Num() - 1);
				ANavigationNode* RandomNode = Nodes[RandomIndex];
				FVector RandomLocation = RandomNode->GetActorLocation();
				// Store a reference to the spawned pickup in the array
				World->SpawnActor<APickupBase>(PickupBlueprint, RandomLocation, FRotator::ZeroRotator);
			}
		}
	}
}

// Called every frame
void AProceduralLandscape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bShouldRegenerate)
	{
		ClearLandscape();
		GenerateLandscape();
		//SpawnPickups(); //Now implemented in PickupManagerSubsystem
		bShouldRegenerate = false;
	}
	if (bDebugNeedsUpdate)
	{
		DebugShowNavNodes();
		bDebugNeedsUpdate = false;
	}
}
