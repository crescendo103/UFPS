// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"



#include "MyCharacter.h"
#include "PawnManager.generated.h"


/**
 * 
 */
class AHelicopter;

UCLASS()
class FPS_API UPawnManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	

public: 
	void RegisterPlayer(AMyCharacter* player);
	void PossesHelicopter();
	void PossesLocalPlayer();
	void SpawnHeli();
	void SetHeliClass(TSubclassOf<AHelicopter> heli);
	void SetParachuteClass(TSubclassOf<AActor> actor);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AMyCharacter* LocalPlayer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AHelicopter* Helicopter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heli")
	TSubclassOf<AHelicopter> HeliClass;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* Parachute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heli")
	TSubclassOf<AActor> ParachuteClass;
};
