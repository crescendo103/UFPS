#include "MyAudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


UMyAudioComponent::UMyAudioComponent()
{
    bAutoActivate = false;
}

void UMyAudioComponent::StartLoop()
{
    if (EnterSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            EnterSound,
            GetComponentLocation());

        GetWorld()->GetTimerManager().SetTimer(
            EngineStartHandle,
            this,
            &UMyAudioComponent::StartMiddleSound,
            EnterSound->GetDuration(),
            false);
    }
    else
    {
        if (LoopSound)
        {
            SetSound(LoopSound);
            Play();
        }
    }
}

void UMyAudioComponent::StopLoop()
{
    Stop();

    if (ExitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ExitSound,
            GetComponentLocation());
    }
}

void UMyAudioComponent::StartMiddleSound()
{
    if (LoopSound)
    {
        SetSound(LoopSound);
        Play();
    }
}
