# LuckyWheel v2.0.9

2026-11-04 的語言與 UI 修正版，以 Windows 本機 D:\projects\LuckyWheel\native-v2 已修正原始碼作為唯一應用程式來源，重新完整 MSVC x64 Release 建置。舊 v2.0.8 tag／附件保留。

## 修正

- 首次啟動依 Windows 介面語言偏好選擇繁中、簡中、英、日、韓、西、法、德、葡；無支援時回退英文。
- 手動語言選擇保存並優先沿用，既有 SQLite 配置、庫存及紀錄保持相容。
- 高 DPI 小桌面初始／最小視窗尺寸調整、內容捲動與換頁重設捲動位置。
- 獎品名稱、結果及訊息強制純文字，避免 <b> 等字串被誤判為 HTML。
- 歷史時間／輪次／機率等欄位依內容配置寬度；保留語言切換即時刷新狀態的修正，移除關於頁過時發布文字。
- 更新 README、變更紀錄、授權版本資訊與來源交付；封裝腳本依當前版本收錄發布說明，部署固定使用既有授權紀錄中的 SDK D3Dcompiler DLL。

## 下載

- [Portable](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.9/LuckyWheel-2.0.9-Windows-x64-Portable.zip)：完整解壓後執行 LuckyWheel.exe。
- [Setup](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.9/LuckyWheel-2.0.9-Windows-x64-Setup.exe)：目前使用者安裝。
- [應用程式來源](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.9/LuckyWheel-2.0.9-source.zip)。
- [第三方對應來源](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.9/LuckyWheel-2.0.9-third-party-sources.zip)。
- [SHA256SUMS.txt](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.9/SHA256SUMS.txt)。

Portable／Setup 均實體內含相同第三方來源 ZIP、licenses 與依賴。新 EXE 為 2.0.9，不沿用舊 build/dist。升級前關閉程式並用內建功能備份資料。

## 建置與驗證

- C++17、Qt 6.12.0、MSVC 19.44.35229 x64（工具目錄 14.44.35207）、Release／Ninja；清除編譯產物後完成全量建置。
- 核心 42 項通過。離屏 UI 全套其餘 31 項通過；授權頁曾因整理文件的署名大小寫差異失敗，恢復原署名、重建後定向回歸通過，合計 32 項涵蓋完成。
- Windows 原生 UI 全套 30 項通過；同一授權頁修正後定向回歸通過。另 1 項真實游標／按鈕回饋測試，兩次均記錄前景／命中為其他程式 PID，無法完成原生滑鼠驗證；離屏相應測試通過。
- 150%／200% 原生縮放：小桌面、九語最小視窗控制項可達性與捲動定向測試均通過。
- Microsoft 條款拒絕／接受、有效收據及過期收據測試通過。
- 封裝驗證須核對新建置 EXE、Portable／Setup 負載、48 個 DLL 來源雜湊、第三方來源及 source ZIP 與提交內容；附件以 SHA256SUMS.txt 識別。

未實測切換 Windows 語言後重新登入、混合 DPI 多螢幕、乾淨 Windows 安裝／解除安裝、既有安裝原地升級及長時間壓力測試。未數位簽章。

## 授權

自有程式碼及自製音效 MIT；Qt 動態連結 LGPLv3、FFmpeg LGPLv2.1 或更新版本、Qt 翻譯 BSD-3-Clause。允許替換相容 LGPL 函式庫，並為修改函式庫進行除錯與逆向工程；完整通知與固定來源見 licenses/SOURCE-DELIVERY.md、licenses/source-provenance.json。
Microsoft SDK／Visual C++ 元件限依各自條款散布；保留安裝及首次啟動的同意流程。無聲安裝需先閱讀同意，再明確提供 /ACCEPTMS=1。相關義務僅限指定 Microsoft DLL，不限制 MIT／LGPL 權利；散布者持續義務見 licenses/MICROSOFT-REDIST.md。
第三方函式庫未升版或修改，未宣稱可位元組相同重建。