# 🎯 [SS] - Unreal Engine FPS 게임

> 배틀그라운드 모티브 1v1 팀 데스매치 기반의 멀티플레이어 FPS 게임

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.x-313131?logo=unrealengine)](https://www.unrealengine.com/)
[![Language](https://img.shields.io/badge/Language-C++-blue?logo=cplusplus)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)](#)

<br>

## 📌 목차

- [📖 프로젝트 개요](#-프로젝트-개요)
- [🕹️ 클라이언트 (Client)](#️-클라이언트-client)
  - [주요 기능](#주요-기능)
  - [기술 스택](#기술-스택)
  - [폴더 구조](#폴더-구조)
- [🖥️ 서버 (Server)](#️-서버-server)
  - [주요 기능](#주요-기능-1)
  - [기술 스택](#기술-스택-1)
  - [폴더 구조](#폴더-구조-1)
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

- **캐릭터 무브먼트**: 걷기 / 달리기 / 점프 / 슬라이딩 / 웅크리기
- **무기 시스템**: 사격, 재장전, 무기 교체, 탄도 계산
- **애니메이션**: Animation Blueprint, Blend Space, Montage 기반 모션 처리
- **UI / HUD**: 체력바, 탄약 표시, 미니맵, 킬로그
- **사운드**: 발소리, 총성, 환경음 (Metasound / Sound Cue)
- **이펙트**: 머즐 플래시, 피격 이펙트, 파티클 (Niagara)

### 기술 스택

| 분류 | 사용 기술 |
|---|---|
| 엔진 | Unreal Engine 5.x |
| 언어 | C++, Blueprint |
| 애니메이션 | Animation Blueprint, Control Rig |
| 입력 | Enhanced Input System |
| 네트워크 동기화 | Replication, RPC |

### 폴더 구조

```
Client/
├── Source/
│   ├── Character/        # 플레이어 캐릭터, 무브먼트
│   ├── Weapon/            # 무기 베이스 및 개별 무기 클래스
│   ├── UI/                # HUD, 위젯 관련
│   └── Animation/         # AnimInstance, Notify
├── Content/
│   ├── Characters/
│   ├── Weapons/
│   ├── Maps/
│   └── UI/
└── README.md
```

[⬆ 목차로 돌아가기](#-목차)

<br>

---

## 🖥️ 서버 (Server)

서버 파트에 대한 간단한 설명을 작성하세요. (예: 데디케이트 서버 기반으로 매치 진행, 동기화, 판정 처리 담당)

### 주요 기능

- **서버 구조**: Dedicated Server / Listen Server 여부
- **매치메이킹**: 세션 생성 및 매칭 로직
- **게임 로직 동기화**: 데미지 판정, 히트박스 처리(서버 권위 방식)
- **리플리케이션**: 플레이어 상태, 무기, 스코어 동기화
- **DB 연동**: 유저 정보, 전적, 랭킹 저장 (사용 시)

### 기술 스택

| 분류 | 사용 기술 |
|---|---|
| 엔진 | Unreal Engine 5.x (Dedicated Server Build) |
| 언어 | C++ |
| 네트워크 | Unreal Replication, Steam Online Subsystem 등 |
| 데이터베이스 | (사용한다면 MySQL / MongoDB 등 기입) |
| 배포 환경 | (AWS, 온프레미스 등 기입) |

### 폴더 구조

```
Server/
├── Source/
│   ├── GameMode/          # 게임 모드, 라운드 로직
│   ├── PlayerState/       # 점수, 전적 등 상태 관리
│   ├── Network/           # 세션, 매치메이킹
│   └── Database/          # DB 연동 로직 (사용 시)
└── README.md
```

[⬆ 목차로 돌아가기](#-목차)

<br>

---

## 🚀 실행 방법

```bash
# 1. 저장소 클론
git clone https://github.com/username/repo-name.git

# 2. .uproject 파일 우클릭 -> Generate Visual Studio project files

# 3. 엔진 버전 확인 후 빌드 (Development Editor)
```

서버 실행 시 별도 빌드/실행 절차가 있다면 여기에 작성하세요.

<br>
