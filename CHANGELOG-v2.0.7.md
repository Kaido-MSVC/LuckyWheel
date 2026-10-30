# LuckyWheel 2.0.7

本版修正封裝流程與授權文件，抽獎核心、資料格式及介面行為不變。

## 授權
1. LuckyWheel 採 MIT 授權，著作權人 kaido-MSVC，全文見 `LICENSE`（發佈包內為 `LICENSE.txt`）。執行檔與安裝程式的版本資訊加入版權與發行者；安裝程式新增授權頁。
2. `licenses/` 原本放錯檔案：`Copyright.txt` 是 CMake 授權，`COPYING.txt` 是 Apache 2.0，兩者都與隨包元件無關，已移除。改為附上 Qt 適用的 `LGPL-3.0.txt`、`GPL-3.0.txt`，FFmpeg 適用的 `LGPL-2.1.txt`；Qt 授權說明改名為 `Qt-LICENSE.txt`。
3. 新增 `THIRD-PARTY-NOTICES.txt`：從本機 Qt 6.12.0 文件中已部署模組的 attribution 頁面原文產生，共 57 項，包含 zlib、libpng、FreeType、HarfBuzz、SQLite、libtiff、libwebp、FFmpeg、Mesa llvmpipe 等。補上 qtimageformats 的 SBOM。
4. 重寫 `THIRD-PARTY.md`，逐項列出隨包 DLL、授權及 Qt／FFmpeg 對應原始碼的下載位置。

## 封裝修正
5. 本機 Qt 新增 PDF 模組後，windeployqt 會額外部署未使用的 `Qt6Pdf.dll` 與 `qpdf.dll`。`build.cmd` 現在排除 `qpdf`，部署內容與 2.0.6 相同。
6. `build.cmd` 部署前先清空 `build\dist`，避免舊的 DLL 被一併打包。
7. `package.ps1` 改成先刪除再複製授權資料夾，修正重複執行時出現 `licenses\licenses` 巢狀目錄、已移除檔案殘留的問題。缺少授權檔或 payload 含 `Qt6Pdf.dll` 時中止封裝。

## 程式修正
8. 獨立模擬在設定無效時，會讀取空結果導致越界；現在改為顯示錯誤。正常操作下設定都已驗證，不易觸發。
9. `--data-dir` 後面沒有路徑時，原本會無聲結束；現在顯示雙語提示。

建置仍使用 Qt 6.12.0、MSVC 14.44、CMake／Ninja，不新增外部套件。
