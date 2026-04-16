# 테마 (Themes)
테마를 사용하여 Pico Launcher의 외관과 느낌을 사용자 정의할 수 있습니다. 테마는 `/_pico/themes` 폴더의 하위 폴더에 배치합니다. 예: `/_pico/themes/my_theme`.

## JSON 파일
각 테마에는 테마 정보를 담고 있는 `theme.json` 파일이 있습니다.

- **type** - 테마의 유형입니다. 현재 `material`과 `custom` 유형을 지원합니다. 각 유형에 대한 자세한 내용은 아래를 참조하십시오.
- **name** - 테마의 이름입니다.
- **description** - 테마에 대한 설명입니다.
- **author** - 테마 제작자입니다.
- **primaryColor** - 사용할 'Material Design 3' 주 색상(Primary Color)입니다. `r`, `g`, `b` 값은 0-255 범위로 제공됩니다.
- **darkTheme** - `true`로 설정하면 어두운(Dark) 'Material Design 3' 팔레트가 사용됩니다.

### 예시
```json
{
    "type": "material",
    "name": "테마 이름",
    "description": "여기에 테마 설명을 입력하세요.",
    "author": "제작자 이름",
    "primaryColor": {
        "r": 149,
        "g": 143,
        "b": 237
    },
    "darkTheme": true
}
```

## Material 유형
![Material 유형 가로 디스플레이 모드](images/Horizontal.png)

`material` 유형은 순수 'Material Design 3' 테마입니다. `theme.json`에 설정된 `primaryColor`와 `darkTheme` 값을 기반으로 전체 테마가 적용됩니다. 커버플로우 모드에서 이 테마 유형은 'Material Design 3' 스타일의 캐러셀(Carousel)을 사용합니다.

## Custom 유형
![커스텀 테마 가로 디스플레이 모드](images/HorizontalCustom.png)

`custom` 유형은 `material` 유형에 비해 훨씬 더 자유로운 사용자 정의가 가능합니다.
다만, UI의 일부 요소에는 여전히 `theme.json`의 `primaryColor` 및 `darkTheme` 설정이 색상 적용에 사용된다는 점에 유의해 주십시오.

추가로 다음과 같은 파일들이 필요합니다:

| 파일 이름 | 크기 | 형식 | 설명 |
|--------------------------------------------------------------|----------------------|--------------------------|----------------------------------------------------------------|
| bannerListCell.bin<br>bannerListCellPltt.bin | 256x49 (209x49 사용) | A3I5<br>32색 팔레트 | 배너 리스트 모드에서 선택되지 않은 항목의 배경입니다. |
| bannerListCellSelected.bin<br>bannerListCellSelectedPltt.bin | 256x49 (209x49 사용) | A3I5<br>32색 팔레트 | 배너 리스트 모드에서 선택된 항목의 배경입니다. |
| bottombg.bin | 256x192 | 15 bpp 비트맵 | 하단 화면 배경입니다. |
| gridcell.bin<br>gridcellPltt.bin | 64x48 (48x48 사용) | A3I5<br>32색 팔레트 | 그리드 모드에서 선택되지 않은 항목의 배경입니다. |
| gridcellSelected.bin<br>gridcellPlttSelected.bin | 64x48 (48x48 사용) | A3I5<br>32색 팔레트 | 그리드 모드에서 선택된 항목의 배경입니다. |
| scrim.bin<br>scrimPltt.bin | 8x42 | A5I3<br>8색 팔레트 | 툴바용 배경입니다. 반투명한 페이드 효과를 의도한 설정입니다. |
| topbg.bin | 256x192 | 15 bpp 비트맵 | 상단 화면 배경입니다. |

이 파일들은 예를 들어 [NitroPaint](https://github.com/Garhoogin/NitroPaint)와 같은 도구를 사용하여 생성할 수 있습니다.

상단 화면 배경에는 선택된 항목의 배너 텍스트와 아이콘이 표시될 박스 영역이 포함되어야 합니다.

### 추가 JSON 속성 (Additional JSON properties)
커스텀 테마는 더욱 세밀한 사용자 정의를 위해 `theme.json` 파일에서 추가 속성을 지원합니다.

- **topIcon** - 상단 화면에 표시되는 아이콘의 속성입니다.
- **topBannerTextLine0** - 상단 화면에 표시되는 배너 텍스트의 첫 번째 줄 속성입니다.
- **topBannerTextLine1** - 상단 화면에 표시되는 배너 텍스트의 두 번째 줄 속성입니다.
- **topBannerTextLine2** - 상단 화면에 표시되는 배너 텍스트의 세 번째 줄 속성입니다.
- **topFileNameText** - 상단 화면에 표시되는 파일 이름 텍스트의 속성입니다.
- **topCover** - 상단 화면에 표시되는 커버 이미지의 속성.
- **gridIcon** - 그리드 디스플레이 모드 시 하단 화면에 표시되는 아이콘의 속성입니다.
- **bannerListIcon** - 배너 리스트 디스플레이 모드 시 하단 화면에 표시되는 아이콘의 속성입니다.
- **bannerListTextLine0** - 배너 리스트 디스플레이 모드 시 하단 화면에 표시되는 배너 텍스트의 첫 번째 줄 속성입니다.
- **bannerListTextLine1** - 배너 리스트 디스플레이 모드 시 하단 화면에 표시되는 배너 텍스트의 두 번째 줄 속성입니다.
- **bannerListTextLine2** - 배너 리스트 디스플레이 모드 시 하단 화면에 표시되는 배너 텍스트의 세 번째 줄 속성입니다.

혼합 색상(Blend colors)은 가짜 투명도(fake translucency)를 구현하는 데 사용됩니다. 이 값은 배경색과 가장 유사한 색상으로 설정해 주시기 바랍니다.

```json
{
    "type": "custom",
    "name": "Raspberry",
    "description": "Theme based on raspberries.",
    "author": "Gericom",
    "primaryColor": { "r": 138, "g": 217, "b": 255 },
    "darkTheme": false,
    "topIcon": {
        "position": { "x": 24, "y": 132 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topBannerTextLine0": {
        "position": { "x": 70, "y": 126 },
        "width": 176,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topBannerTextLine1": {
        "position": { "x": 70, "y": 141 },
        "width": 176,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topBannerTextLine2": {
        "position": { "x": 70, "y": 155 },
        "width": 176,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topFileNameText": {
        "position": { "x": 18, "y": 170 },
        "width": 220,
        "textColor": { "r": 30, "g": 30, "b": 30 },
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "topCover": {
        "position": { "x": 75, "y": 18 }
    },
    "gridIcon": {
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "bannerListIcon": {
        "blendColor": { "r": 200, "g": 200, "b": 200 }
    },
    "bannerListTextLine0": {
        "textColor": { "r": 30, "g": 30, "b": 30 }
    },
    "bannerListTextLine1": {
        "textColor": { "r": 30, "g": 30, "b": 30 }
    },
    "bannerListTextLine2": {
        "textColor": { "r": 30, "g": 30, "b": 30 }
    }
}
```

## 배경 음악 (BGM)
테마 폴더 내의 `bgm` 폴더에 DSP-ADPCM으로 인코딩된 `.bcstm` 파일을 배치하여 배경 음악을 재생할 수 있습니다. 반복 재생(Looping)이 지원됩니다. 여러 개의 `.bcstm` 파일이 있는 경우, Pico Launcher가 실행될 때마다 배경 음악이 무작위로 선택됩니다.
