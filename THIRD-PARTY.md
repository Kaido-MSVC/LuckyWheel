# 第三方元件與授權

LuckyWheel 動態連結 Qt 官方 6.12.1 Windows MSVC x64 套件提供的 Qt 與 FFmpeg DLL。實際 Qt 提交及二進位雜湊見 `licenses/source-provenance.json`。安裝版與免安裝 ZIP 隨附 `licenses/`。

| 隨包檔案 | 元件 | 授權 | `licenses/` 內的文件 |
|---|---|---|---|
| `Qt6Core/Gui/Widgets/Sql/Multimedia/Network/Svg.dll`、`platforms/`、`styles/`、`imageformats/`、`iconengines/`、`sqldrivers/`、`tls/`、`networkinformation/`、`multimedia/` 外掛 | Qt 6.12.1（qtbase、qtmultimedia、qtsvg、qtimageformats），The Qt Company Ltd. 及其貢獻者 | GNU LGPL v3（Qt 另有 GPL 與商業授權，本程式採 LGPL v3） | `LGPL-3.0.txt`、`GPL-3.0.txt`、`Qt-LICENSE.txt`、`qt*-6.12.1.spdx.json`/`.cdx.json` |
| 嵌入執行檔的 `qtbase_*.qm` | Qt 翻譯（qttranslations 6.12.0） | BSD-3-Clause（採開源選項） | `Qt-Translations-BSD-3-Clause.txt`、`qttranslations-6.12.0.*.json` |
| Qt 內含的第三方程式碼（zlib、libpng、libjpeg、FreeType、HarfBuzz、PCRE2、SQLite、md4c、libtiff、libwebp、libpsl、Unicode 資料等） | 各原作者 | BSD、MIT、Zlib、FTL、Unicode、公有領域等寬鬆授權 | `THIRD-PARTY-NOTICES.txt` |
| `avcodec-63.dll`、`avformat-63.dll`、`avutil-61.dll`、`swresample-7.dll`、`swscale-10.dll` | FFmpeg 9.0.2（Qt 官方建置，內嵌 zlib 1.3.1） | GNU LGPL v2.1 或更新版本 | `LGPL-2.1.txt`、`THIRD-PARTY-NOTICES.txt` |
| `d3dcompiler_47.dll`、`dxcompiler.dll`、`dxil.dll` | Microsoft Direct3D／DirectX Shader Compiler（Windows SDK 可轉散布檔案，由 windeployqt 複製） | Microsoft Windows SDK 授權條款及限定元件接收者條款 | `Microsoft-SDK-LICENSE.rtf`、`Microsoft-SDK-THIRD-PARTY-NOTICES.rtf`、`MICROSOFT-REDIST.md` |
| `msvcp140*.dll`、`vcruntime140*.dll`、`concrt140.dll`、`vccorlib140.dll` | Microsoft Visual C++ 執行階段（C++23 本機建置用 MSVC 14.52，已發布 v2.0.9 用 MSVC 14.44） | Microsoft Visual Studio 授權條款中的可轉散布程式碼 | `MICROSOFT-REDIST.md` |
| SQLite 3.54.1 | 由 Qt 6.12.1 的 `qsqlite` 外掛內建 | 公有領域 | `THIRD-PARTY-NOTICES.txt` |

## LGPL 與對應原始碼

Qt 與 FFmpeg 以獨立 DLL 動態連結。使用者可替換相容版本，也可為修改函式庫進行除錯與逆向工程。
重新編譯應用程式所需的自有來源與建置腳本在 source ZIP；無 DRM、DLL 簽章或雜湊鎖定。

完整的固定來源、建置設定、SHA256 與交付方式見 `licenses/SOURCE-DELIVERY.md`、
`licenses/source-provenance.json` 及 `licenses/build-info/`。
第三方來源包必須與二進位在同一下載位置、免費且無額外登入門檻提供，並驗證所有雜湊。
v2.1.0 Portable 內含 `sources/LuckyWheel-2.1.0-third-party-sources.zip`。
Qt Widgets 的繪製使用光柵畫布。Portable 排除備用 Mesa 軟體 OpenGL、TUIO 外掛及 SQLite 以外的 Qt SQL 驅動程式。
Microsoft 元件同意流程與限定範圍見 `licenses/END-USER-TERMS.txt`、`MICROSOFT-REDIST.md`。
完整二進位包及對應來源一併提供。 [v2.1.0 正式發布頁](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.1.0) · [v2.1.0 第三方來源 ZIP](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.1.0/LuckyWheel-2.1.0-third-party-sources.zip)

九個嵌入的 qtbase_*.qm：zh_TW、zh_CN、en、ja、ko、es、fr、de、pt_BR。
Copyright (C) The Qt Company Ltd. and other contributors.
完整 BSD-3-Clause 條件與免責聲明在 `licenses/Qt-Translations-BSD-3-Clause.txt`，亦嵌入關於頁。
此項不採 LGPL，不推論必須按 LGPL 交付 .ts。

## 本專案自有內容

- 音效：本專案自製的 PCM WAV，來源檔保留在 `assets/`，編譯時嵌入 Qt 資源。
- 字型：使用 Windows 系統字型，未打包複製任何字型檔。

LuckyWheel 本身（原始碼、音效等本專案自製內容）採 MIT 授權，著作權人 kaido-MSVC，全文見 `LICENSE`（隨包為 `LICENSE.txt`）。MIT 授權不涵蓋上表的第三方元件，它們仍各自適用其授權。

## 維護

`THIRD-PARTY-NOTICES.txt` 使用已部署模組的 Qt 來源授權與歸屬聲明。SBOM 從 Qt kit 的 `sbom/` 原樣複製。升級 Qt 或調整部署元件後，請同步核對上表與 `licenses/`。
