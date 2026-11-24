# LuckyWheel v2.1.0 — Windows x64

## English overview

**LuckyWheel v2.1.0** is an offline Windows x64 prize-drawing wheel built with C++23 and Qt Widgets. It has collapsible navigation, a redesigned wheel and draw console, and light, dark, and system themes. Each outcome is selected and recorded before the wheel animation displays it. [v2.1.0 release notes](RELEASE-NOTES-v2.1.0.md)

The interface supports **nine languages**: Traditional Chinese, Simplified Chinese, English, Japanese, Korean, Spanish, French, German, and Portuguese. On first launch it follows supported Windows display-language preferences, falling back to English. Manual language selections are saved across restarts.

When a prize runs out, its probability becomes part of the no-prize outcome; the probabilities of other prizes do not increase. The settings password lock helps prevent accidental changes but **does not encrypt the local database**.

Extract the complete v2.1.0 Portable ZIP and retain the bundled libraries, license notices and source archive. The program works offline. The project's own code is MIT-licensed. Third-party components retain separate terms, including Microsoft component terms presented for acceptance on first launch. [Download Windows x64 Portable](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.1.0/LuckyWheel-2.1.0-Windows-x64-Portable.zip)


Qt Widgets 幸運轉盤，支援獎品庫存、歷史紀錄、CSV／JSON 與九語介面。
v2.1.0 使用 C++23，重新配置轉盤與抽獎控制台，提供可完全收合的導覽及跟隨系統外觀。
本版使用 Qt 6.12.1 官方 QSQLITE 驅動程式，內含 SQLite 3.54.1 的安全修正。

v2.1.0 提供 Windows x64 免安裝版與對應原始碼。 [正式發布頁](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.1.0) · [版本說明](RELEASE-NOTES-v2.1.0.md) · [驗證範圍](RELEASE-STATUS.md)

## 取得與執行

- 完整解壓 ZIP 後，開啟根目錄的 `LuckyWheel.exe`。 [Windows x64 免安裝版](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.1.0/LuckyWheel-2.1.0-Windows-x64-Portable.zip)
- 建置或查閱本版原始碼請使用對應的來源附件。 [應用程式來源 ZIP](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.1.0/LuckyWheel-2.1.0-source.zip) · [第三方來源 ZIP](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.1.0/LuckyWheel-2.1.0-third-party-sources.zip)
- 建置目前的原始碼請執行 `build.cmd`，完成後開啟 `build-cpp23/dist/LuckyWheel.exe`。詳見 [BUILD.md](BUILD.md)。

SHA-256 可用於核對下載檔案。LuckyWheel 主程式目前未簽章。請保留 DLL、Qt 外掛、`licenses/` 和 `sources/`。 [SHA256SUMS.txt](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.1.0/SHA256SUMS.txt)

資料預設位於 `%LOCALAPPDATA%/LuckyWheel/LuckyWheel/`，程式離線工作。
如需隔離資料，可執行 `LuckyWheel.exe --data-dir "D:\LuckyWheelData"`。
免安裝版使用方式、升級及備份流程見 [INSTALL.md](INSTALL.md)。

## 功能

- 每個獎品設定名稱、機率、總數、剩餘數量與顏色。
- 機率支援四位小數百分比，最低單位為 `0.0001%`。只接受一般十進位、小數點使用 `.`，不接受科學記號或自動四捨五入。
- 機率總和不可超過 100%；不足部分自動成為「未中獎」。
- 總數、剩餘數量都是整數；`-1 / -1` 代表不限量。剩餘數量不可超過總數。最多 200 個獎品，名稱最多 120 字元。
- 售罄獎品機率轉成未中獎，其他獎品的機率保持不變。
- 可顯示或隱藏售罄區塊；灰色售罄區塊代表未中獎，不代表還能取得該獎品。隱藏後整個面積合併到未中獎。
- 配置、庫存、歷史與介面設定自動儲存於 SQLite。
- 保存／重新載入配置、匯出／載入 JSON、重置數量並建立新一輪；歷史保留。
- 歷史依時間與流水號由新到舊排列。顯示本機時間，CSV 使用 UTC 時間。紀錄保存抽獎當時的有效機率與隨機值。
- 中獎與未中獎音效、音量／靜音，中獎彩紙效果。
- 九語：繁中、簡中、英、日、韓、西班牙、法、德、葡萄牙。首次啟動依 Windows 介面語言偏好選擇支援語言，無支援時回退英文；手動選擇會保存並優先沿用，既有配置不被系統語言覆蓋。
- CSV 批次匯入與確認前預覽、歷史 CSV 匯出、JSON 配置檔。
- 一致性資料庫備份、還原前自動安全備份、淺色／深色／跟隨系統外觀。
- 收合導覽時主畫面自動補滿。展開寬度配合各語言文字，小視窗會讓較長文字換行。
- 全螢幕按 F11，Esc 離開全螢幕。檔案或確認對話方塊開啟時，Esc 會先取消該對話方塊。
- 歷史統計；獨立機率模擬最多 100 萬次，不扣正式庫存、不寫正式紀錄。
- 設定密碼鎖定；鎖定時仍可抽獎、查看與匯出紀錄、備份及模擬。

首次啟動有範例 A（10%、3份）與 B（20%、5份），剩餘 70% 為未中獎。正式活動前請自行設定並按「套用並保存」。

## 抽獎規則

抽獎核心與轉盤繪圖分離。使用系統隨機來源、拒絕取樣消除取餘數偏差，再從 `[0,1000000)` 的整數機率區間抽取一次。顯示設定不參與結果選擇。

不預排結果、不保證中獎、不按歷史或期望比例補償結果。每次均依當時的有效機率重新獨立取樣。庫存會改變後續有效機率：獎品售罄後，其原機率從下一次抽獎起轉為未中獎。

例如 A=10%、B=20%，未中獎=70%；A 售罄後 B 仍為20%，未中獎=80%。顯示售罄時 A 的10%灰色區塊屬於未中獎；隱藏時 B 的面積仍為20%，未中獎面積為80%。

按下開始時即抽取一次並交易保存，接著播放約 3.2 秒動畫，在對應區塊中心停下，保留結果約 1.6 秒。這段時間使用該次抽獎前的機率快照，抽獎與獎品編輯會暫停。結果展示結束後，轉盤回到下一次可抽的機率畫面，控制台保留結果。

因此動畫途中關閉或當機不會重新抽取或退回庫存：結果已保留於歷史。若資料庫交易失敗，不扣庫存、不新增紀錄、不播放成功動畫。

## CSV 與配置

匯入 CSV 為 UTF-8，支援既有四欄，也可在末尾加上 `remaining` 欄保存剩餘數量。

```csv
name,probability,limit,color
頭獎,1.0000,1,#ffd166
二獎,9.0000,5,#70c1ff
```

見 `examples/prizes.csv`。可使用雙引號包住含逗號或引號的欄位，名稱不能換行。匯入先驗證與預覽，確認後取代獎品設定並保留歷史。省略 `remaining` 時，剩餘數量以總數為準。

JSON 保存完整配置與當時剩餘數量；匯出不包含設定鎖密碼。載入 JSON 會保留目前輪次及設定鎖，替換其他配置與庫存。JSON 與 CSV 匯入檔案上限2 MiB。

CSV 歷史匯出使用 UTF-8 BOM；可能被試算表視為公式的欄位會加上單引號，避免被當成公式執行。歷史 CSV 用於查閱，不能當作獎品匯入檔。

## 備份、還原與鎖定

備份使用 SQLite `VACUUM INTO`，包含已提交的配置、庫存與歷史，不需要手動複製 WAL。為避免誤覆寫，備份檔名必須尚未存在。

還原前會先在資料目錄產生 `before-restore-*.sqlite`，再唯讀驗證來源備份及資料格式，最後在同一交易中還原。無效備份不改動目前資料。安全備份不自動刪除，可依自己的保留政策清理。

第一次鎖定設定時輸入並確認至少4字元的密碼。解鎖只對目前執行期間有效，重啟會再次鎖定。再次鎖定需輸入原密碼。本版沒有介面變更／移除密碼功能；活動前請先備份並妥善保管密碼。

設定鎖用於防止操作失誤；資料庫未加密，不能抵抗具有本機檔案存取權的人修改資料。

## 授權與公開範圍

自有原始碼及自製音效採 MIT，Copyright (c) 2026 kaido-MSVC，見 [LICENSE](LICENSE)。
Qt、FFmpeg、翻譯檔及其他第三方元件各自適用其授權，見 [THIRD-PARTY.md](THIRD-PARTY.md)。
「設定與工具 → 關於與授權」提供 MIT、Qt LGPL 與翻譯 BSD 完整聲明，以及授權資料夾入口。
LGPL 函式庫可替換成相容版本；允許為修改函式庫而進行除錯與逆向工程。

`package.ps1` 以明確清單建立 source ZIP，排除本機資料、建置與舊驗證紀錄。
`.gitignore` 只影響未追蹤檔案；不要直接把整個資料夾上傳。
舊測試紀錄仍留在本機，未納入 source ZIP。

Microsoft 元件條款限於指定 DLL；首次啟動需明確同意。每份 Portable／Setup 均內含 Qt／FFmpeg 完整來源 ZIP，見 licenses/SOURCE-DELIVERY.md。

## 版本變更紀錄

- [v2.1.0 版本說明](RELEASE-NOTES-v2.1.0.md)
- [v2.0.9](CHANGELOG-v2.0.9.md)
- [v2.0.8](CHANGELOG-v2.0.8.md)
- [v2.0.7](CHANGELOG-v2.0.7.md)
- [v2.0.6](CHANGELOG-v2.0.6.md)
- [v2.0.5](CHANGELOG-v2.0.5.md)
- [v2.0.4](CHANGELOG-v2.0.4.md)
- [v2.0.2](CHANGELOG-v2.0.2.md)
- [v2.0.1](CHANGELOG-v2.0.1.md)
