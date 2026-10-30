# LuckyWheel v2.0.8 發布狀態（2026-10-30）

- Repo：已公開於 [Kaido-MSVC/LuckyWheel](https://github.com/Kaido-MSVC/LuckyWheel)，採 package.ps1 明確清單所產生的 source ZIP／相同檔案清單。
- Release：[v2.0.8](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.8) 已正式發布，包含 Portable、Setup、自有 source ZIP、第三方 source ZIP 與 SHA256 清單；已補齊 Microsoft 下游條款／接受流程及 LGPL 實體來源交付。
- main 的文件已更新為發布後狀態；v2.0.8 tag 與既有附件保留發布時快照。

## 已解除的兩項 blocker

1. SDK 三個 DLL 在官方 Classic Windows app redist 清單；原始 RTF／通知隨附。
   VS 2026 Community 第 8 節及 VC/redist 清單亦已核對；13 個 Microsoft DLL 未修改且來源雜湊一致。
   安裝版勾選接收者條款；無聲模式需明確 /ACCEPTMS=1；兩種版本首次啟動均有同意流程。
   條款僅針對 Microsoft DLL，不限制 Qt／FFmpeg 的 LGPL 權利。
2. Qt 四模組來源 SBOM 與來源包逐檔核對，Git 控制檔缺項補齊；FFmpeg 與固定官方 recipe 相符。
   每份 Portable／Setup 都實際內含第三方來源 ZIP 及建置設定，不依賴線上服務。

## 持續義務及界線

發布者必須遵守 SDK／VS 散布者條款，包含 Microsoft 賠償／抗辯／免責保護義務，
見 licenses/MICROSOFT-REDIST.md。接收者按同意不能代替發布者義務。
正式下載以 [v2.0.8 Release](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.8) 的五個附件及 [SHA256SUMS.txt](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.8/SHA256SUMS.txt) 為準；
再散布時保留完整來源與通知。舊本機同版套件不屬於此次發布。
版本維持 2.0.8，以發布附件的 SHA256 辨識內容。未簽章，未驗證乾淨 Windows 或既有安裝原地升級。
第三方庫未作位元組相同重建；已驗證來源對應性及交付，並未聲稱重現整個上游供應鏈。
既有素材的自製聲明仍由發布者負責。
