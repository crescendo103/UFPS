#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "ItemEffectComponent.generated.h"

class UAudioComponent;
class USphereComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UItemEffectComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UItemEffectComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:

	//--------------------------------------------------
	// Niagara
	//--------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* StartEffect;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* DestroyEffect;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* OverlapEffect;

	//--------------------------------------------------
	// Sound
	//--------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* StartSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* DestroySound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* OverlapSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* LoopSound;

	//--------------------------------------------------
	// Overlap
	//--------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Overlap")
	float OverlapRadius = 200.f;

	//¼Ò¸®°¨¼è
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundAttenuation* SoundAttenuation;


private:

	UPROPERTY()
	UAudioComponent* LoopAudioComponent;

	UPROPERTY()
	USphereComponent* OverlapSphere;

public:

	UFUNCTION(BlueprintCallable)
	void PlayStartEffect();

	UFUNCTION(BlueprintCallable)
	void PlayDestroyEffect();

	UFUNCTION(BlueprintCallable)
	void PlayStartSound();

	UFUNCTION(BlueprintCallable)
	void PlayDestroySound();

	UFUNCTION(BlueprintCallable)
	void StartLoopSound();

	UFUNCTION(BlueprintCallable)
	void StopLoopSound();
};