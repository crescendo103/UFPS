// Fill out your copyright notice in the Description page of Project Settings.


#include "WatingRoom.h"

void UWatingRoom::ClearInformationText()
{
    if (TitleInformaitonText)
    {
        TitleInformaitonText->SetText(FText::GetEmpty());
    }

    if (InformationText)
    {
        InformationText->SetText(FText::GetEmpty());
    }
}

void UWatingRoom::SetInformationText(const FText& InText)
{
    if (InformationText)
    {
        InformationText->SetText(InText);
    }
}
