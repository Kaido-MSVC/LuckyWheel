# LGPL 對應原始碼與建置交付

LuckyWheel v2.1.0 的 Windows x64 Portable 內附
`sources/LuckyWheel-2.1.0-third-party-sources.zip`。散布完整二進位時請保留這份來源 ZIP 及 `licenses/`。
上游網址、固定提交、壓縮檔大小及 SHA-256 見 [source-provenance.json](source-provenance.json)。
自有程式的 MIT 授權來源及建置腳本另附在 LuckyWheel source ZIP。

| 元件 | 對應來源 |
|---|---|
| Qt Base 6.12.1 | 官方來源壓縮檔，提交 5af1a37e31a869cd6eb8a73d82e2c71a450e6634。包含 QSQLITE 驅動程式和 SQLite 3.54.1 完整 amalgamation |
| Qt SVG 6.12.1 | 官方來源壓縮檔，提交 ce2d9787bf7c6e4d9ab79b33b99ebbcfec7aef00 |
| Qt Multimedia 6.12.1 | 官方來源壓縮檔，提交 1469cb18a238c2f3c442cc1a66e75a1dbf55e717 |
| Qt Image Formats 6.12.1 | 官方來源壓縮檔，提交 032f617fbb10c3116e1cf23b630974f9f729efd1 |
| Qt 翻譯 6.12.0 | 九個原有嵌入式 `.qm` 檔與已核對的來源相符，採 BSD-3-Clause |
| FFmpeg 9.0.2 | 上游發布壓縮檔，SHA-1 與 Qt 官方 MSVC 建置腳本相符 |
| zlib 1.3.1 | 官方壓縮檔，Qt 的 MSVC 建置腳本將它連結至 FFmpeg。Qt 自身內嵌的 zlib 1.4 已隨 Qt Base 來源交付 |
| Qt 建置腳本 | Qt supermodule v6.12.1 解參照提交 88bc9efbe213208113439d7cb3e119469784f363，包含 `coin/provisioning` |

## 建置與替換

1. 解壓縮來源 ZIP 並用 `SHA256SUMS.txt` 核對其中八份壓縮檔。`build-info/` 保留上游 Qt kit 的模組配置與 FFmpeg 的實際建置參數。
2. 用 x64 MSVC、CMake、Ninja 建置動態連結 Qt Base，先執行來源的 `configure.bat` 並選擇自己的安裝路徑。參考 `config_qtbase.opt` 和 `.summary` 保留內嵌 SQLite 設定。以該路徑的 `bin/qt-configure-module.bat` 設定 Qt SVG、Qt Image Formats、Qt Multimedia。
3. FFmpeg 用 x64 MSVC、MSYS2 shell、make 及 NASM。先建置 zlib 1.3.1，再將 `build-info/ffmpeg-configuration.txt` 的路徑換成本機位置，以相同選項執行來源 `configure`、`make`、`make install`。保持 `--enable-shared --disable-static`。Qt Multimedia 建置時指向該 FFmpeg 路徑。FFmpeg 與 zlib 的 Qt 建置腳本在 `coin/provisioning/common/windows/install-ffmpeg-msvc.ps1` 和同目錄的輔助指令碼，通用配置在 `coin/provisioning/common/shared/ffmpeg_config_options.txt`。
4. 依 LuckyWheel source ZIP 的 `BUILD.md` 建置應用程式。關閉程式後，使用可寫入的部署目錄替換相容 x64 Qt／FFmpeg DLL 和外掛程式，再啟動驗證。LuckyWheel 允許為修改 LGPL 函式庫進行除錯與逆向工程。套件製作核對來源雜湊，應用程式執行時允許替換相容 DLL。

[Qt Windows 來源建置](https://doc.qt.io/qt-6/windows-building.html) · [Qt Multimedia 來源建置](https://doc.qt.io/qt-6/qtmultimedia-building-from-source.html) · [FFmpeg Windows 建置](https://ffmpeg.org/platform.html) · [FFmpeg 授權](https://ffmpeg.org/legal.html)

## 對應性證據

- Qt 6.12.1 四份來源壓縮檔各已符合 Qt 官方公布的 SHA-256。搭配各模組的逐檔 source SPDX，合計核對 27,695 筆記錄。比對接受可驗證的 CRLF checkout 差異，49 個 Git 控制檔與 `.tag` 補件按固定提交取回並核對。Qt Multimedia 的 `.tag` 匯出值與 source SPDX 的該筆雜湊不同，其餘 3,253 筆相符。記錄見 `build-info/source-verification.json`，補件在 `supplement/`。
- SQLite 3.54.1 的 `sqlite3.c` 和 `sqlite3.h` 與 SQLite 官方 amalgamation ZIP 逐位元組相同，`sqlite_source_id()` 也相符。該 SQLite 原始碼已在 Qt Base 壓縮檔內交付。QSQLITE 使用 Qt 官方 x64 kit 提供的原始外掛。
- FFmpeg 的五個 DLL 均回報相同的編譯選項和 LGPL v2.1 或更新版本。Qt 官方 Windows 建置腳本固定 FFmpeg 9.0.2 與 zlib 1.3.1 的下載雜湊。來源對應性依上游壓縮檔、SPDX 及二進位執行期證據核對。
- v2.0.9 的原始檔及附件版本見 [正式發布頁](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.9)。
