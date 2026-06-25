// AudioManager.cpp
#include "AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "GameConfigData3.h"

void UAudioManager::Init(UGameConfigData3* Config3)
{
	IntroBGM = Config3->IntroBGM;
	MenuBGM = Config3->MenuBGM;
	GameBGM = Config3->GameBGM;
}


void UAudioManager::PlayIntroBGM()
{
	CrossfadeBGM(IntroBGM);
}

void UAudioManager::PlayMenuBGM()
{
	CrossfadeBGM(MenuBGM);
}

void UAudioManager::PlayGameBGM()
{
	CrossfadeBGM(GameBGM);
}

void UAudioManager::CrossfadeBGM(USoundBase* NewSound, float FadeDuration)
{
	if (!NewSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AudioManager] CrossfadeBGM: NewSound is null"));
		return;
	}

	// 이미 같은 곡이 재생 중이면 다시 틀지 않음
	if (BGMComponent && BGMComponent->IsPlaying() && BGMComponent->Sound == NewSound)
	{
		return;
	}

	// 기존 곡 페이드아웃 후 정리 (bAutoDestroy가 true라 FadeOut 끝나면 자동 파괴됨)
	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->FadeOut(FadeDuration, 0.0f);
	}

	// 새 곡 생성 및 페이드인
	BGMComponent = UGameplayStatics::SpawnSound2D(
		this,
		NewSound,
		1.0f,   // VolumeMultiplier
		1.0f,   // PitchMultiplier
		0.0f,   // StartTime
		nullptr,// ConcurrencySettings
		true,   // bPersistAcrossLevelTransition
		true    // bAutoDestroy
	);

	if (BGMComponent)
	{
		BGMComponent->FadeIn(FadeDuration);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AudioManager] Failed to spawn BGM component"));
	}
}