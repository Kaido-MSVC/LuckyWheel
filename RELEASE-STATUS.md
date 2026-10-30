# LuckyWheel v2.0.8 發布狀態（2026-10-30）

- Repo：GO，限 package.ps1 明確清單所產生的 source ZIP／相同檔案清單。
- Release：GO，本機完整發布套件已補齊 Microsoft 下游條款／接受流程及 LGPL 實體來源交付。
- 沒有推送 GitHub、建立公開 Release 或修改使用者正式資料；公開上傳仍待使用者確認。

## 已解除的兩項 blocker

1. SDK 三個 DLL 在官方 Classic Windows app redist 清單；原始 RTF／通知隨附。
   VS 2026 Community 第 8 節及 VC/redist 清單亦已核對；13 個 Microsoft DLL 未修改且來源雜湊一致。
   安裝版勾選接收者條款；無聲模式需明確 /ACCEPTMS=1；兩種版本首次啟動均有同意流程。
   條款僅針對 Microsoft DLL，不限制 Qt／FFmpeg 的 LGPL 權利。
2. Qt 四模組來源 SBOM 與來源包逐檔核對，Git 控制檔缺項補齊；FFmpeg 與固定官方 recipe 相符。
   每份 Portable／Setup 都實際內含第三方來源 ZIP 及建置設定，不依賴尚未建立的線上服務。

## 持續義務及界線

發布者必須遵守 SDK／VS 散布者條款，包含 Microsoft 賠償／抗辯／免責保護義務，
見 licenses/MICROSOFT-REDIST.md。接收者按同意不能代替發布者義務。
後續公開發布應上傳同一套 Portable、Setup、自有 source ZIP、第三方 source ZIP、SHA256，
保留來源與通知；不要使用 packages 根目錄的舊 2.0.8 包。
維持版本 2.0.8，以新的 SHA256 辨識內容。未簽章，未驗證乾淨 Windows 或既有安裝原地升級。
第三方庫未作位元組相同重建；已驗證來源對應性及交付，並未聲稱重現整個上游供應鏈。
既有素材的自製聲明仍由發布者負責。本機 native-v2 沒有 .git，不存在已核對乾淨的 Git 歷史。
