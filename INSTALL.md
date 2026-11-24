# LuckyWheel v2.1.0 免安裝版與資料

v2.1.0 提供 Windows x64 免安裝版。主程式目前未簽章。 [正式發布頁](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.1.0) · [驗證範圍](RELEASE-STATUS.md)

- 完整解壓 ZIP 並開啟根目錄的 `LuckyWheel.exe`。保留 DLL、外掛、授權文件與來源。 [Windows x64 免安裝版](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.1.0/LuckyWheel-2.1.0-Windows-x64-Portable.zip)
- 資料：預設 `%LOCALAPPDATA%\LuckyWheel\LuckyWheel\luckywheel.sqlite`；實際位置見設定頁。
  可用 `--data-dir` 指定隔離目錄。新捷徑不會繼承自訂參數，應自行保留。
- 升級時先在舊版備份資料庫，關閉舊版，再從新解壓的資料夾啟動 v2.1.0。使用自訂資料目錄時，沿用原本的 `--data-dir`。
- 備份：使用程式內備份，選尚未存在的檔名；勿在執行中只複製 SQLite 主檔而遺漏 WAL。
  還原前會產生 before-restore 安全備份。CSV 歷史不能取代完整 SQLite 備份。

清除歷史會經過兩次確認，屬於永久刪除；先備份。
設定鎖用於避免誤操作，資料庫沒有加密，不能防止有檔案權限者改動資料。

首次執行會顯示 Microsoft 元件條款。若拒絕，程式會在開啟資料庫前結束。條款改版後會重新詢問。
`microsoft-terms.sha256` 只在所選資料目錄保存同意版本，沒有對外傳送。
每份完整二進位包都帶有 sources/ 第三方來源 ZIP，請保留它及 licenses/；
修改 LGPL 庫的建置與替換說明見 licenses/SOURCE-DELIVERY.md。

## v2.0.9 歷史安裝版

v2.0.9 安裝版採目前使用者安裝。預設程式目錄為 `%LOCALAPPDATA%\Programs\LuckyWheel`，會建立開始選單捷徑，可選擇是否建立桌面捷徑。確認已閱讀並同意 Microsoft 元件條款後，可用 `/S /ACCEPTMS=1` 無聲安裝。解除安裝會保留資料及自行加入的檔案。 [v2.0.9 正式發布頁](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.9)
