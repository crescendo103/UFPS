// Fill out your copyright notice in the Description page of Project Settings.


#include "AimWidget.h"

void UAimWidget::SetTargetGap(float NewGap)
{
	TargetGap = NewGap;
}

void UAimWidget::SetAim(bool state)
{
    gap = state;    
}

void UAimWidget::SetCurrentHp(float amount)
{

    CurrentHp -= amount;
    if (CurrentHp < 0) {
        CurrentHp = 0;
    }

    Percent = CurrentHp / MaxHp;
    ProgressBar->SetPercent(Percent);
}




void UAimWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // BeginPlay처럼 초기화 코드
    Left_crosshairSlote = Cast<UCanvasPanelSlot>(Left_crosshair->Slot);
    Right_crosshairSlot = Cast<UCanvasPanelSlot>(Right_crosshair->Slot);
    Top_crosshairSlot = Cast<UCanvasPanelSlot>(Top_crosshair->Slot);
    Bottom_crosshairSlot = Cast<UCanvasPanelSlot>(Bottom_crosshair->Slot);
}

void UAimWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (gap) {
        // 부드럽게 벌어지게
        CurrentGap = FMath::FInterpTo(CurrentGap, TargetGap, InDeltaTime, InterpSpeed);//보간    

        Left_crosshairSlote->SetPosition(FVector2D(-CurrentGap, 0));
        Right_crosshairSlot->SetPosition(FVector2D(CurrentGap, 0));
        Top_crosshairSlot->SetPosition(FVector2D(0, -CurrentGap));
        Bottom_crosshairSlot->SetPosition(FVector2D(0, CurrentGap));
    }
    else {
        // 부드럽게 벌어지게
        CurrentGap = FMath::FInterpTo(CurrentGap, DefaultGap, InDeltaTime, InterpSpeed);//보간    

        Left_crosshairSlote->SetPosition(FVector2D(-CurrentGap, 0));
        Right_crosshairSlot->SetPosition(FVector2D(CurrentGap, 0));
        Top_crosshairSlot->SetPosition(FVector2D(0, -CurrentGap));
        Bottom_crosshairSlot->SetPosition(FVector2D(0, CurrentGap));
    }
    

}
