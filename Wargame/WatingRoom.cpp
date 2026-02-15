// Fill out your copyright notice in the Description page of Project Settings.


#include "WatingRoom.h"

void UWatingRoom::SetInformationText(const FText& InText)
{
    if (InformationText)
    {
        InformationText->SetText(InText);
    }
}
