#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomWidget.generated.h"

class URoomEntryWidget;
class UScrollBox;
class UButton;
class UEditableTextBox;

UCLASS()
class FPS_API URoomWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
   
        

    void AddRoom(
        int32 RoomId,
        const FString& RoomName,
        int32 PlayerCount);

    void RemoveRoom(int32 RoomId);

    UFUNCTION()
    void OnCreateRoomClicked();

    

    UFUNCTION()
    void RequestRoomList();

    UFUNCTION()
    void UpdateRoomPlayerCount(int32 RoomId, int32 PlayerCount);
        

    int32 CurrentRoomId = -1; // 현재 참가한 방 ID

    
protected:
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* RoomNameInputBox;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* RoomScrollBox;

    UPROPERTY(meta = (BindWidget))
    UButton* CreateRoomButton;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<URoomEntryWidget> RoomEntryClass;

public:

    UPROPERTY()
    TMap<int32, URoomEntryWidget*> RoomMap;

    UFUNCTION()
    void JoinRoom(int32 RoomId);
};