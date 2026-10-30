# LuckyWheel v2.0.5

基準：已唯讀核實 CMake 與 runtime EXE 為 2.0.4。沿用 Qt 6.12.0 / MSVC 14.44 / CMake / NSIS；未安裝或升級依賴。

- 歷史畫面改顯示「流水編號」，CSV 第一欄改為 `sequence`；按內部 ID 排序計算目前歷史集合的 1-based 編號，清除全部後下一筆為 1。資料庫 ID 與 AUTOINCREMENT 不重設。排序、庫存、輪次與抽獎交易保持原有行為。
- 獎品頁新增「匯入格式與範例」，可查看並儲存 UTF-8 CSV 與 schema=1 JSON 範例。範例由正式序列化函式產生，經正式解析器與介面匯入測試。
- CSV 相容原本四欄 `name,probability,limit,color`，新增可選第五欄 `remaining`；四欄匯入仍將庫存設為 limit。新增獎品 CSV 匯出，五欄可保留目前庫存與顏色。CSV 會生成新獎品 ID；JSON 可保留 ID。歷史 CSV 不能當作獎品 CSV 匯入。
- CSV 提供行號與欄位錯誤；JSON 提供解析錯誤行號／位元組位置或 schema 欄位路徑。JSON 對已提供的選項拒絕錯誤型別，缺省選項仍相容原本預設值。取消與無效匯入不寫入資料，也不先處理未儲存草稿。
- 新增 Qt QColorDialog 圖形選色、HEX 色碼色塊、自動配色、恢復預設配色與草稿轉盤預覽。操作先修改草稿，套用保存後才更新實際轉盤與 SQLite。預覽可讀取尚未提交的 HEX 編輯器，不保存、不抽獎。
- 「恢復預設配色」按目前列順序恢复原始 A/B 配色（#72b8ff / #a8dca6），其他列使用原本新增獎品配色規則。自動配色使用固定 HSV 色相間隔；不改動機率、庫存、ID。
- JSON／CSV 匯出皆使用 QColor 的標準 #RRGGBB。JSON 載入保留目前輪次與鎖定，CSV 保留全域設定；既有行為不變。
- Hover 只增加測試诊斷、視窗定位／命中檢查與獨立樣式渲染測試，未修改正式 applyTheme 樣式。真實游標與完整測試的結果及限制以 TEST-REPORT-v2.0.5.md 為準。

使用方式：在獎品頁選取獎品後按「圖形選色」，或直接編輯顏色欄 HEX。預覽草稿後按「套用並保存」。匯入前可由「匯入格式與範例」儲存可用檔案；CSV 預覽確認後才保存。備份 SQLite 可保留完整設定與歷史。

免安裝版解壓後執行 LuckyWheel.exe。預設資料位置沿用 Qt AppLocalDataLocation；免安裝不表示資料自動寫在 EXE 同目錄，可用 `--data-dir` 指定資料目錄。
