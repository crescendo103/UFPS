#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameUIWidget.generated.h"

class UTextBlock;
class UButton;
class UWidgetAnimation;

UCLASS()
class FPS_API UEndGameUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	// UI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Message1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Message2;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_MainMenu;

	// Animations (UMG에서 만든 애니메이션 바인딩)
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_Message1_Fade;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_Message2_Fade;

	

	UFUNCTION()
	void OnMainMenuClicked();

public:
	void UpdateEndGameUI(FString WinLoseText, FString KillerName);
	// 함수
	void PlaySequence();
};