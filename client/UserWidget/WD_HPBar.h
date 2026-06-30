// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/ProgressBar.h>

#include "WD_HPBar.generated.h"


/**
 * 
 */
UCLASS()
class FPS_API UWD_HPBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hp")
	float CurrentHp = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hp")
	float MaxHp = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hp")
	float Percent = 100.f;


	bool SettHpBar(float amount);

	virtual void NativeConstruct() override;
};
