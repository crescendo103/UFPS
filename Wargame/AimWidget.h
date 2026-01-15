// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"

#include <Components/ProgressBar.h>
#include "Components/CanvasPanelSlot.h"

#include "AimWidget.generated.h"


/**
 * 
 */
UCLASS()
class WARGAME_API UAimWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 크로스헤어 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
    float CurrentGap = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
    float TargetGap = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
    float DefaultGap = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
    float InterpSpeed = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
    bool gap = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hp")
    float CurrentHp = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hp")
    float MaxHp = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hp")
    float Percent = 100.f;


    UPROPERTY(Transient)
    UCanvasPanelSlot* Left_crosshairSlote;
    UPROPERTY(Transient)
    UCanvasPanelSlot* Right_crosshairSlot;
    UPROPERTY(Transient)
    UCanvasPanelSlot* Top_crosshairSlot;
    UPROPERTY(Transient)
    UCanvasPanelSlot* Bottom_crosshairSlot;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ProgressBar;

    // C++에서 호출할 함수
    void SetTargetGap(float NewGap);
    void SetAim(bool state);
    void SetCurrentHp(float amount);

protected:
    virtual void NativeConstruct() override;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


    UPROPERTY(meta = (BindWidget))
    UBorder* Left_crosshair;

    UPROPERTY(meta = (BindWidget))
    UBorder* Right_crosshair;

    UPROPERTY(meta = (BindWidget))
    UBorder* Top_crosshair;

    UPROPERTY(meta = (BindWidget))
    UBorder* Bottom_crosshair;
};
