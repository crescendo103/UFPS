#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

#include "MyAudioComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API  UMyAudioComponent : public UAudioComponent
{
    GENERATED_BODY()

public:
    UMyAudioComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* EnterSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* LoopSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* ExitSound;

    UFUNCTION(BlueprintCallable)
    void StartLoop();

    UFUNCTION(BlueprintCallable)
    void StopLoop();

    UFUNCTION()
    void StartMiddleSound();

    FTimerHandle EngineStartHandle;
};