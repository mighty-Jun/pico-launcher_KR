# 파일 연결 (File Associations)
Pico Launcher는 `.nds` 이외의 파일 확장자에 대해 사용자 지정 파일 연결을 지원합니다.
파일 연결을 설정하면, 지정된 확장자를 가진 파일을 특정 DS 애플리케이션을 통해 실행할 수 있습니다.
이때 파일 경로는 argv를 통해 전달됩니다.

## 파일 연결 설정 방법
1. `/_pico/settings.json` 파일을 엽니다.
2. 만약 해당 키가 없다면, `fileAssociations` 키를 추가해 주십시오.
3. 각 파일 연결에 대해 `fileAssociations` 키 내부에 항목을 추가합니다. 예시는 다음과 같습니다:
    ```json
    "fileAssociations": {
      "gb": {
        "appPath": "/Emulators/GB.nds"
      },
      "gbc": {
        "appPath": "/Emulators/GB.nds"
      },
      "gba": {
        "appPath": "/Emulators/gbar3-frontend.nds"
      },
    }
    ```
    이와 같이 설정하면 `.gb`, `.gbc` 파일은 `/Emulators/GB.nds`로 실행되며, `.gba` 파일은 `/Emulators/gbar3-frontend.nds`로 실행됩니다. 위 예시의 확장자 외에도 추가로 에뮬레이터를 설정할 수 있습니다.