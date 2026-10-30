UTF-8；1–200 個獎品。名稱 1–120 字，不可換行。顏色 #RRGGBB。
機率總和 ≤100%，未分配及售罄機率歸未中獎；獨立抽樣。
limit=-1 為不限量（remaining 必須 -1）；否則 0≤remaining≤limit。
匯入將取代獎品，確認前不寫入；先備份可保留舊設定。
CSV 標頭：name,probability,limit,color 或再加 remaining。欄位順序固定。
probability 是百分比數字，最多 4 位小數，不含 %；10.0000=10%。
省略 remaining 時庫存設為 limit；CSV 產生新的獎品 ID，不含全域設定。
逗號或引號必須以雙引號包住；內部引號寫成兩個引號。獎品 CSV 與歷史 CSV 格式不同。
UTF-8; fixed headers above. Percent 0–100, ≤4 decimals; color #RRGGBB.
Optional remaining preserves stock; omission resets it to limit. IDs are regenerated.

UTF-8；1–200 個獎品。名稱 1–120 字，不可換行。顏色 #RRGGBB。
機率總和 ≤100%，未分配及售罄機率歸未中獎；獨立抽樣。
limit=-1 為不限量（remaining 必須 -1）；否則 0≤remaining≤limit。
匯入將取代獎品，確認前不寫入；先備份可保留舊設定。
JSON schema=1，prizes 陣列必須含唯一 id、name、weight、limit、remaining、color。
weight 為整數：100000=10%，1000000=100%。volume 0–100；theme light/dark；
language zh_TW/zh_CN/en/ja/ko/es/fr/de/pt；sound/showSoldOut 為布林。
載入保留目前輪次與鎖定；範例不含密碼。JSON 可保留獎品 ID、庫存及顏色。