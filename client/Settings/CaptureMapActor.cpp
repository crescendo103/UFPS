// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureMapActor.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformProcess.h"   // ⭐ 필수
#include "Misc/Paths.h"            // ⭐ 추천
#include "Components/SceneCaptureComponent2D.h"

// Sets default values
ACaptureMapActor::ACaptureMapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Root
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // SceneCapture 생성
    CaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));
    CaptureComp->SetupAttachment(RootComponent);

    // 위에서 아래로
    CaptureComp->SetRelativeLocation(FVector(0, 0, 3000));
    CaptureComp->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

    // Orthographic 설정
    CaptureComp->ProjectionType = ECameraProjectionMode::Orthographic;
    CaptureComp->OrthoWidth = 10000.f;
}

// Called when the game starts or when spawned
void ACaptureMapActor::BeginPlay()
{
	Super::BeginPlay();
    // RenderTarget 생성
    RenderTarget = NewObject<UTextureRenderTarget2D>();
    RenderTarget->InitAutoFormat(1024, 1024);
    RenderTarget->UpdateResourceImmediate();

    CaptureComp->TextureTarget = RenderTarget;

    // 입력 받기
    EnableInput(GetWorld()->GetFirstPlayerController());

    InputComponent->BindAction("Capture", IE_Pressed, this, &ACaptureMapActor::CaptureAndSave);
}

// Called every frame
void ACaptureMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACaptureMapActor::CaptureAndSave()
{
    if (!CaptureComp || !RenderTarget) return;

    FVector CaptureCenter = CaptureComp->GetComponentLocation();
    FVector ActorLoc = GetActorLocation();
    FVector RelativeLoc = CaptureComp->GetRelativeLocation();

    UE_LOG(LogTemp, Warning, TEXT("===== Minimap Info ====="));
    UE_LOG(LogTemp, Warning, TEXT("Actor Location: %s"), *ActorLoc.ToString());
    UE_LOG(LogTemp, Warning, TEXT("Capture Relative Location: %s"), *RelativeLoc.ToString());
    UE_LOG(LogTemp, Warning, TEXT(">>> Capture Center (USE THIS): %s"), *CaptureCenter.ToString());
    UE_LOG(LogTemp, Warning, TEXT("========================"));

    // 캡처
    CaptureComp->CaptureScene();

    // 픽셀 읽기
    FTextureRenderTargetResource* RTResource =
        RenderTarget->GameThread_GetRenderTargetResource();

    TArray<FColor> Pixels;
    RTResource->ReadPixels(Pixels);

    // PNG 변환
    TArray<uint8> PNGData;
    FImageUtils::CompressImageArray(
        RenderTarget->SizeX,
        RenderTarget->SizeY,
        Pixels,
        PNGData
    );

    // 저장
    FString FilePath =
        FPaths::ProjectSavedDir() + TEXT("SavedMap.png");

    FFileHelper::SaveArrayToFile(PNGData, *FilePath);

    UE_LOG(LogTemp, Warning, TEXT("Saved to: %s"), *FilePath);
}