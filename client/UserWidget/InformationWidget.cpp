// Fill out your copyright notice in the Description page of Project Settings.


#include "InformationWidget.h"

#include "Components/TextBlock.h"

void UInformationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (InformaitonText)
    {
        InformaitonText->SetText(FText::FromString(TEXT("")));
    }
}

void UInformationWidget::UpdateInformationUI(FString information)
{
    FString Message = information;

    if (InformaitonText)
    {
        InformaitonText->SetText(FText::FromString(Message));
    }

    if (FadeAnim)
    {
        PlayAnimation(FadeAnim);
    }
}
