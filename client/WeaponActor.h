// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "ItemEffectComponent.h"
#include "SpawnItemActor.h"
#include "HighlightInterface.h"
#include "WeaponUseComponent.h"

#include "WeaponActor.generated.h"

class AWeaponCompo;
class AMyCharacter;

UCLASS(Blueprintable)
class FPS_API AWeaponActor : public AActor, public IHighlightInterface, public ISpawnItemActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshy")
	USkeletalMeshComponent* Mesh;

	// 구체 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	class USphereComponent* TriggerSphere;	
	
	UPROPERTY(EditDefaultsOnly, Category = "ItemName")
	int32 ItemRowName;
	int32 GetItemName();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void ItemOutlineOn_Implementation() override;//아이템 하이라이트 켜기
	virtual void ItemOutlineOff_Implementation() override;//아이템 하이라이트 끄기	

	UPROPERTY()
	bool bPickedUp;
	
	USphereComponent* GetTriggerSphere() const { return TriggerSphere; }	

	//----총기 스고프 부착 관련 된건데 미완성--------
	AWeaponCompo* myGunCompo;
	void SetMyGunCompo(AWeaponCompo* compo);
	AWeaponCompo* GetMyGunCompo();
	//---------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "feature")
	UItemEffectComponent* EffectComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "feature")
	UWeaponUseComponent* SpawnComponent;
	
	void SpawnItem(FVector startPos, FVector startRot);

	void SetMyOwner(AMyCharacter* myowner);
	AMyCharacter* Owner;

	UPROPERTY(EditDefaultsOnly, Category = "ItemID")
	int32 ItemSpawnID=0;

	void sendItemPacket();
	int32 GetItemSpawnID();		
};
