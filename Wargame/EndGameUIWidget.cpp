#include "EndGameUIWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UEndGameUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	

	// 버튼 이벤트
	if (Btn_MainMenu)
	{
		Btn_MainMenu->OnClicked.AddDynamic(this, &UEndGameUIWidget::OnMainMenuClicked);
	}

	// 연출 시작
	//PlaySequence();
}

void UEndGameUIWidget::PlaySequence()
{
	// 1번 메시지 애니 재생
	if (Anim_Message1_Fade)
	{
		PlayAnimation(Anim_Message1_Fade);
	}

	// 2번 메시지는 딜레이 후 실행
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			if (Anim_Message2_Fade)
			{
				PlayAnimation(Anim_Message2_Fade);
			}
		},
		2.0f,
		false
	);
}

void UEndGameUIWidget::OnMainMenuClicked()
{
	//UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UEndGameUIWidget::UpdateEndGameUI(FString WinLoseText, FString KillerName)
{
	FString Message = TEXT(" You killed by") + KillerName;

	if (Text_Message1)
	{
		Text_Message1->SetText(FText::FromString(WinLoseText));
	}

	if (Text_Message2)
	{
		Text_Message2->SetText(FText::FromString(Message));
	}

	UE_LOG(LogTemp, Warning, TEXT("Text1 Value = %s"),
		*Text_Message1->GetText().ToString());

	UE_LOG(LogTemp, Warning, TEXT("Text2 Value = %s"),
		*Text_Message2->GetText().ToString());
}