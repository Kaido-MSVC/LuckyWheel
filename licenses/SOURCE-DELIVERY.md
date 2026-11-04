# LGPL 對應來源與建置交付（v2.0.9）

每份 Portable 與 Setup 都實際內含 `sources/LuckyWheel-2.0.9-third-party-sources.zip`，
不依賴上游網址或寄信索取。相同來源 ZIP 亦可由 [v2.0.9 Release 免費下載](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.9/LuckyWheel-2.0.9-third-party-sources.zip)。
內含下表原始壓縮檔、SHA256 清單、這份說明、建置設定及來源證據。
完整來源網址、提交、大小、SHA256 見 source-provenance.json；請按雜湊識別，勿只看 6.12.0 名稱。

| 元件 | 固定來源 | 證據 |
|---|---|---|
| qtbase | 025bdad181de241e81bf853c8a2d7bf3d19261a9 | 安裝版 SBOM downloadLocation 與來源 .tag 相符 |
| qtsvg | 883a58532078d003cefef0a549bfe9f885ce41f5 | 安裝版 SBOM downloadLocation 與來源 .tag 相符 |
| qtmultimedia | 6.12.0，.tag 6ca885f084a3984749d53bb50a0e08bdc60e86cc | 官方版本來源包；安裝 SBOM 為 6.12.0 |
| qtimageformats | 6.12.0，.tag dc13f2fbff1d579cfb2eb2d9092e3563b4d05b8e | 官方版本來源包；安裝 SBOM 為 6.12.0 |
| qttranslations | 6.12.0，.tag 2811457ceff8f0c24a1c4d14f947a283a17239d0 | 九個 .qm 與安裝來源一致，SBOM 為 BSD-3-Clause |
| FFmpeg | 9.0.1 原始 tar.gz | SHA1 89c318905212e6b67e10e42ab10ab8007e0aefda，與固定 Qt recipe 相符 |
| zlib | 1.3.1 原始 tar.gz | FFmpeg 內嵌 configure 指向 zlib-1.3.1；來源隨附 |
| Qt 建置腳本 | qt5 ef25002ed292b132d1ed3246bfcee1d24a9cd966 | v6.12.0 tag 解參照提交，來源 .tag 相符 |

Qt 建置腳本 archive 只含 supermodule 自身內容（coin/provisioning 等），子模組來源由上表各包提供；
不把 supermodule 空子模組目錄誤認為完整 Qt 原始碼。

## 如何重建與替換

1. 解開來源包。Qt configure 與各模組 configure.opt/configure.summary 的實際內容保留於 build-info/。
   上游配置含 Qt CI 的路徑與工具，不是使用者私人路徑；須依本機工具位置調整。
2. Qt DLL 由上游 MSVC 19.44.35227、CMake 4.3.2、Ninja Multi-Config 建立。
   先以 qtbase/configure.bat 選擇 x64 MSVC、shared、自己的 prefix，配置／建置／安裝 qtbase，
   再以該 prefix/bin/qt-configure-module.bat 配置 qtsvg、qtimageformats、qtmultimedia。
   qtmultimedia 指向下步的 FFmpeg prefix。需要的額外建置工具按官方來源建置文件準備。
3. FFmpeg 的完整 configure 命令在 build-info/ffmpeg-configuration.txt，五個 DLL 均相同。
   用 x64 MSVC、MSYS2 shell、make、NASM 等所需工具，先編譯 zlib 1.3.1，將 include/lib 路徑換成本機位置，
   執行來源 configure 後 make / make install。保留 --enable-shared --disable-static，
   不啟用 --enable-gpl、--enable-nonfree 或 --enable-version3。
   實際上游 recipe 位於 qt5 archive 的 coin/provisioning/common/windows/install-ffmpeg.ps1，
   共用選項在 common/shared/ffmpeg_config_options.txt，zlib 與工具輔助腳本也在同一 archive。
   CI provisioning 會改動機器及安裝工具，請閱讀後手動調整，不要直接執行整套腳本。
4. 自有程式依 BUILD.md 建置。關閉 LuckyWheel，備份程式資料及原 DLL，在可寫入的程式目錄替換
   相容 x64 Qt／FFmpeg DLL 及外掛，再啟動驗證。不要只替換其中一個 ABI 不相容 DLL。
   LuckyWheel 沒有 DLL 雜湊或簽章鎖定；允許為修改函式庫進行除錯與逆向工程。

參考：
https://doc.qt.io/qt-6/windows-building.html
https://doc.qt.io/qt-6/qtmultimedia-building-from-source.html
https://doc.qt.io/qt-6/ffmpeg-with-qt-multimedia.html
https://ffmpeg.org/platform.html
https://ffmpeg.org/legal.html
https://www.qt.io/development/open-source-lgpl-obligations

## 已完成的實際交付安排

打包必須先驗證來源 ZIP 中八份來源 archive 的 SHA256 與 source-provenance.json 相同，
並要求來源 ZIP 內的來源證據及建置資料與本機一致；驗證通過後嵌入 Portable／Setup。
因此每次傳送任何一種完整二進位包，都同時交付完整第三方來源，不必另向作者索取。
安裝時會把 sources/ 一併安裝；解除安裝前可複製保存。MIT 自有程式來源另附 source ZIP。
[GitHub Release v2.0.9](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.9) 已標示 FFmpeg LGPL 與來源包下載位置，
並提供 Portable、Setup、自有 source ZIP、第三方 source ZIP、SHA256 清單。
鏡像及再散布保留整包來源與通知即可沿用交付方式。

## 對應性驗證結果及限制

Qt kit 自帶的 *.source.spdx 逐檔 SHA1 用來核對實際建置來源，完整核對結果在
build-info/source-verification.json，原始 source SBOM 在來源 ZIP 的 evidence/。
qtbase 23,144、qtsvg 642、qtmultimedia 3,149、qtimageformats 508 筆來源紀錄均已處理：
程式／建置內容逐位元組或 Windows checkout 的 LF/CRLF 轉換後相同；
qtbase／qtsvg 的 47 個 Git 控制檔因上游 export-ignore 未納入 archive，已按相同固定提交
下載、核對並附於 supplement/；可疊放回各模組。`.tag` 的差異已確認來自 .gitattributes 的 export-subst：Git archive 將 `$Format:%H$`
替換為提交；兩份原始 .tag 也已附於 supplement/，Windows CRLF 版本符合來源 SBOM。
所有來源紀錄均已有對應檔案與核對結果，固定提交也與 binary SBOM 相符。
FFmpeg 五檔 DLL 的內嵌 configure、LGPL 宣告及 SHA256 與 kit 一致；9.0.1 tarball
SHA1 與固定官方 recipe 相同；recipe 直接解包 configure，未套用 patch，zlib 來源亦隨附。
Qt 官方說明：https://doc.qt.io/qt-6/qtmultimedia-attribution-ffmpeg.html
LuckyWheel 沒有自行修改第三方庫。未作位元組相同的第三方重建；此項不是來源交付的前提。
這些是依上游發行證據作出的對應性判定，不是對所有歷史供應鏈行為的獨立證明。
