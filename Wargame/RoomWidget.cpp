#include "RoomWidget.h"
#include "RoomEntryWidget.h"
#include "MyServer.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"        // UButton
#include "Components/EditableTextBox.h" // RoomNameInputBox
#include "Engine/World.h"
#include "CustomPlayerController.h"


void URoomWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CreateRoomButton)
    {
        CreateRoomButton->OnClicked.AddDynamic(
            this,
            &URoomWidget::OnCreateRoomClicked);
    }
}

// 방 생성 버튼 클릭 시 호출 (버튼에 바인딩)
void URoomWidget::OnCreateRoomClicked()
{
    UMyServer* Server = GetGameInstance()->GetSubsystem<UMyServer>();
    if (!Server) return;

    // 서버에 방 생성 요청만 보냄 - UI는 서버 응답 오면 그때 추가
    FRoomPacket Packet;
    Packet.Header.Size = sizeof(FRoomPacket);
    Packet.Header.Type = static_cast<int>(EPacketType::Room);
    Packet.RoomType = ERoomPacketType::CreateRoom;
    Packet.RoomId = -1;
    Packet.PlayerCount = -1;
    Packet.HostId = -1;

    // 방 이름은 입력창에서 가져오기
    FString RoomNameStr = RoomNameInputBox->GetText().ToString();
    memset(Packet.RoomName, 0, sizeof(Packet.RoomName));
    // TCHAR_TO_ANSI 대신 FTCHARToUTF8 사용
    FTCHARToUTF8 Converted(*RoomNameStr);
    memcpy(Packet.RoomName, Converted.Get(),
        FMath::Min(Converted.Length(), 15));
    Packet.RoomName[15] = '\0';
    Server->MoveRoomPacket(Packet);


    ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetWorld()->GetFirstPlayerController());
    PC->HiddenRoomUI();
    PC->ShowHiddenStartMenu();
}

// 방 목록 요청 (로비 화면 열릴 때 호출)
void URoomWidget::RequestRoomList()
{
    UMyServer* Server = GetGameInstance()->GetSubsystem<UMyServer>();
    if (!Server) return;

    FRoomPacket Packet;
    Packet.Header.Size = sizeof(FRoomPacket);
    Packet.Header.Type = static_cast<int>(EPacketType::Room);
    Packet.RoomType = ERoomPacketType::RoomList;
    Packet.RoomId = -1;
    Packet.PlayerCount = -1;
    Packet.HostId = -1;

    Server->MoveRoomPacket(Packet);
}



// 서버 응답으로 방 목록에 항목 추가 (ProcessRoomPacket에서 호출)
void URoomWidget::AddRoom(int32 RoomId, const FString& RoomName, int32 PlayerCount)
{
    if (RoomMap.Contains(RoomId)) return;

    URoomEntryWidget* Entry =
        CreateWidget<URoomEntryWidget>(GetWorld(), RoomEntryClass);
    if (!Entry) return;

    Entry->SetRoomInfo(RoomId, RoomName, PlayerCount);
    Entry->OnJoinRoomClicked.AddDynamic(this, &URoomWidget::JoinRoom);
    RoomScrollBox->AddChild(Entry);
    RoomMap.Add(RoomId, Entry);
}

void URoomWidget::RemoveRoom(int32 RoomId)
{
    URoomEntryWidget** Found = RoomMap.Find(RoomId);
    if (!Found) return;

    RoomScrollBox->RemoveChild(*Found);
    RoomMap.Remove(RoomId);
}
// 방 참가 버튼 클릭 시 호출
void URoomWidget::JoinRoom(int32 RoomId)
{
    UMyServer* Server = GetGameInstance()->GetSubsystem<UMyServer>();
    if (!Server) return;

    FRoomPacket Packet;
    Packet.Header.Size = sizeof(FRoomPacket);
    Packet.Header.Type = static_cast<int>(EPacketType::Room);
    Packet.RoomType = ERoomPacketType::JoinRoom;
    Packet.RoomId = RoomId;
    Packet.PlayerCount = -1;
    Packet.HostId = -1;

    Server->MoveRoomPacket(Packet);

    ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetWorld()->GetFirstPlayerController());
    PC->HiddenRoomUI();
    PC->ShowHiddenStartMenu();
    PC->SetDisableStartButton();
}

// 인원수 업데이트
void URoomWidget::UpdateRoomPlayerCount(int32 RoomId, int32 PlayerCount)
{
    URoomEntryWidget** Found = RoomMap.Find(RoomId);
    if (!Found) return;
    (*Found)->UpdatePlayerCount(PlayerCount);
}
