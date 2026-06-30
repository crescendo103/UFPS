#include "RoomEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void URoomEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(
            this,
            &URoomEntryWidget::OnJoinClicked);
    }
}

void URoomEntryWidget::SetRoomInfo(
    int32 InRoomId,
    const FString& InRoomName,
    int32 InPlayerCount)
{
    RoomId = InRoomId;

    if (RoomNameText)
    {
        RoomNameText->SetText(
            FText::FromString(InRoomName));
    }

    if (PlayerCountText)
    {
        PlayerCountText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("%d"),
                    InPlayerCount)));
    }
}

void URoomEntryWidget::OnJoinClicked()
{
    OnJoinRoomClicked.Broadcast(RoomId);
}

void URoomEntryWidget::UpdatePlayerCount(int32 NewCount)
{
    if (PlayerCountText)
    {
        PlayerCountText->SetText(
            FText::FromString(
                FString::Printf(TEXT("%d"), NewCount)));
    }
}