# LuckyWheel 2.0.8

本版在介面加入「關於與授權」，抽獎核心、資料格式、部署內容及封裝流程都不變。

## 介面
1. 「設定與工具」新增「關於與授權」按鈕，開啟的對話框包含：
   - 程式名稱、版本、著作權（Copyright © 2026 kaido-MSVC）與 MIT 授權。
   - 授權摘要：本程式可以自由使用、修改、散布，但要保留著作權與授權聲明，軟體依現況提供、不附擔保。第三方元件是未修改的獨立 DLL，各自適用其授權，不受 MIT 涵蓋，可以換成相容版本。
   - 第三方元件清單：Qt（LGPL v3）、FFmpeg 9.0.1（LGPL v2.1 或更新版）、Qt 內含的 zlib／FreeType／SQLite 等、Mesa llvmpipe、DirectX 與 VC++ 執行階段，並附 Qt、FFmpeg 對應原始碼的下載位置。Qt 版本取自執行時實際載入的 DLL。
   - MIT 授權全文。`LICENSE` 透過新增的 `assets/legal.qrc` 嵌入執行檔，只搬走 EXE 也看得到。
2. 對話框提供「開啟授權資料夾」（開啟程式目錄的 `licenses`；找不到時會提示安裝版與免安裝 ZIP 都附完整授權文件）、「關於 Qt」（Qt 內建授權資訊）及「關閉」。
3. 新增的文字都有九種語言版本，包括通用的「關閉」。

## 測試
4. UI 測試新增 `aboutShowsLicenses`，檢查授權摘要、第三方清單、LGPL 版本字樣及 MIT 全文確實顯示。「開啟授權資料夾」會啟動檔案總管，不納入自動測試。

## 維護
5. 對話框的第三方清單寫在 `MainWindow::about()`。升級 Qt 或部署內容改變時，除了 `THIRD-PARTY.md` 與 `licenses/`，也要同步修改這份清單。

建置仍使用 Qt 6.12.0、MSVC 14.44、CMake／Ninja，不新增外部套件。
