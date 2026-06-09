#include "MinimapCaptureActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

AMinimapCaptureActor::AMinimapCaptureActor()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = false;

    Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(
        TEXT("Capture"));

    RootComponent = Capture;

   
    // 자동 캡처
    Capture->bCaptureEveryFrame = true;
    Capture->bCaptureOnMovement = true;

    // 렌더 유지
    Capture->bAlwaysPersistRenderingState = true;

    // 최종 컬러 캡처
    Capture->CaptureSource =
        ESceneCaptureSource::SCS_FinalColorLDR;

    // 월드 전체 렌더
    Capture->PrimitiveRenderMode =
        ESceneCapturePrimitiveRenderMode::
        PRM_RenderScenePrimitives;

    Capture->ProjectionType = ECameraProjectionMode::Orthographic;
    
    Capture->OrthoWidth = 250000;    
}

void AMinimapCaptureActor::BeginPlay()
{
    Super::BeginPlay();

    if (!MinimapRT)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("MinimapRT is NULL at BeginPlay"));

        return;
    }

    Capture->TextureTarget = MinimapRT;

    UE_LOG(LogTemp, Warning,
        TEXT("Minimap RT Connected"));
}

void AMinimapCaptureActor::SetRenderTarget(
    UTextureRenderTarget2D* RT)
{
    if (!RT)
    {
        UE_LOG(LogTemp, Error,
            TEXT("SetRenderTarget : RT NULL"));

        return;
    }

    MinimapRT = RT;

    Capture->TextureTarget = RT;

    UE_LOG(LogTemp, Warning,
        TEXT("RenderTarget Assigned"));
}