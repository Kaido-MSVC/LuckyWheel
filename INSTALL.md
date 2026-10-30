# LuckyWheel v2.0.8 安裝與資料

正式下載見 [GitHub Release v2.0.8](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.8)，發布狀態與驗證範圍見 [RELEASE-STATUS.md](RELEASE-STATUS.md)。
主程式及安裝包未簽章。安裝版採目前使用者安裝，不要求系統管理員權限。

- 安裝：執行 [LuckyWheel-2.0.8-Windows-x64-Setup.exe](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.8/LuckyWheel-2.0.8-Windows-x64-Setup.exe)；預設程式目錄為
  `%LOCALAPPDATA%\Programs\LuckyWheel`，開始選單捷徑固定建立，桌面捷徑可選。
- 免安裝：完整解壓 [LuckyWheel-2.0.8-Windows-x64-Portable.zip](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.8/LuckyWheel-2.0.8-Windows-x64-Portable.zip)，開啟根目錄 `LuckyWheel.exe`。保留 DLL、外掛與授權文件。
- 資料：預設 `%LOCALAPPDATA%\LuckyWheel\LuckyWheel\luckywheel.sqlite`；實際位置見設定頁。
  可用 `--data-dir` 指定隔離目錄。新捷徑不會繼承自訂參數，應自行保留。
- 升級：先關閉程式並用程式內「備份 SQLite」，再使用原安裝目錄。安裝不覆寫資料庫。
  本次未驗證使用者既有安裝的原地升級。
- 備份：使用程式內備份，選尚未存在的檔名；勿在執行中只複製 SQLite 主檔而遺漏 WAL。
  還原前會產生 before-restore 安全備份。CSV 歷史不能取代完整 SQLite 備份。
- 解除安裝：只刪除封裝中的程式檔案、捷徑與登錄，保留資料及程式目錄自行加入的檔案。

清除歷史會經過兩次確認，屬於永久刪除；先備份。
設定鎖用於避免誤操作，資料庫沒有加密，不能防止有檔案權限者改動資料。

Microsoft 元件條款：安裝版需勾選同意；無聲安裝僅在已閱讀並同意條款時使用
`/S /ACCEPTMS=1`，缺少同意參數會退出而不安裝。Portable 及安裝後首次執行均會
顯示同意／拒絕，拒絕在開啟資料庫前結束。條款改版後會重新詢問。
`microsoft-terms.sha256` 只在所選資料目錄保存同意版本，沒有對外傳送。
每份完整二進位包都帶有 sources/ 第三方來源 ZIP，請保留它及 licenses/；
修改 LGPL 庫的建置與替換說明見 licenses/SOURCE-DELIVERY.md。
