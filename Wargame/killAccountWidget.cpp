// Fill out your copyright notice in the Description page of Project Settings.


#include "killAccountWidget.h"
#include "Components/TextBlock.h"

void UkillAccountWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (DeathText)
    {
        DeathText->SetText(FText::FromString(TEXT("")));
    }
}

void UkillAccountWidget::UpdateDeathUI(FString killerName, FString victimName)
{
    FString Message = killerName + TEXT(" killed ") + victimName;

    if (DeathText)
    {
        DeathText->SetText(FText::FromString(Message));
    }

    if (FadeAnim)
    {
        PlayAnimation(FadeAnim);
    }
}
