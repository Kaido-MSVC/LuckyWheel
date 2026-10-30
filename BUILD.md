# LuckyWheel v2.0.8 建置與封裝

## 本機確認的工具

C++17、Qt Widgets 6.12.0（MSVC x64 kit）、Visual Studio Community 2026、
MSVC 14.44.35207（cl 19.44.35229）、CMake 4.4.4、Ninja 1.12.1、NSIS 3.13、Windows SDK 10.0.28000.0。
Qt 上游 DLL 的編譯器為 19.44.35227，詳見 SBOM；不是本程式的 cl 版本。
必要 Qt 元件：Core、Gui、Widgets、Sql、Multimedia、Test。

## 建置與測試

在原始碼根目錄執行 `build.cmd`。腳本選擇已安裝的 VS、MSVC 14.44、Qt 6.12.0，
以 CMake/Ninja 建立 Release，執行核心與 UI 的 CTest，通過後才部署 `build/dist/`。
不安裝、升級或修復任何工具；缺少工具即停止。

預設 Qt 位於 `D:\Qt\6.12.0\msvc2022_64`，可用 `LW_QT_ROOT`、`LW_NINJA`、
`LW_BUILD_DIR`、`LW_CRT_ROOT` 覆寫。建置目錄不要使用磁碟根目錄、原始碼目錄或資料目錄。
CMake cache 綁定原始碼位置，移動來源後應另選新的建置目錄。

如需直接使用 CMake，先載入 x64 MSVC 14.44 開發環境，並將 Qt bin 加入 PATH：

```bat
cmake --preset msvc-release
cmake --build --preset msvc-release
ctest --preset msvc-release
```

CTest 的 UI 使用 offscreen。原生 Windows 回歸可執行：

```bat
set QT_QPA_PLATFORM=windows
build\ui_tests.exe -o build\ui-native-results.txt,txt
```

測試使用暫存資料庫，不使用正式資料。設定 `LW_SCREENSHOT_DIR` 可輸出 UI 截圖。

## 封裝與來源交付

```powershell
./package.ps1 -OutputDir packages/prepublish-20261030
# 只有剛完成同一份來源的 build.cmd，才可略過重建：
./package.ps1 -SkipBuild -OutputDir packages/prepublish-20261030
```

產生 Portable ZIP、NSIS Setup、source ZIP 與 `SHA256SUMS.txt`。
來源清單為 src、tests、assets、examples、installer、licenses 與必要根目錄文件；
不把 runtime、packages、build、legacy、validation、舊報告、私人資料塞進 source ZIP。
`-OutputDir` 可選其他輸出位置；既有同名輸出會拒絕覆寫，避免破壞前次交付。
預設沒有 `-Candidate` 時，若 RELEASE-STATUS.md 仍含 NO-GO 則停止。
`-Validation` 沿用原本獨立登錄與捷徑的安裝驗證模式，不影響正式安裝登錄。

安裝程式依實際 payload 生成逐檔解除安裝清單，不遞迴清除使用者資料。
SDK、CRT 採既有可轉散布來源，不執行系統執行階段安裝器。
第三方來源 ZIP 必須事先存在 OutputDir，且內容、metadata、build-info 通過打包檢查；
隨後實際嵌入 Portable／Setup 的 sources/。它不放進自有 source ZIP。

完整本機交付在 packages/prepublish-20261030，尚未上傳 GitHub。
需要更新同名交付時才明確加 -Refresh；只更新本版指定輸出，不建立新的副本目錄。
發布前檢查 RELEASE-STATUS.md，不得僅因編譯／封裝成功即宣稱授權合規。

