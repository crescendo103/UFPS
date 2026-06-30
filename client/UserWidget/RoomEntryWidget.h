#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomEntryWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnJoinRoomClicked,
    int32,
    RoomId);

UCLASS()
class FPS_API URoomEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void SetRoomInfo(
        int32 InRoomId,
        const FString& InRoomName,
        int32 InPlayerCount);

    UPROPERTY(BlueprintAssignable)
    FOnJoinRoomClicked OnJoinRoomClicked;

    UFUNCTION()
    void UpdatePlayerCount(int32 NewCount);
protected:

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoomNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerCountText;

    UPROPERTY(meta = (BindWidget))
    UButton* JoinButton;

private:

    UFUNCTION()
    void OnJoinClicked();

    int32 RoomId;
};