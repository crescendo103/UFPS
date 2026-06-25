// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "MyServer.h"
#include "CustomPlayerController.h"
#include "AudioManager.h"


void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginClicked);

    introPlayer->OpenSource(introMediaSource);
    introPlayer->Play();
    introPlayer->OnEndReached.AddDynamic(this, &ULoginWidget::OnIntroFinished);

    introSoundComponent = NewObject<UMediaSoundComponent>(this);
    introSoundComponent->SetMediaPlayer(shareMediaPlayer);
    introSoundComponent->RegisterComponent();


    LoginButton->SetRenderOpacity(0.0f); // 완전 투명
    NameTextBox->SetRenderOpacity(0.0f);
    HandImg->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f)); // Alpha=0

}


void ULoginWidget::OnLoginClicked()
{
	UMyServer* MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    FString InName = NameTextBox->GetText().ToString();

    FConnectionPacket packet;
    packet.Header.Type = (int32)EPacketType::Client;
    packet.Header.Size = sizeof(FConnectionPacket);
    packet.order = -1;
    // 🔥 이름 안전하게 복사 (7글자 제한)
    //FString InName = "client1";
    FString SafeName = InName.Left(7);
    FTCHARToUTF8 Convert(*SafeName);  // TCHAR → UTF8(char)
    strncpy(packet.Name, Convert.Get(), sizeof(packet.Name) - 1);
    packet.Name[sizeof(packet.Name) - 1] = '\0';
    
    MyServer->MoveConnect(packet);
    SetVisibility(ESlateVisibility::Hidden);

    ACustomPlayerController* PC =
        Cast<ACustomPlayerController>(GetWorld()->GetFirstPlayerController());
    PC->ShowRoomUI();

    UAudioManager* AudioMgr = GetGameInstance()->GetSubsystem<UAudioManager>();
    if (AudioMgr) {
        AudioMgr->PlayMenuBGM();
    }

    UE_LOG(LogTemp, Error, TEXT("OnLoginClicked"));
}

void ULoginWidget::OnIntroFinished()
{
    PlayAnimation(FadeInUI);
}