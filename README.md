# Pico Launcher-KR
이 저장소는 [Pico Loader](https://github.com/LNH-team/pico-loader)의 프런트엔드인 Pico Launcher를 포함하고 있습니다. 비공식 한글, 일본어 패치가 적용되어 있습니다.

![Horizontal display mode with custom theme](docs/images/HorizontalCustom.png)
![Banner list display mode](docs/images/List.png)
![Coverflow display mode](docs/images/Coverflow.png)

## 주요 기능
- [Pico Loader](https://github.com/LNH-team/pico-loader)를 사용하여 홈브류 및 정식 발매 게임을 실행할 수 있습니다.
- 다양한 디스플레이 모드 지원
    - 가로 및 세로 아이콘 그리드
    - 배너 리스트
    - 커버플로우(Coverflow)
    - 테마, 언어 변경 기능
- [파일 연결](docs/FileAssociations.md) 지원
- [커버 이미지](docs/Covers.md) 지원
- [Material Design 3 및 커스텀 테마](docs/Themes.md) 지원
- 배경 음악 지원 ([테마](docs/Themes.md) 참조)
- 치트 지원 ([치트](docs/Cheats.md) 참조)

일반적인 사용법 문서는 여기에서 확인하실 수 있습니다: [사용법](docs/Usage.md).

## 설정 및 구성
이 저장소를 컴파일하려면 WSL(Windows Subsystem for Linux) 또는 MSYS2 사용을 권장합니다.
아래 단계는 해당 환경 중 하나가 이미 설정되어 있다고 가정합니다.

1. [BlocksDS](https://blocksds.skylyrac.net/docs/setup/options/)를 설치해 주십시오.

## 컴파일 방법

1. `make` 명령어를 실행합니다.

컴파일된 런처는 루트 디렉토리에서 `LAUNCHER.nds`라는 이름으로 확인하실 수 있습니다.

2. `LAUNCHER.nds` 파일을 SD 카드로 복사해 주십시오.
    - DSpico를 사용 중인 경우, 파일 이름을 `_picoboot.nds`로 변경하여 SD 카드 루트에 배치하십시오.
3. `_pico` 폴더를 SD 카드 루트로 복사해 주십시오.

> [!NOTE]
> Pico Launcher를 사용하려면 Pico Loader 파일들(`aplist.bin`, `patchlist.bin`. `savelist.bin`, `picoLoader7.bin`, `picoLoader9.bin`)이 SD 카드의 `/_pico` 폴더 내에 반드시 존재해야 합니다.

DSpico의 최종 디렉토리 구조는 다음과 같습니다:
```
.
├── _pico
│   ├── themes
│   │   ├── material
│   │   │   └── theme.json
│   │   └── raspberry
│   │       ├── bannerListCell.bin
│   │       ├── bannerListCellPltt.bin
│   │       ├── bannerListCellSelected.bin
│   │       ├── bannerListCellSelectedPltt.bin
│   │       ├── bottombg.bin
│   │       ├── gridcell.bin
│   │       ├── gridcellPltt.bin
│   │       ├── gridcellSelected.bin
│   │       ├── gridcellSelectedPltt.bin
│   │       ├── scrim.bin
│   │       ├── scrimPltt.bin
│   │       ├── theme.json
│   │       └── topbg.bin
│   ├── aplist.bin
│   ├── savelist.bin
│   ├── picoLoader7.bin
│   └── picoLoader9.bin
└── _picoboot.nds
```

참고: DSpico에서 DSiWare를 플레이하려면 추가 파일이 필요합니다. 자세한 내용은 [Pico Loader](https://github.com/LNH-team/pico-loader) Readme를 참조해 주십시오.

## 라이선스

아이콘 제공: [icons8](https://icons8.com/)

이 프로젝트는 Zlib 라이선스에 따라 라이선스가 부여됩니다. 자세한 내용은 `LICENSE.txt`를 참조해 주십시오.

프로젝트에 추가 라이선스가 적용될 수 있습니다. 자세한 내용은 `license` 디렉토리를 참조해 주십시오.

## 기여자
- [@Gericom](https://github.com/Gericom)
- [@XLuma](https://github.com/XLuma)
- [@Dartz150](https://github.com/Dartz150)
- [@lifehackerhansol](https://github.com/lifehackerhansol)