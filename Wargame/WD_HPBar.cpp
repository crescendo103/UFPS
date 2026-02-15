// Fill out your copyright notice in the Description page of Project Settings.


#include "WD_HPBar.h"


void UWD_HPBar::SettHpBar(float amount)
{

    CurrentHp -= amount;
    if (CurrentHp < 0) {
        CurrentHp = 0;
    }

    Percent = CurrentHp / MaxHp;
    ProgressBar->SetPercent(Percent);

}

void UWD_HPBar::NativeConstruct()
{
    Super::NativeConstruct();

    CurrentHp = 100.f;
    MaxHp = 100.f;
    Percent = 1.f;
    if (ProgressBar)
    {
        ProgressBar->SetPercent(Percent);
    }
}
