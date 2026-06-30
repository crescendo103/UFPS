#include "ItemEffectComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundAttenuation.h"

UItemEffectComponent::UItemEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OverlapSphere =	CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(this);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UItemEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (!Owner)
		return;

}

void UItemEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PlayDestroyEffect();
	PlayDestroySound();
	Super::EndPlay(EndPlayReason);
}

void UItemEffectComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	if (OverlapSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			OverlapSound,
			GetComponentLocation(),
			FRotator::ZeroRotator,
			1.f,
			1.f,
			0.f,
			SoundAttenuation);
	}

	if (OverlapEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			OverlapEffect,
			GetComponentLocation(),
			GetComponentRotation());
	}
}

void UItemEffectComponent::PlayStartEffect()
{
	if (!StartEffect)
		return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		StartEffect,
		GetComponentLocation(),
		GetComponentRotation());
}

void UItemEffectComponent::PlayDestroyEffect()
{
	if (!DestroyEffect)
		return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		DestroyEffect,
		GetComponentLocation(),
		GetComponentRotation());
}

void UItemEffectComponent::PlayStartSound()
{
	if (!StartSound)
		return;
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		StartSound,
		GetComponentLocation(),
		FRotator::ZeroRotator,
		1.f,
		1.f,
		0.f,
		SoundAttenuation);
}

void UItemEffectComponent::PlayDestroySound()
{
	if (!DestroySound)
		return;
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		DestroySound,
		GetComponentLocation(),
		FRotator::ZeroRotator,
		1.f,
		1.f,
		0.f,
		SoundAttenuation);
}

void UItemEffectComponent::StartLoopSound()
{
	if (!LoopSound)
		return;

	if (LoopAudioComponent)
		return;

	LoopAudioComponent = UGameplayStatics::SpawnSoundAttached(
			LoopSound,
			this,
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			false,
			1.f,
			1.f,
			0.f,
			SoundAttenuation);
	if (LoopAudioComponent)
	{
		LoopAudioComponent->Play();
	}
}

void UItemEffectComponent::StopLoopSound()
{
	if (!LoopAudioComponent)
		return;
	LoopAudioComponent->Stop();
	LoopAudioComponent = nullptr;
}