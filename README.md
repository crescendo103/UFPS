# 🎯 [UFPS] - Unreal Engine FPS 게임

> 배틀그라운드 모티브 1v1 팀 데스매치 기반의 멀티플레이어 FPS 게임

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.x-313131?logo=unrealengine)](https://www.unrealengine.com/)
[![Language](https://img.shields.io/badge/Language-C++-blue?logo=cplusplus)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)](#)

<br>

## 📌 목차

- [📖 프로젝트 개요](#-프로젝트-개요)
- [🕹️ 클라이언트 (Client)](#️-클라이언트-client)
  - [주요 기능](#주요-기능)  
  - [주요 클라 구조](#주요-클라-구조)
- [🖥️ 서버 (Server)](#️-서버-server)
  - [주요 기능](#주요-기능-1)
  - [서버 구조](#서버-구조)
- [🚀 실행 영상](#-실행-방법)

<br>

## 📖 프로젝트 개요

| 항목 | 내용 |
|---|---|
| 장르 | FPS (1~3인칭 슈팅게임) |
| 엔진 | Unreal Engine 5.4 |
| 개발 인원 | 1명 |
| 개발 기간 | 2026.01 ~ 2026.06 |
| 플레이 인원 | 1 vs 1 를 가정하고 만들 었습니다. |

언리얼 엔진을 처음 공부하면서 학부과정에 배운 소켓 프로그래밍을 적용하여 만든 개인 프로젝트입니다.

<br>

---

## 🕹️ 클라이언트 (Client)

클라이언트 파트에 대한 간단한 설명을 작성하세요. (예: 캐릭터 조작, 전투, UI 등 플레이어가 직접 체감하는 모든 요소 담당)

### 주요 기능

- **게임 시작 전 UI**: 플레이어 이름 설정, 게임 세션 관리 등 실제 게임에 가까운 시작 화면 UI 구현
- **무기 시스템**: 무기 사용 로직과 VFX, SFX 관리
- **인벤토리 시스템**: 배틀그라운드 스타일 드래그&드롭 인벤토리 UI + 레이캐스팅 기반 아이템 관리 시스템
- **AI Patrol**: 임의의 위치로 이동하는 AI
- **차량**: 현재 탑승중인 클라이언트에서 서버로 위치 동기화 패킷 송신
- **자기장, 레드존**: 자기장 외부에 있을 시 피격 이펙트, 레드존 안에는 포탄 떨어지는 기능 추가

### 주요 클라 구조

```
Client/
├── MyCharacter/ # 플레이어
│   ├── GrenadeThrowCalculateCompo/        # 수류탄 날아가는 궤적 계산하는 컴포넌트
│   ├── CurrentWeapon /            # 현재 들고있는 무기 종류(Enum)
│   ├── CurrentGunWeapon /                # 현재 들고있는 무기 액터
│   └── SceneCaptureComponent2D/         # 미니맵 렌더링
├── WeaponActor/ # 무기
│   ├── WeaponUseComponent/ # 무기마다 스폰할 총알 로직 컴포넌트 (산탄총이면 총알을 여러개 스폰, 소총은 단발로 하나씩 스폰)
│   ├── EffectComponent/ # VFX, SFX 효과 연출 컴포넌트
│   ├── 
│   └── 
├── MyInventory/ # 인벤토리(아래는 다 UserWidget)
│   ├── FloorItemBox/ # 플레이어와 가까운 거리에 있는 WeaponActor 표시
│   ├── HaveWidget/ # 플레이어가 소유하고 있는 WeaponActor 표시
│   ├── WeaponBoxPlayerWidget # 플레이어의 주무기에 해당하는 WeaponActor 표시
│   └── WeaponBoxWidget # 플레이어의 보조무기(수류탄)에 해당하는 WeaponActor 표시
│   └── TrashWidget # 드래그 드랍하여 WeaponActor를 버리는 UI 표시
└── README.md
```

[⬆ 목차로 돌아가기](#-목차)

<br>

---

## 🖥️ 서버 (Server)

(Winsock)소켓 기반의 서버입니다. 클라이언트와 직접 TCP 소켓으로 통신하며 패킷 구조체를 정의해 송수신합니다.
로비 → 방 생성/참가 → 게임 시작 흐름을 직접 구현했고, 캐릭터 위치・총알・수류탄・아이템・차량 등의 동기화와 AI 이동 로직을 서버에서 처리합니다.

<p align="center">
<img width="933" height="593" alt="서버 구조도" src="https://github.com/user-attachments/assets/972f68e5-693e-4cc8-8eb4-03c20352dd06" />
</p>

### 주요 기능

- **서버 구조**: 서버 데이터 처리용 쓰레드 1(ProcessThread), N(클라이언트 수, 수신용), 1(SendLoop, 송신용)로 구성됨
- **게임 로비(게임시작 전)**: `SessionManager`가 로비 클라이언트 목록을 관리하며, `CreateRoom` / `JoinRoom` / `StartGame` / `RemoveRoom` 패킷을 처리해 방 단위로 `GameSession`을 생성하고 배정
- **게임 로직 동기화**: 클라이언트로부터 받은 캐릭터 위치, 총알, 근접 공격, 수류탄, 사망 판정 등을 서버가 받아 큐에 적재 후 일괄 브로드캐스트
- **패킷 전송**: `GameSession::SendLoop()`에서 8ms 주기로 캐릭터 위치, 총알, 차량, 수류탄, AI, 타이머, 사망, 아이템 정보를 각 방에 접속한 클라이언트들에게 전송
- **AI 이동 처리**: `AIEnemyPositionManager`가 CSV로 미리 저장된 NavMesh 좌표들을 읽어, AI가 목적지에 도착(`IsJump`)하면 다음 랜덤 목적지를 큐에서 꺼내 갱신
- **DB 연동**: 미사용 (현재 유저 정보/전적/랭킹은 서버 메모리 상에서만 관리)

### 서버 구조

```
Server/
├── 소스.cpp(main)                    # 소켓 통신 연결
│   ├── AIEnemyPositionManager.h,cpp  # CSV파일에 존재하는 임의의 좌표값 읽기(AI 목표 위치 설정에 사용)
│   ├── SessionManager.h              # GameSession이 생성되기 전 즉 게임이 시작되기전 로비에서 처리 로직
│   ├── GameSession.h,cpp             # 게임에 대한 패킷처리
└── Common.h                          # 구조체, 변수 선언

### 실행 영상, 기술 문서
https://youtu.be/pb_JKvnfxuQ
[기술문서.pdf](https://github.com/user-attachments/files/29489303/default.pdf)
