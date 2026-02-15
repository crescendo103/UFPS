// Fill out your copyright notice in the Description page of Project Settings.


#include "ExportNavMeshVolumePos.h"

#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavMesh/NavMeshRenderingComponent.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


// Sets default values
AExportNavMeshVolumePos::AExportNavMeshVolumePos()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExportNavMeshVolumePos::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExportNavMeshVolumePos::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
void AExportNavMeshVolumePos::ExportNavMeshPos()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavSys) return;

    const ARecastNavMesh* NavMesh =
        Cast<ARecastNavMesh>(NavSys->GetDefaultNavDataInstance());
    if (!NavMesh) return;


   


    FString Out;

    
    FRecastDebugGeometry Geom;

    Geom.bGatherNavMeshEdges = true;
    Geom.bGatherPolyEdges = true;

    NavMesh->GetDebugGeometry(Geom, true);

    UE_LOG(LogTemp, Warning, TEXT("Verts: %d  Indices: %d"),
        Geom.MeshVerts.Num(),
        Geom.BuiltMeshIndices.Num());

    
    for (int32 i = 0; i + 2 < Geom.BuiltMeshIndices.Num(); i += 3)
    {

        const FVector& A = Geom.MeshVerts[Geom.BuiltMeshIndices[i]];
        const FVector& B = Geom.MeshVerts[Geom.BuiltMeshIndices[i + 1]];
        const FVector& C = Geom.MeshVerts[Geom.BuiltMeshIndices[i + 2]];

        FVector Center = (A + B + C) / 3.f;

        Out += FString::Printf(TEXT("%f,%f,%f\n"),
            Center.X, Center.Y, Center.Z);
    }

    FString Path = FPaths::ProjectDir() / TEXT("NavMeshCenters.csv");
    FFileHelper::SaveStringToFile(Out, *Path);

    UE_LOG(LogTemp, Warning, TEXT("NavMesh exported to %s"), *Path);
}
*/

ARecastNavMesh* AExportNavMeshVolumePos::GetNavMeshData() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	if (!NavSys) return nullptr;

	return Cast<ARecastNavMesh>(NavSys->GetDefaultNavDataInstance());
}

void AExportNavMeshVolumePos::ExportNavMeshPos()
{
    ARecastNavMesh* NavMesh = GetNavMeshData();
    if (!NavMesh) return;

    FString Out;

    int32 TileCount = NavMesh->GetNavMeshTilesCount();
    UE_LOG(LogTemp, Warning, TEXT("Tiles: %d"), TileCount);

    for (int32 TileIdx = 0; TileIdx < TileCount; ++TileIdx)
    {
        TArray<FNavPoly> Polys;
        if (!NavMesh->GetPolysInTile(TileIdx, Polys))
            continue;

        for (const FNavPoly& Poly : Polys)
        {
            TArray<FVector> Verts;
            if (!NavMesh->GetPolyVerts(Poly.Ref, Verts))
                continue;

            FVector Center(0);
            for (const FVector& V : Verts)
                Center += V;

            Center /= Verts.Num();

            Out += FString::Printf(TEXT("%f,%f,%f\n"),
                Center.X, Center.Y, Center.Z);
        }
    }

    FString Path = FPaths::ProjectDir() / TEXT("NavMeshCenters.csv");
    FFileHelper::SaveStringToFile(Out, *Path);

    UE_LOG(LogTemp, Warning, TEXT("NavMesh poly centers exported to %s"), *Path);
}