# Microsoft redist 核對（2026-10-30）

Windows SDK 10.0.28000.0 的 d3dcompiler_47.dll、dxcompiler.dll、dxil.dll
與本機 Redist/D3D/x64 原檔 SHA256 相符（source-provenance.json）。
官方清單明列這三檔可隨 Classic Windows app 散布，不能隨 UWP app 散布：
https://learn.microsoft.com/en-us/legal/windows-sdk/redist
LuckyWheel 是有實質抽獎功能的 Windows 桌面程式；不修改上述 DLL，保留著作權及通知。
SDK 原始授權及第三方通知 RTF 原樣隨包；另附文字版方便閱讀。

MSVC CRT 14.44.35112 的十個 DLL 與本機 VS 2026 Community 的
VC/Redist/MSVC/14.44.35112/x64/Microsoft.VC143.CRT 相符，不含 Debug CRT。
VS 2026 官方 redist 清單允許 VC/redist 下非 debug_nonredist 檔案隨程式散布，
包含透過 VS 2026 安裝的相容 VC143 元件；本機使用 Stable Community（個人自有程式）。
https://learn.microsoft.com/en-us/visualstudio/releases/2026/redistribution
https://visualstudio.microsoft.com/license-terms/vs2026-ga-community/
原始官方 Word 授權的全文文字擷取附於 Microsoft-VS2026-Community-LICENSE.txt；
上游附件：https://visualstudio.microsoft.com/wp-content/uploads/2025/10/Visual_Studio_2026-License-Community_ENU.docx

## 下游保護與接受

END-USER-TERMS.txt 是發布者提供、僅涵蓋列名 Microsoft DLL 的接收者條款。
包含限定用途、保留通知、修改／逆向限制及法定例外、禁止獨立散布、出口法、
Microsoft 免責及責任限制、再散布者向下游取得同意的義務。
不將這些限制施加於 MIT、Qt、FFmpeg，保留 LGPL 修改／替換／除錯逆向權利。
NSIS 顯示全文並要求勾選；無聲安裝需 /ACCEPTMS=1，否則退出碼 4、未安裝。
Portable 及安裝後首次啟動均在開啟抽獎資料庫前要求明確同意；拒絕退出碼 4。
同意紀錄按資料目錄與嵌入條款 SHA256 保存；條款改變需重新同意。
此 SHA256 只識別條款，不限制 DLL 替換，並非 DRM。

## 發布者持續義務

SDK 2(a)(ii) 與 VS 2026 第 8 節的散布者義務仍由實際發布者承擔，
包含對 Microsoft 的 indemnify / defend / hold harmless；VS 有單獨源於
Distributable Code 請求的例外，SDK 無此例外。不能以接收者按同意代替發布者責任。
[GitHub Release v2.0.8](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.8) 已公開發布完整套件；發布者仍須履行上述持續義務。
再散布應保留完整套件、條款與通知；不得把 Microsoft DLL 重新授權為 MIT。
