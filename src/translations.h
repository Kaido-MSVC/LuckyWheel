#pragma once
#include <QHash>
#include <QStringList>

inline QString uiText(const QString &key, const QString &lang) {
    static const QStringList codes{"zh_TW", "zh_CN", "en", "ja", "ko", "es", "fr", "de", "pt"};
    // Order: Traditional Chinese, Simplified Chinese, English, Japanese, Korean, Spanish, French, German,
    // Portuguese.
    static const QHash<QString, QString> strings{
        {"about", "關於與授權|关于与授权|About & licenses|バージョン情報とライセンス|정보 및 라이선스|Acerca de y licencias|À propos et licences|Info & Lizenzen|Sobre e licenças"},
        {"aboutLicense", "LuckyWheel 本身（原始碼及自製音效）採 MIT 授權，可自由使用、修改及散布，但須保留著作權與授權聲明；軟體依「現況」提供，不附任何擔保。Qt／FFmpeg 函式庫以未經修改的獨立 DLL 動態連結，各自適用其授權，不受 MIT 授權涵蓋；可用相容版本的 DLL 自行替換。授權全文、元件通知與 SBOM 位於程式目錄的 licenses 資料夾。|LuckyWheel 本身（源代码及自制音效）采用 MIT 许可证，可自由使用、修改及分发，但须保留版权与许可声明；软件按“现状”提供，不附任何担保。Qt／FFmpeg 库以未经修改的独立 DLL 动态链接，各自适用其许可证，不受 MIT 许可证涵盖；可用兼容版本的 DLL 自行替换。许可证全文、组件声明与 SBOM 位于程序目录的 licenses 文件夹。|LuckyWheel itself (source code and original sounds) is licensed under the MIT License: you may use, modify and distribute it if the copyright and license notice are kept. It is provided \"as is\", without warranty. The Qt/FFmpeg libraries are dynamically linked as unmodified, separate DLLs and remain under their own licenses, not the MIT License; you may replace them with compatible versions. Full license texts, notices and SBOMs are in the licenses folder next to the program.|LuckyWheel本体（ソースコードと自作の効果音）はMITライセンスです。著作権表示とライセンス表示を残せば、自由に使用・改変・再配布できます。本ソフトウェアは「現状のまま」提供され、いかなる保証もありません。Qt/FFmpegライブラリは未改変の独立したDLLとして動的リンクされ、それぞれのライセンスが適用されます（MITライセンスの対象外）。互換バージョンのDLLに置き換えることができます。ライセンス全文、通知、SBOMはプログラムフォルダーのlicensesにあります。|LuckyWheel 자체(소스 코드 및 자체 제작 효과음)는 MIT 라이선스입니다. 저작권 및 라이선스 고지를 유지하면 자유롭게 사용, 수정, 배포할 수 있습니다. 소프트웨어는 \"있는 그대로\" 제공되며 어떠한 보증도 없습니다. Qt/FFmpeg 라이브러리는 수정하지 않은 별도 DLL로 동적 링크되며 각자의 라이선스를 따르고 MIT 라이선스의 적용을 받지 않습니다. 호환 버전의 DLL로 교체할 수 있습니다. 라이선스 전문, 고지 및 SBOM은 프로그램 폴더의 licenses 폴더에 있습니다.|LuckyWheel (código fuente y sonidos propios) se distribuye bajo la licencia MIT: puede usarlo, modificarlo y distribuirlo conservando el aviso de copyright y de licencia. Se ofrece \"tal cual\", sin garantía. Las bibliotecas Qt/FFmpeg se enlazan dinámicamente como DLL independientes sin modificar y conservan sus propias licencias, no la MIT; puede sustituirlos por versiones compatibles. Los textos completos, avisos y SBOM están en la carpeta licenses junto al programa.|LuckyWheel (code source et sons originaux) est distribué sous licence MIT : vous pouvez l’utiliser, le modifier et le distribuer en conservant les mentions de copyright et de licence. Il est fourni « tel quel », sans garantie. Les bibliothèques Qt/FFmpeg sont liés dynamiquement sous forme de DLL distinctes non modifiées et restent soumis à leurs propres licences, pas à la licence MIT ; vous pouvez les remplacer par des versions compatibles. Textes complets, avis et SBOM se trouvent dans le dossier licenses à côté du programme.|LuckyWheel selbst (Quellcode und eigene Sounds) steht unter der MIT-Lizenz: Nutzung, Änderung und Weitergabe sind erlaubt, wenn Copyright- und Lizenzhinweis erhalten bleiben. Die Software wird „wie besehen“ ohne Gewährleistung bereitgestellt. Die Qt/FFmpeg-Bibliotheken werden als unveränderte, separate DLLs dynamisch gelinkt und unterliegen ihren eigenen Lizenzen, nicht der MIT-Lizenz; sie können durch kompatible Versionen ersetzt werden. Lizenztexte, Hinweise und SBOMs liegen im Ordner licenses neben dem Programm.|O LuckyWheel (código-fonte e sons próprios) é licenciado sob a licença MIT: você pode usá-lo, modificá-lo e distribuí-lo mantendo o aviso de copyright e de licença. É fornecido \"como está\", sem garantia. As bibliotecas Qt/FFmpeg são vinculados dinamicamente como DLLs separadas e não modificadas e seguem suas próprias licenças, não a MIT; podem ser substituídos por versões compatíveis. Textos completos, avisos e SBOMs estão na pasta licenses ao lado do programa."},
        {"thirdParty", "第三方元件|第三方组件|Third-party components|サードパーティ製コンポーネント|타사 구성 요소|Componentes de terceros|Composants tiers|Drittanbieter-Komponenten|Componentes de terceiros"},
        {"licenseText", "LuckyWheel 授權全文|LuckyWheel 许可证全文|LuckyWheel license text|LuckyWheel ライセンス全文|LuckyWheel 라이선스 전문|Texto de la licencia de LuckyWheel|Texte de la licence LuckyWheel|LuckyWheel-Lizenztext|Texto da licença do LuckyWheel"},
        {"openLicenses", "開啟授權資料夾|打开许可证文件夹|Open licenses folder|ライセンスフォルダーを開く|라이선스 폴더 열기|Abrir carpeta de licencias|Ouvrir le dossier des licences|Lizenzordner öffnen|Abrir pasta de licenças"},
        {"aboutQt", "關於 Qt|关于 Qt|About Qt|Qt について|Qt 정보|Acerca de Qt|À propos de Qt|Über Qt|Sobre o Qt"},
        {"close", "關閉|关闭|Close|閉じる|닫기|Cerrar|Fermer|Schließen|Fechar"},
        {"licensesMissing", "找不到授權資料夾。安裝版與免安裝 ZIP 均附完整授權文件；自行建置時請參考原始碼的 licenses 與 THIRD-PARTY.md。|找不到许可证文件夹。安装版与免安装 ZIP 均附完整许可证文件；自行构建时请参考源代码的 licenses 与 THIRD-PARTY.md。|Licenses folder not found. The installer and portable ZIP include all license files; for your own build, see licenses and THIRD-PARTY.md in the source.|ライセンスフォルダーが見つかりません。インストーラーとポータブルZIPには全ライセンスファイルが含まれます。自分でビルドした場合はソースのlicensesとTHIRD-PARTY.mdを参照してください。|라이선스 폴더를 찾을 수 없습니다. 설치 프로그램과 포터블 ZIP에는 모든 라이선스 파일이 포함됩니다. 직접 빌드한 경우 소스의 licenses와 THIRD-PARTY.md를 참고하세요.|No se encontró la carpeta de licencias. El instalador y el ZIP portátil incluyen todas las licencias; si compiló usted mismo, consulte licenses y THIRD-PARTY.md en el código fuente.|Dossier des licences introuvable. L’installateur et le ZIP portable contiennent toutes les licences ; pour votre propre build, voyez licenses et THIRD-PARTY.md dans les sources.|Lizenzordner nicht gefunden. Installer und portables ZIP enthalten alle Lizenzdateien; bei eigenem Build siehe licenses und THIRD-PARTY.md im Quellcode.|Pasta de licenças não encontrada. O instalador e o ZIP portátil incluem todas as licenças; em uma compilação própria, veja licenses e THIRD-PARTY.md no código-fonte."},
        {"simulateShort", "獨立模擬（不扣庫存）|独立模拟（不扣库存）|Simulation (no stock changes)|シミュレーション（在庫変更なし）|시뮬레이션 (재고 변경 없음)|Simulación (sin cambiar stock)|Simulation (stock inchangé)|Simulation (Bestand unverändert)|Simulação (sem alterar estoque)"},
        {"saveSuccess", "獎品設定已套用並保存，抽獎將使用此設定。|奖品设置已应用并保存，抽奖将使用此设置。|Prize settings applied and saved. Draws will use these settings.|賞品設定を適用して保存しました。抽選はこの設定を使用します。|상품 설정을 적용하고 저장했습니다. 추첨에 이 설정을 사용합니다.|Premios aplicados y guardados. Los sorteos usarán estos ajustes.|Lots appliqués et enregistrés. Les tirages utiliseront ces réglages.|Preise angewendet und gespeichert. Ziehungen nutzen diese Einstellungen.|Prêmios aplicados e salvos. Os sorteios usarão estes ajustes."},
        {"formatJson", "JSON schema=1。prizes 含唯一 id、name、weight、limit、remaining、color。weight 是整數，100000=10%、1000000=100%。volume 0～100；theme light/dark；language 使用現有九語代碼；sound/showSoldOut 是布林。載入保留目前輪次與設定鎖；保存 ID、庫存及顏色。範例不含密碼。|JSON schema=1。prizes 含唯一 id、name、weight、limit、remaining、color。weight 是整数，100000=10%、1000000=100%。volume 0～100；theme light/dark；language 使用现有九语代码；sound/showSoldOut 是布尔。载入保留当前轮次和设置锁；保存 ID、库存和颜色。示例不含密码。|JSON schema=1. prizes: unique id, name, weight, limit, remaining, color. Integer weight: 100000=10%, 1000000=100%. volume 0–100; theme light/dark; language uses the nine existing codes; sound/showSoldOut are booleans. Loading retains current round and settings lock; preserves IDs, stock and colors. Examples omit passwords.|JSON schema=1。prizesは一意id、name、weight、limit、remaining、color。weightは整数、100000=10%、1000000=100%。volume 0～100、theme light/dark、languageは既存9言語コード、sound/showSoldOutは真偽値。読込は現ラウンドとロックを保持し、ID・在庫・色を保存。例にパスワードは含みません。|JSON schema=1. prizes: 고유 id, name, weight, limit, remaining, color. 정수 weight: 100000=10%, 1000000=100%. volume 0~100, theme light/dark, language는 기존 9개 코드, sound/showSoldOut은 불리언. 현재 회차와 잠금을 유지하며 ID·재고·색상을 보존합니다. 예제에 비밀번호는 없습니다.|JSON schema=1. prizes: id único, name, weight, limit, remaining, color. weight entero: 100000=10%, 1000000=100%. volume 0–100; theme light/dark; language: nueve códigos existentes; sound/showSoldOut: booleanos. La carga conserva ronda y bloqueo actuales, ID, stock y colores. Ejemplos sin contraseñas.|JSON schema=1. prizes : id unique, name, weight, limit, remaining, color. weight entier : 100000=10%, 1000000=100%. volume 0–100 ; theme light/dark ; language : neuf codes existants ; sound/showSoldOut : booléens. Le chargement conserve le tour et le verrou actuels, ID, stock et couleurs. Exemples sans mot de passe.|JSON schema=1. prizes: eindeutige id, name, weight, limit, remaining, color. weight ganzzahlig: 100000=10%, 1000000=100%. volume 0–100; theme light/dark; language: neun vorhandene Codes; sound/showSoldOut: boolesch. Laden behält aktuelle Runde und Sperre sowie IDs, Bestand und Farben. Beispiele ohne Passwörter.|JSON schema=1. prizes: id único, name, weight, limit, remaining, color. weight inteiro: 100000=10%, 1000000=100%. volume 0–100; theme light/dark; language: nove códigos existentes; sound/showSoldOut: booleanos. Carregar mantém rodada e bloqueio atuais, IDs, estoque e cores. Exemplos sem senha."},
        {"formatCsv", "CSV 標頭：name,probability,limit,color，可加第五欄 remaining，順序固定。probability 為百分比數字（不含 %），最多四位小數。省略 remaining 時設為 limit；CSV 產生新 ID，不含全域設定。逗號／引號欄位以雙引號包住，內部引號寫兩次。獎品 CSV 與歷史 CSV 不同。|CSV 标头：name,probability,limit,color，可加第五列 remaining，顺序固定。probability 为百分比数字（不含 %），最多四位小数。省略 remaining 时设为 limit；CSV 生成新 ID，不含全局设置。逗号／引号字段以双引号包住，内部引号写两次。奖品 CSV 与历史 CSV 不同。|CSV headers: name,probability,limit,color; optional fifth column remaining. Fixed order. probability: percent without %, up to four decimals. Without remaining, stock resets to limit. CSV creates new IDs and omits global settings. Quote fields containing commas or quotes; double internal quotes. Prize CSV differs from history CSV.|CSV列はname,probability,limit,color、任意の5列目remaining。順序固定。probabilityは%記号なしの百分率、小数4桁まで。remaining省略時はlimitに設定。IDは新規生成、全体設定は含みません。カンマや引用符を含む欄は二重引用符で囲み、内部の引用符は2つ記述。履歴CSVとは別形式。|CSV 열: name,probability,limit,color 및 선택적 다섯 번째 remaining. 순서 고정. probability는 % 없이 소수 4자리까지. remaining을 생략하면 재고는 limit으로 설정됩니다. 새 ID 생성, 전체 설정 제외. 쉼표·따옴표 필드는 큰따옴표로 감싸고 내부 따옴표는 두 번 씁니다. 기록 CSV와 다릅니다.|Cabeceras: name,probability,limit,color; quinta columna opcional remaining. Orden fijo. probability: porcentaje sin %, hasta cuatro decimales. Sin remaining, stock=limit. Se crean ID nuevos, sin ajustes globales. Entrecomille campos con comas o comillas; duplique comillas internas. Difiere del CSV del historial.|Colonnes : name,probability,limit,color ; cinquième facultative remaining. Ordre fixe. probability : pourcentage sans %, quatre décimales maximum. Sans remaining, stock=limit. Nouveaux ID, sans réglages globaux. Entourez de guillemets les champs avec virgules ou guillemets ; doublez les guillemets internes. Format différent du CSV historique.|Spalten: name,probability,limit,color; optional fünfte Spalte remaining. Feste Reihenfolge. probability: Prozent ohne %, maximal vier Dezimalstellen. Ohne remaining gilt Bestand=limit. Neue IDs, keine globalen Einstellungen. Felder mit Kommas oder Anführungszeichen in Anführungszeichen setzen; innere verdoppeln. Anderes Format als Verlauf-CSV.|Colunas: name,probability,limit,color; quinta opcional remaining. Ordem fixa. probability: porcentagem sem %, até quatro casas decimais. Sem remaining, estoque=limit. IDs novos, sem ajustes globais. Use aspas em campos com vírgulas ou aspas; duplique aspas internas. Formato diferente do CSV de histórico."},
        {"formatShared", "UTF-8；1～200 個獎品，名稱 1～120 字且不可換行，顏色 #RRGGBB。確認匯入後取代獎品；確認前不寫入。|UTF-8；1～200 个奖品，名称 1～120 字且不可换行，颜色 #RRGGBB。确认导入后替换奖品；确认前不写入。|UTF-8; 1–200 prizes. Names: 1–120 characters, single line. Color: #RRGGBB. Import replaces prizes after confirmation; nothing is written beforehand.|UTF-8、賞品1～200個。名前は1～120文字、改行不可。色は#RRGGBB。確認後に賞品を置換し、確認前は保存しません。|UTF-8, 상품 1~200개. 이름 1~120자, 줄 바꿈 금지. 색상 #RRGGBB. 확인 후 상품을 교체하며 그 전에는 저장하지 않습니다.|UTF-8; 1–200 premios. Nombre: 1–120 caracteres, una línea. Color: #RRGGBB. Se sustituyen los premios al confirmar; antes no se guarda nada.|UTF-8 ; 1–200 lots. Nom : 1–120 caractères, une ligne. Couleur : #RRGGBB. Les lots sont remplacés après confirmation ; rien n’est écrit avant.|UTF-8; 1–200 Preise. Namen: 1–120 Zeichen, einzeilig. Farbe: #RRGGBB. Import ersetzt Preise erst nach Bestätigung.|UTF-8; 1–200 prêmios. Nome: 1–120 caracteres, uma linha. Cor: #RRGGBB. A importação substitui prêmios após confirmação; antes nada é salvo."},
        {"stockRules", "機率總和 ≤100%，最多四位小數；未分配及售罄機率轉為未中獎，其他獎品機率不變。總數與剩餘填 -1/-1 為不限量；否則 0≤剩餘≤總數。每次抽獎先交易保存結果並扣庫存，再播放動畫；關閉動畫不退回庫存。重置恢復總數並開新一輪，保留歷史。|概率总和 ≤100%，最多四位小数；未分配和售罄概率转为未中奖，其他奖品概率不变。总数和剩余填 -1/-1 为不限量；否则 0≤剩余≤总数。抽奖先保存结果并扣库存，再播放动画；关闭动画不退回库存。重置恢复总数并开新一轮，保留历史。|Total probability ≤100%, up to four decimals. Unassigned and sold-out probability becomes no prize; other odds stay unchanged. Use -1/-1 for unlimited stock; otherwise 0≤remaining≤limit. Each draw commits its result and stock before animation; closing does not refund stock. Reset restores limits, starts a new round and retains history.|確率合計は100%以下、小数点以下4桁まで。未割当と品切れの確率ははずれとなり、他の確率は変わりません。上限/残数が-1/-1なら無制限。それ以外は0≤残数≤上限。結果と在庫はアニメーション前に保存され、閉じても在庫は戻りません。リセットで上限まで補充し新ラウンドを開始。履歴は保持。|확률 합계는 100% 이하, 소수점 4자리까지입니다. 미배정·소진 확률은 미당첨으로 전환되고 다른 확률은 유지됩니다. -1/-1은 무제한, 그 외에는 0≤남은 수량≤한도입니다. 애니메이션 전에 결과와 재고를 저장하며 닫아도 재고는 복원되지 않습니다. 초기화하면 한도까지 보충하고 새 회차를 시작하며 기록은 유지됩니다.|Probabilidad total ≤100%, hasta cuatro decimales. La parte libre o agotada pasa a sin premio; las demás no cambian. -1/-1 indica stock ilimitado; si no, 0≤restantes≤límite. Se guarda el resultado y se descuenta stock antes de animar; cerrar no lo repone. Reiniciar repone los límites, abre otra ronda y conserva el historial.|Probabilité totale ≤100%, quatre décimales maximum. La part libre ou épuisée devient sans lot ; les autres restent inchangées. -1/-1 signifie stock illimité ; sinon 0≤restants≤limite. Résultat et stock sont enregistrés avant l’animation ; fermer ne restitue pas le stock. Réinitialiser restaure les limites et ouvre un nouveau tour, avec historique conservé.|Gesamtwahrscheinlichkeit ≤100%, höchstens vier Dezimalstellen. Freie und ausverkaufte Anteile werden zu kein Gewinn; andere Chancen bleiben gleich. -1/-1 bedeutet unbegrenzt, sonst 0≤Restbestand≤Limit. Ergebnis und Bestand werden vor der Animation gespeichert; Schließen gibt keinen Bestand zurück. Zurücksetzen füllt bis zum Limit auf und startet eine neue Runde; Verlauf bleibt erhalten.|Probabilidade total ≤100%, até quatro casas decimais. A parte livre ou esgotada vira sem prêmio; as demais não mudam. -1/-1 indica estoque ilimitado; caso contrário 0≤restantes≤limite. Resultado e estoque são salvos antes da animação; fechar não devolve estoque. Repor restaura limites e inicia nova rodada, mantendo o histórico."},
        {"duplicate_id", "獎品 ID 不可空白或重複。|奖品 ID 不可空白或重复。|Prize IDs must be nonempty and unique.|賞品IDは空欄不可、重複不可です。|상품 ID는 비어 있거나 중복될 수 없습니다.|Los ID de premios deben ser únicos y no vacíos.|Les ID des lots doivent être uniques et non vides.|Preis-IDs müssen eindeutig und nicht leer sein.|IDs dos prêmios devem ser únicos e não vazios."},
        {"invalid_lock", "設定鎖資料無效。請檢查配置檔。|设置锁数据无效。请检查配置文件。|Invalid settings lock data. Check the configuration file.|設定ロックのデータが不正です。設定ファイルを確認してください。|설정 잠금 데이터가 잘못되었습니다. 설정 파일을 확인하세요.|Bloqueo inválido. Revise el archivo de configuración.|Verrouillage invalide. Vérifiez le fichier de configuration.|Ungültige Sperrdaten. Konfigurationsdatei prüfen.|Dados de bloqueio inválidos. Verifique a configuração."},
        {"invalid_theme", "外觀須為 light 或 dark。|外观须为 light 或 dark。|Theme must be light or dark.|テーマはlightまたはdarkです。|테마는 light 또는 dark여야 합니다.|El tema debe ser light o dark.|Le thème doit être light ou dark.|Darstellung muss light oder dark sein.|Tema deve ser light ou dark."},
        {"invalid_language", "不支援的介面語言。|不支持的界面语言。|Unsupported interface language.|未対応の表示言語です。|지원하지 않는 인터페이스 언어입니다.|Idioma no compatible.|Langue non prise en charge.|Nicht unterstützte Sprache.|Idioma não suportado."},
        {"invalid_settings", "設定數值無效。輪次須至少為 1，音量須為 0～100。|设置数值无效。轮次须至少为 1，音量须为 0～100。|Invalid settings: round must be at least 1; volume must be 0–100.|設定値が不正です。ラウンドは1以上、音量は0～100です。|잘못된 설정: 회차는 1 이상, 음량은 0~100입니다.|Ajustes inválidos: ronda mínima 1; volumen 0–100.|Réglages invalides : tour ≥1 ; volume 0–100.|Ungültige Einstellungen: Runde mindestens 1; Lautstärke 0–100.|Ajustes inválidos: rodada mínima 1; volume 0–100."},
        {"rowError", "請檢查第 %1 列獎品設定。|请检查第 %1 行奖品设置。|Check prize row %1.|賞品の%1行目を確認してください。|상품 %1행을 확인하세요.|Revise el premio de la fila %1.|Vérifiez le lot à la ligne %1.|Preis in Zeile %1 prüfen.|Verifique o prêmio na linha %1."},
        {"fileTooLarge", "檔案超過 2 MiB 上限，請縮小檔案後重試。|文件超过 2 MiB 上限，请缩小文件后重试。|File exceeds the 2 MiB limit. Reduce its size and retry.|ファイルは上限の2 MiBを超えています。小さくして再試行してください。|파일이 2 MiB 한도를 초과합니다. 크기를 줄여 다시 시도하세요.|El archivo supera 2 MiB. Reduzca su tamaño y reintente.|Le fichier dépasse 2 MiB. Réduisez sa taille et réessayez.|Datei überschreitet 2 MiB. Verkleinern und erneut versuchen.|O arquivo excede 2 MiB. Reduza o tamanho e tente novamente."},
        {"fileFailed", "檔案操作失敗。請檢查路徑、權限及可用空間。|文件操作失败。请检查路径、权限和可用空间。|File operation failed. Check the path, permissions and free space.|ファイル操作に失敗しました。パス、権限、空き容量を確認してください。|파일 작업 실패. 경로, 권한 및 여유 공간을 확인하세요.|Error de archivo. Revise la ruta, los permisos y el espacio libre.|Échec de l’opération sur le fichier. Vérifiez le chemin, les droits et l’espace libre.|Dateivorgang fehlgeschlagen. Pfad, Rechte und freien Speicher prüfen.|Falha na operação de arquivo. Verifique caminho, permissões e espaço livre."},
        {"saveFailed", "保存失敗；目前草稿保留。請檢查資料目錄權限與可用空間後重試。|保存失败；当前草稿保留。请检查目录权限和可用空间后重试。|Save failed; the draft is retained. Check data folder permissions and free space, then retry.|保存に失敗しました。下書きは保持されます。フォルダーの権限と空き容量を確認して再試行してください。|저장 실패. 초안은 유지됩니다. 데이터 폴더 권한과 여유 공간을 확인하고 다시 시도하세요.|No se pudo guardar; se conserva el borrador. Revise permisos y espacio libre e inténtelo de nuevo.|Échec de l’enregistrement ; le brouillon est conservé. Vérifiez les droits et l’espace libre, puis réessayez.|Speichern fehlgeschlagen; Entwurf bleibt erhalten. Ordnerrechte und freien Speicher prüfen und erneut versuchen.|Falha ao salvar; o rascunho foi mantido. Verifique permissões e espaço livre e tente novamente."},
        {"colorHint", "雙擊顏色欄可選色；HEX 可直接編輯。改色後請套用並保存。|双击颜色栏可选色；HEX 可直接编辑。改色后请应用并保存。|Double-click a color to choose it; HEX stays editable. Apply & save color changes.|色の欄をダブルクリックして選択。HEXも編集できます。変更後は適用して保存。|색상 칸을 두 번 클릭해 선택하세요. HEX도 편집할 수 있습니다. 변경 후 적용 및 저장하세요.|Doble clic en el color para elegirlo; HEX sigue editable. Aplique y guarde los cambios.|Double-cliquez sur une couleur ; HEX reste modifiable. Appliquez et enregistrez les changements.|Farbe doppelt anklicken; HEX bleibt bearbeitbar. Änderungen anwenden und speichern.|Clique duas vezes na cor; HEX pode ser editado. Aplique e salve as alterações."},
        {"stockGroup", "重載與庫存管理|重载与库存管理|Reload & stock|再読込と在庫管理|다시 불러오기 및 재고|Recarga y existencias|Rechargement et stock|Neu laden & Bestand|Recarregar e estoque"},
        {"fileGroup", "匯入、匯出與格式範例|导入、导出与格式示例|Files & format examples|ファイルと形式の例|파일 및 형식 예제|Archivos y ejemplos|Fichiers et exemples|Dateien & Formatbeispiele|Arquivos e exemplos"},
        {"colorGroup", "配色與預覽|配色与预览|Colors & preview|配色とプレビュー|색상 및 미리보기|Colores y vista previa|Couleurs et aperçu|Farben & Vorschau|Cores e prévia"},
        {"moreOptions", "更多選項|更多选项|More options|その他のオプション|더 많은 옵션|Más opciones|Plus d’options|Weitere Optionen|Mais opções"},
        {"sequence", "流水編號|流水编号|Sequence|連番|순번|Secuencia|Numéro|Laufnummer|Sequência"},
        {"chooseColor", "圖形選色|图形选色|Choose color|色を選ぶ|색상 선택|Elegir color|Choisir couleur|Farbe wählen|Escolher cor"},
        {"autoColors", "自動配色|自动配色|Auto colors|自動配色|자동 색상|Colores automáticos|Couleurs auto|Auto-Farben|Cores automáticas"},
        {"defaultColors", "恢復預設配色|恢复默认配色|Default colors|既定の配色|기본 색상|Colores predeterminados|Couleurs par défaut|Standardfarben|Cores padrão"},
        {"draftPreview", "預覽草稿|预览草稿|Preview draft|下書きプレビュー|초안 미리보기|Vista previa|Aperçu du brouillon|Entwurf ansehen|Prévia do rascunho"},
        {"draftPreviewHint", "預覽未保存的獎品設定；不保存、不抽獎。售罄區塊依目前設定顯示。|预览未保存的奖品设置；不保存、不抽奖。售罄区块依当前设置显示。|Preview unsaved prizes without saving or drawing. Sold-out slices follow current settings.|未保存の賞品を表示します。保存や抽選は行いません。品切れ区画は現在の設定に従います。|저장하지 않은 상품을 미리 봅니다. 저장하거나 추첨하지 않습니다. 소진 구간은 현재 설정을 따릅니다.|Vista previa de premios sin guardar ni sortear. Los sectores agotados siguen los ajustes actuales.|Aperçu des lots sans enregistrer ni tirer. Les secteurs épuisés suivent les réglages actuels.|Ungespeicherte Preise ansehen, ohne Speichern oder Ziehung. Ausverkaufte Felder folgen den aktuellen Einstellungen.|Prévia dos prêmios sem salvar ou sortear. Setores esgotados seguem os ajustes atuais."},
        {"selectPrize", "請先選取一個獎品。HEX 顏色也可直接編輯。|请先选择一个奖品。HEX 颜色也可直接编辑。|Select a prize first. HEX color can also be edited directly.|先に賞品を選択してください。HEXの色も直接編集できます。|먼저 상품을 선택하세요. HEX 색상도 직접 편집할 수 있습니다.|Seleccione un premio. También puede editar el color HEX.|Sélectionnez un lot. Vous pouvez aussi modifier sa couleur HEX.|Zuerst einen Preis auswählen. HEX-Farbe ist auch direkt bearbeitbar.|Selecione um prêmio. A cor HEX também pode ser editada."},
        {"exportPrizeCsv", "匯出獎品 CSV|导出奖品 CSV|Export prizes CSV|賞品CSV出力|상품 CSV 내보내기|Exportar premios CSV|Exporter lots CSV|Preise CSV exportieren|Exportar prêmios CSV"},
        {"formatExamples", "匯入格式與範例|导入格式与示例|Import format & examples|形式とサンプル|형식 및 예제|Formato y ejemplos|Format et exemples|Format und Beispiele|Formato e exemplos"},
        {"saveExample", "儲存範例檔|保存示例文件|Save example file|サンプル保存|예제 저장|Guardar ejemplo|Enregistrer exemple|Beispiel speichern|Salvar exemplo"},
        {"showSoldShort", "顯示售罄區塊|显示售罄区块|Show sold-out slices|品切れ区画を表示|소진 구간 표시|Mostrar agotados|Afficher les lots épuisés|Ausverkaufte Felder zeigen|Mostrar esgotados"},
        {"unsavedTitle", "未儲存的變更|未保存的更改|Unsaved changes|未保存の変更|저장하지 않은 변경|Cambios sin guardar|Modifications non enregistrées|Ungespeicherte Änderungen|Alterações não salvas"},
        {"unsavedPrompt", "獎品設定尚未保存。要保存、捨棄變更，或取消此次操作？|奖品设置尚未保存。保存、放弃更改或取消操作？|Prize changes have not been saved. Save, discard changes, or cancel this action?|賞品の変更が未保存です。保存、破棄、または操作をキャンセルしますか？|상품 변경이 저장되지 않았습니다. 저장, 변경 취소 또는 작업 취소?|Hay premios sin guardar. ¿Guardar, descartar o cancelar?|Les lots ont été modifiés. Enregistrer, abandonner ou annuler ?|Preisänderungen sind ungespeichert. Speichern, verwerfen oder abbrechen?|Os prêmios foram alterados. Salvar, descartar ou cancelar?"},
        {"discard", "捨棄變更|放弃更改|Discard changes|変更を破棄|변경 버리기|Descartar cambios|Abandonner les modifications|Änderungen verwerfen|Descartar alterações"},
        {"collapseNav", "收合導覽|收起导航|Collapse navigation|ナビを折りたたむ|탐색 접기|Contraer menú|Réduire le menu|Menü einklappen|Recolher menu"},
        {"expandNav", "展開導覽|展开导航|Expand navigation|ナビを展開|탐색 펼치기|Expandir menú|Développer le menu|Menü ausklappen|Expandir menu"},

        {"clearHistory", "清除歷史紀錄|清除历史记录|Clear history|履歴を消去|기록 삭제|Borrar historial|Effacer l’historique|Verlauf löschen|Limpar histórico"},
        {"clearBackupHint", "建議先到「設定與工具」備份 SQLite；清除後無法復原。|建议先备份 SQLite；清除后无法恢复。|Back up SQLite in Settings & tools first; deletion cannot be undone.|設定から先にSQLiteをバックアップしてください。削除は元に戻せません。|설정에서 SQLite를 먼저 백업하세요. 삭제는 되돌릴 수 없습니다.|Haga una copia SQLite en Ajustes antes; no se puede deshacer.|Sauvegardez SQLite dans Réglages avant ; suppression irréversible.|SQLite zuerst in Einstellungen sichern; Löschen ist endgültig.|Faça backup SQLite em Configurações antes; exclusão irreversível."},
        {"clearFirst", "將清除全部 %1 筆歷史紀錄。建議先到「設定與工具」備份 SQLite，或匯出歷史 CSV。只刪除歷史，不變更獎品配置、剩餘庫存或輪次。是否繼續？|将清除全部 %1 条历史。建议先备份 SQLite 或导出 CSV。奖品、库存和轮次不变。继续？|Clear all %1 records? Back up SQLite or export CSV first. Prizes, stock and round stay unchanged. Continue?|全%1件を消去します。先にSQLiteまたはCSVを保存してください。賞品・在庫・回次は変更しません。続行？|%1개 기록을 삭제합니다. SQLite 또는 CSV를 먼저 백업하세요. 상품, 재고, 회차는 유지됩니다. 계속할까요?|¿Borrar %1 registros? Guarde SQLite o CSV antes. Premios, existencias y ronda se conservan. ¿Continuar?|Effacer %1 entrées ? Sauvegardez SQLite ou CSV avant. Lots, stocks et tour restent inchangés. Continuer ?|%1 Einträge löschen? Zuerst SQLite oder CSV sichern. Preise, Bestand und Runde bleiben erhalten. Fortfahren?|Limpar %1 registros? Salve SQLite ou CSV antes. Prêmios, estoque e rodada permanecem. Continuar?"},
        {"clearFinal", "最後確認：永久刪除 %1 筆歷史紀錄，此操作無法復原。確定清除？|最后确认：永久删除 %1 条历史，无法恢复。确定？|Final confirmation: permanently delete %1 records? This cannot be undone.|最終確認：%1件を完全に削除します。元に戻せません。消去しますか？|최종 확인: %1개 기록을 영구 삭제할까요? 복구할 수 없습니다.|Confirmación final: ¿eliminar %1 registros permanentemente? No se puede deshacer.|Confirmation finale : supprimer définitivement %1 entrées ? Irréversible.|Letzte Bestätigung: %1 Einträge endgültig löschen? Nicht rückgängig zu machen.|Confirmação final: excluir %1 registros permanentemente? Não pode ser desfeito."},
        {"clearDone", "已刪除 %1 筆歷史紀錄。|已删除 %1 条历史。|Deleted %1 history records.|履歴%1件を削除しました。|기록 %1개를 삭제했습니다.|Se eliminaron %1 registros.|%1 entrées supprimées.|%1 Einträge gelöscht.|%1 registros excluídos."},
        {"historyTotals", "總筆數：%1　中獎：%2　未中獎：%3|总笔数：%1　中奖：%2　未中奖：%3|Total: %1   Won: %2   No prize: %3|合計：%1　当選：%2　はずれ：%3|전체: %1   당첨: %2   미당첨: %3|Total: %1   Ganadores: %2   Sin premio: %3|Total : %1   Gains : %2   Sans lot : %3|Gesamt: %1   Gewinne: %2   Kein Gewinn: %3|Total: %1   Ganhos: %2   Sem prêmio: %3"},
        {"history_changed", "歷史紀錄已變更；未刪除任何紀錄，請重新確認筆數。|历史已变更；未删除，请重新确认。|History changed; nothing deleted. Confirm the current count again.|履歴が変更されました。削除せず、件数を再確認してください。|기록이 변경되었습니다. 삭제하지 않았습니다. 다시 확인하세요.|El historial cambió; no se borró nada. Confirme otra vez.|Historique modifié ; rien supprimé. Confirmez à nouveau.|Verlauf geändert; nichts gelöscht. Erneut bestätigen.|Histórico alterado; nada excluído. Confirme novamente."},

        {"zero", "0%：不參與抽獎|0%：不参与抽奖|0%: excluded|0%：抽選対象外|0%: 추첨 제외|0%: excluido|0% : exclu|0%: ausgeschlossen|0%: excluído"},
        {"zeroInfo", "0% 獎品會保存，但不顯示轉盤區塊、不參與抽獎；請設定大於 0% 的機率。|0% 奖品会保存，但不显示转盘区块、不参与抽奖；请设置大于 0% 的概率。|0% prizes are saved but have no wheel slice and cannot win. Set a probability above 0%.|0%の賞品は保存されますが、区画は表示されず当選しません。0%より大きい確率を設定してください。|0% 상품은 저장되지만 구간이 없고 당첨되지 않습니다. 0%보다 큰 확률을 설정하세요.|Los premios al 0% se guardan pero no aparecen en la rueda ni ganan. Asigne más de 0%.|Les lots à 0% sont enregistrés mais sans secteur et sans gain. Définissez plus de 0%.|Preise mit 0% werden gespeichert, haben aber kein Radsegment und gewinnen nicht. Mehr als 0% einstellen.|Prêmios com 0% são salvos, mas não aparecem na roda e não ganham. Defina mais de 0%."},
        {"lastResult", "上次結果|上次结果|Last result|前回の結果|지난 결과|Último resultado|Dernier "
                       "résultat|Letztes Ergebnis|Último resultado"},
        {"ticket", "隨機值|随机值|Random ticket|乱数値|난수 값|Valor aleatorio|Valeur "
                   "aléatoire|Zufallswert|Valor aleatório"},
        {"title", "幸運轉盤|幸运转盘|Lucky Wheel|ラッキールーレット|행운의 룰렛|Ruleta "
                  "de la suerte|Roue de la chance|Glücksrad|Roleta da sorte"},
        {"wheel", "抽獎|抽奖|Draw|抽選|추첨|Sorteo|Tirage|Ziehung|Sorteio"},
        {"prizes", "獎品設定|奖品设置|Prizes|賞品設定|상품 설정|Premios|Lots|Preise|Prêmios"},
        {"history", "歷史紀錄|历史记录|History|履歴|기록|Historial|Historique|Verlauf|Histórico"},
        {"settings", "設定與工具|设置与工具|Settings & tools|設定とツール|설정 및 도구|Ajustes y "
                     "herramientas|Réglages et outils|Einstellungen & Werkzeuge|Configurações e ferramentas"},
        {"spin", "開始抽獎|开始抽奖|Draw|抽選開始|추첨 시작|Sortear|Tirer|Drehen|Sortear"},
        {"ready", "準備開始|准备开始|Ready|準備完了|준비 완료|Listo|Prêt|Bereit|Pronto"},
        {"spinning", "轉動中…|转动中…|Spinning…|回転中…|회전 중…|Girando…|En rotation…|Dreht sich…|Girando…"},
        {"miss", "未中獎|未中奖|No prize|はずれ|미당첨|Sin premio|Sans lot|Kein Gewinn|Sem prêmio"},
        {"sold", "售罄|售罄|Sold out|品切れ|소진|Agotado|Épuisé|Ausverkauft|Esgotado"},
        {"showSold", "顯示售罄區塊（計入未中獎）|显示售罄区块（计入未中奖）|Show sold-out slices (no "
                     "prize)|品切れ区画を表示（はずれ）|소진 구간 표시 (미당첨)|Mostrar agotados (sin "
                     "premio)|Afficher les lots épuisés (sans lot)|Ausverkaufte Felder zeigen (kein "
                     "Gewinn)|Mostrar esgotados (sem prêmio)"},
        {"name", "名稱|名称|Name|名前|이름|Nombre|Nom|Name|Nome"},
        {"prob", "機率 %|概率 %|Probability %|確率 %|확률 %|Probabilidad %|Probabilité %|Wahrscheinlichkeit "
                 "%|Probabilidade %"},
        {"limit", "總數（-1＝不限量）|总数（-1＝不限量）|Limit (-1 = unlimited)|上限（-1＝無制限）|한도 (-1 "
                  "= 무제한)|Límite (-1 = ilimitado)|Limite (-1 = illimité)|Limit (-1 = unbegrenzt)|Limite "
                  "(-1 = ilimitado)"},
        {"remaining", "剩餘數量|剩余数量|Remaining|残数|남은 수량|Restantes|Restants|Verbleibend|Restantes"},
        {"color", "顏色|颜色|Color|色|색상|Color|Couleur|Farbe|Cor"},
        {"add", "新增獎品|新增奖品|Add prize|賞品追加|상품 추가|Añadir premio|Ajouter un lot|Preis "
                "hinzufügen|Adicionar prêmio"},
        {"remove", "刪除選取|删除选中|Remove selected|選択項目を削除|선택 삭제|Eliminar selección|Supprimer "
                   "la sélection|Auswahl entfernen|Remover seleção"},
        {"save", "套用並保存|应用并保存|Apply & save|適用して保存|적용 및 저장|Aplicar y guardar|Appliquer "
                 "et enregistrer|Anwenden & speichern|Aplicar e salvar"},
        {"reload", "重載已保存設定|重载已保存设置|Reload saved settings|保存設定を再読込|저장 설정 다시 "
                   "불러오기|Recargar ajustes|Recharger les réglages|Gespeicherte Einstellungen "
                   "laden|Recarregar configurações"},
        {"reset", "重置數量／新一輪|重置数量／新一轮|Reset stock / new round|在庫リセット／新ラウンド|수량 "
                  "초기화 / 새 라운드|Reiniciar stock / nueva ronda|Réinitialiser / nouvelle série|Bestand "
                  "zurücksetzen / neue Runde|Repor estoque / nova rodada"},
        {"import", "匯入 CSV（預覽）|导入 CSV（预览）|Import CSV (preview)|CSV取込（プレビュー）|CSV "
                   "가져오기 (미리보기)|Importar CSV (vista previa)|Importer CSV (aperçu)|CSV importieren "
                   "(Vorschau)|Importar CSV (prévia)"},
        {"exportConfig", "匯出配置 JSON|导出配置 JSON|Export configuration JSON|設定JSONを出力|설정 JSON "
                         "내보내기|Exportar ajustes JSON|Exporter la configuration JSON|Konfiguration als "
                         "JSON exportieren|Exportar configuração JSON"},
        {"importConfig",
         "載入配置 JSON|载入配置 JSON|Load configuration JSON|設定JSONを読込|설정 JSON 불러오기|Cargar "
         "ajustes JSON|Charger une configuration JSON|JSON-Konfiguration laden|Carregar configuração JSON"},
        {"exportCsv",
         "匯出歷史 CSV|导出历史 CSV|Export history CSV|履歴CSVを出力|기록 CSV 내보내기|Exportar historial "
         "CSV|Exporter l'historique CSV|Verlauf als CSV exportieren|Exportar histórico CSV"},
        {"time", "時間（本機）|时间（本机）|Time (local)|時刻（ローカル）|시간 (로컬)|Hora (local)|Heure "
                 "(locale)|Zeit (lokal)|Hora (local)"},
        {"round", "輪次|轮次|Round|ラウンド|라운드|Ronda|Série|Runde|Rodada"},
        {"result", "結果|结果|Result|結果|결과|Resultado|Résultat|Ergebnis|Resultado"},
        {"id",
         "流水號|流水号|Record ID|記録ID|기록 ID|ID de registro|ID du tirage|Datensatz-ID|ID do registro"},
        {"language", "介面語言|界面语言|Language|言語|언어|Idioma|Langue|Sprache|Idioma"},
        {"sound", "啟用音效|启用音效|Enable sound|効果音を有効化|효과음 활성화|Activar sonido|Activer le "
                  "son|Ton aktivieren|Ativar som"},
        {"volume", "音量|音量|Volume|音量|볼륨|Volumen|Volume|Lautstärke|Volume"},
        {"theme", "外觀|外观|Theme|テーマ|테마|Tema|Thème|Darstellung|Tema"},
        {"light", "明亮|明亮|Light|ライト|밝게|Claro|Clair|Hell|Claro"},
        {"dark", "深色|深色|Dark|ダーク|어둡게|Oscuro|Sombre|Dunkel|Escuro"},
        {"fullscreen", "切換全螢幕（F11）|切换全屏（F11）|Toggle fullscreen (F11)|全画面切替（F11）|전체 "
                       "화면 전환 (F11)|Pantalla completa (F11)|Plein écran (F11)|Vollbild umschalten "
                       "(F11)|Alternar tela cheia (F11)"},
        {"backup", "備份資料庫|备份数据库|Back up database|データベースをバックアップ|데이터베이스 "
                   "백업|Copia de seguridad|Sauvegarder la base|Datenbank sichern|Backup do banco de dados"},
        {"restore", "還原資料庫|还原数据库|Restore database|データベースを復元|데이터베이스 복원|Restaurar "
                    "base de datos|Restaurer la base|Datenbank wiederherstellen|Restaurar banco de dados"},
        {"simulate",
         "獨立模擬（不扣庫存）|独立模拟（不扣库存）|Independent simulation (no stock "
         "changes)|独立シミュレーション（在庫変更なし）|독립 시뮬레이션 (재고 변경 없음)|Simulación "
         "independiente (sin cambiar stock)|Simulation indépendante (stock inchangé)|Unabhängige Simulation "
         "(Bestand unverändert)|Simulação independente (sem alterar estoque)"},
        {"statistics", "統計|统计|Statistics|統計|통계|Estadísticas|Statistiques|Statistik|Estatísticas"},
        {"lock", "鎖定設定|锁定设置|Lock settings|設定をロック|설정 잠금|Bloquear ajustes|Verrouiller les "
                 "réglages|Einstellungen sperren|Bloquear configurações"},
        {"unlock", "解鎖設定|解锁设置|Unlock settings|設定ロック解除|설정 잠금 해제|Desbloquear "
                   "ajustes|Déverrouiller les réglages|Einstellungen entsperren|Desbloquear configurações"},
        {"password",
         "密碼（至少 4 字元）|密码（至少 4 字符）|Password (at least 4 "
         "characters)|パスワード（4文字以上）|비밀번호 (4자 이상)|Contraseña (mínimo 4 caracteres)|Mot de "
         "passe (4 caractères minimum)|Passwort (mindestens 4 Zeichen)|Senha (mínimo 4 caracteres)"},
        {"confirmPassword",
         "再次輸入密碼|再次输入密码|Confirm password|パスワードを再入力|비밀번호 확인|Confirmar "
         "contraseña|Confirmer le mot de passe|Passwort bestätigen|Confirmar senha"},
        {"wrongPassword", "密碼錯誤或不一致|密码错误或不一致|Incorrect or mismatched "
                          "password|パスワードが違うか一致しません|비밀번호가 틀리거나 일치하지 "
                          "않습니다|Contraseña incorrecta o distinta|Mot de passe incorrect ou "
                          "différent|Passwort falsch oder nicht identisch|Senha incorreta ou diferente"},
        {"confirm", "確認|确认|Confirm|確認|확인|Confirmar|Confirmer|Bestätigen|Confirmar"},
        {"cancel", "取消|取消|Cancel|キャンセル|취소|Cancelar|Annuler|Abbrechen|Cancelar"},
        {"error", "操作失敗|操作失败|Operation failed|操作失敗|작업 실패|Error de operación|Échec de "
                  "l'opération|Vorgang fehlgeschlagen|Falha na operação"},
        {"saved", "已保存|已保存|Saved|保存しました|저장됨|Guardado|Enregistré|Gespeichert|Salvo"},
        {"preview",
         "匯入預覽：確認後取代獎品設定|导入预览：确认后替换奖品设置|Import preview: confirm to replace "
         "prizes|取込プレビュー：確認後に賞品設定を置換|가져오기 미리보기: 확인하면 상품 설정을 "
         "교체합니다|Vista previa: confirmar reemplaza los premios|Aperçu : confirmer remplace les "
         "lots|Vorschau: Bestätigung ersetzt die Preise|Prévia: confirmar substitui os prêmios"},
        {"resetConfirm",
         "將恢復各獎品總數並開始新一輪，歷史紀錄保留。|将恢复各奖品总数并开始新一轮，历史记录保留。|Restore "
         "all stock and start a new round? History is "
         "retained.|在庫を上限まで戻して新ラウンドを開始します。履歴は保持されます。|재고를 복원하고 새 "
         "라운드를 시작합니다. 기록은 유지됩니다.|¿Reponer stock y empezar otra ronda? Se conserva el "
         "historial.|Réinitialiser le stock et commencer une série ? L'historique est conservé.|Bestand "
         "auffüllen und neue Runde starten? Verlauf bleibt erhalten.|Repor estoque e iniciar nova rodada? O "
         "histórico será mantido."},
        {"restoreConfirm",
         "還原將取代目前配置、庫存與歷史。會先自動備份目前資料。|"
         "还原将替换当前配置、库存与历史。会先自动备份当前数据。|Restore replaces settings, stock and "
         "history. Current data will be backed up "
         "first.|復元は設定・在庫・履歴を置換します。現在のデータを先にバックアップします。|복원하면 설정, "
         "재고, 기록이 교체됩니다. 현재 데이터를 먼저 백업합니다.|Restaurar reemplaza ajustes, stock e "
         "historial. Primero se crea una copia.|La restauration remplace réglages, stock et historique. Une "
         "sauvegarde sera créée.|Wiederherstellung ersetzt Einstellungen, Bestand und Verlauf. Zuerst wird "
         "gesichert.|A restauração substitui ajustes, estoque e histórico. Um backup será criado antes."},
        {"count", "次數|次数|Count|回数|횟수|Cantidad|Nombre|Anzahl|Quantidade"},
        {"total", "抽獎總次數|抽奖总次数|Total draws|総抽選回数|총 추첨 횟수|Sorteos totales|Total des "
                  "tirages|Ziehungen insgesamt|Total de sorteios"},
        {"rate", "實際比例 %|实际比例 %|Observed %|観測割合 %|관측 비율 %|Porcentaje observado|Pourcentage "
                 "observé|Beobachtet %|Porcentagem observada"},
        {"simulationInfo",
         "依目前有效機率獨立取樣；不消耗庫存、不產生正式紀錄，不要求結果湊滿比例。|"
         "依当前有效概率独立采样；不消耗库存、不产生正式记录，不要求结果凑满比例。|Independent samples from "
         "current effective probabilities. No stock or history changes; counts are not forced to match "
         "percentages.|現在の有効確率から独立抽選。在庫や正式履歴は変更せず、割合に合わせる補正もしません。|"
         "현재 유효 확률로 독립 추첨합니다. 재고와 공식 기록은 변경하지 않으며 비율을 강제로 맞추지 "
         "않습니다.|Muestras independientes con probabilidades actuales. No cambia stock ni historial ni "
         "fuerza porcentajes.|Échantillons indépendants selon les probabilités actuelles. Stock et "
         "historique inchangés, sans correction des résultats.|Unabhängige Ziehungen mit aktuellen "
         "Wahrscheinlichkeiten. Keine Bestands- oder Verlaufsänderung, keine erzwungenen Quoten.|Amostras "
         "independentes das probabilidades atuais. Sem alterar estoque ou histórico nem forçar proporções."},
        {"probability_over_100",
         "機率總和不得超過 100%。|概率总和不得超过 100%。|Total probability must not exceed "
         "100%.|確率の合計は100%以下にしてください。|확률 합계는 100% 이하여야 합니다.|La probabilidad total "
         "no puede superar 100%.|La probabilité totale ne doit pas dépasser 100 %.|Gesamtwahrscheinlichkeit "
         "darf 100 % nicht übersteigen.|A probabilidade total não pode ultrapassar 100%."},
        {"invalid_probability",
         "機率須為 0～100，最多四位小數。|概率须为 0～100，最多四位小数。|Probability must be 0–100 with at "
         "most four decimal places.|確率は0～100、小数点以下4桁までです。|확률은 0~100이며 소수점 이하 "
         "4자리까지입니다.|Probabilidad de 0 a 100, máximo cuatro decimales.|Probabilité de 0 à 100, quatre "
         "décimales maximum.|Wahrscheinlichkeit 0–100 mit höchstens vier Nachkommastellen.|Probabilidade de "
         "0 a 100, no máximo quatro casas decimais."},
        {"invalid_quantity",
         "數量須為整數，剩餘數量不可超過總數；不限量請填 -1。|数量须为整数，剩余数量不可超过总数；不限量请填 "
         "-1。|Use integer quantities; remaining cannot exceed the limit. Use -1 for "
         "unlimited.|数量は整数で、残数は上限以下にしてください。無制限は-1です。|수량은 정수이며 남은 "
         "수량은 한도 이하여야 합니다. 무제한은 -1입니다.|Use enteros; restantes no puede superar el límite. "
         "-1 indica ilimitado.|Utilisez des entiers ; le reste ne peut dépasser la limite. -1 pour "
         "illimité.|Ganzzahlen verwenden; Restbestand darf Limit nicht übersteigen. -1 für unbegrenzt.|Use "
         "inteiros; restantes não pode superar o limite. -1 para ilimitado."},
        {"invalid_name",
         "名稱不可空白或換行，最多 120 字元。|名称不可空白或换行，最多 120 字符。|Names must be nonempty, "
         "single-line and at most 120 characters.|名前は空欄不可、改行不可、120文字以内です。|이름은 비어 "
         "있으면 안 되며 한 줄, 최대 120자입니다.|Nombre no vacío, una línea, máximo 120 caracteres.|Nom non "
         "vide, une ligne, 120 caractères maximum.|Name erforderlich, einzeilig, höchstens 120 Zeichen.|Nome "
         "obrigatório, uma linha, no máximo 120 caracteres."},
        {"prize_count", "獎品數量須為 1～200。|奖品数量须为 1～200。|Use 1–200 "
                        "prizes.|賞品数は1～200です。|상품 수는 1~200개입니다.|Use de 1 a 200 "
                        "premios.|Utilisez 1 à 200 lots.|1–200 Preise verwenden.|Use de 1 a 200 prêmios."},
        {"invalid_color",
         "請使用有效顏色，例如 #62b5ff。|请使用有效颜色，例如 #62b5ff。|Use a valid color, such as "
         "#62b5ff.|有効な色を入力してください（例：#62b5ff）。|유효한 색상을 입력하세요 (예: #62b5ff).|Use "
         "un color válido, como #62b5ff.|Utilisez une couleur valide, comme #62b5ff.|Gültige Farbe "
         "verwenden, z. B. #62b5ff.|Use uma cor válida, como #62b5ff."},
        {"invalid_csv", "CSV 格式錯誤。欄位須為 name,probability,limit,color[,remaining][,remaining][,remaining][,remaining]。|CSV 格式错误。字段须为 name,probability,limit,color[,remaining][,remaining][,remaining][,remaining]。|Invalid CSV. Columns must be name,probability,limit,color[,remaining][,remaining][,remaining][,remaining].|CSV形式が不正です。列はname,probability,limit,color[,remaining][,remaining][,remaining][,remaining]です。|잘못된 CSV입니다. 열은 name,probability,limit,color[,remaining][,remaining][,remaining][,remaining]여야 합니다.|CSV inválido. Columnas: name,probability,limit,color[,remaining][,remaining][,remaining][,remaining].|CSV invalide. Colonnes : name,probability,limit,color[,remaining][,remaining][,remaining][,remaining].|Ungültige CSV. Spalten: name,probability,limit,color[,remaining][,remaining][,remaining][,remaining].|CSV inválido. Colunas: name,probability,limit,color[,remaining][,remaining][,remaining][,remaining]."},
        {"invalid_backup",
         "備份無效或不相容；目前資料未變更。|备份无效或不兼容；当前数据未更改。|Invalid or incompatible "
         "backup; current data is "
         "unchanged.|バックアップが不正または非互換です。現在のデータは変更されません。|백업이 잘못되었거나 "
         "호환되지 않습니다. 현재 데이터는 변경되지 않았습니다.|Copia inválida o incompatible; los datos "
         "actuales no cambiaron.|Sauvegarde invalide ou incompatible ; données actuelles "
         "inchangées.|Ungültige oder inkompatible Sicherung; aktuelle Daten unverändert.|Backup inválido ou "
         "incompatível; dados atuais não alterados."},
        {"backup_exists",
         "備份檔案已存在，請選擇新的檔名。|备份文件已存在，请选择新文件名。|Backup file already exists. "
         "Choose a new filename.|バックアップが既に存在します。別の名前を選んでください。|백업 파일이 이미 "
         "있습니다. 새 이름을 선택하세요.|La copia ya existe. Elija otro nombre.|La sauvegarde existe déjà. "
         "Choisissez un autre nom.|Sicherung existiert bereits. Neuen Dateinamen wählen.|O backup já existe. "
         "Escolha outro nome."},
        {"genericInvalid",
         "檔案或資料無效，請檢查格式。|文件或数据无效，请检查格式。|Invalid file or data. Check the "
         "format.|ファイルまたはデータが不正です。形式を確認してください。|파일 또는 데이터가 "
         "잘못되었습니다. 형식을 확인하세요.|Archivo o datos inválidos. Revise el formato.|Fichier ou "
         "données invalides. Vérifiez le format.|Ungültige Datei oder Daten. Format prüfen.|Arquivo ou dados "
         "inválidos. Verifique o formato."},
        {"rules", "每次獨立隨機，不保證中獎。未分配及售罄機率均轉未中獎。|"
                  "每次独立随机，不保证中奖。未分配及售罄概率均转未中奖。|Each draw is independent; winning "
                  "is not guaranteed. Unassigned and sold-out probability becomes no "
                  "prize.|毎回独立抽選、当選保証なし。未割当・品切れの確率ははずれになります。|매 추첨은 "
                  "독립적이며 당첨을 보장하지 않습니다. 미배정 및 소진 확률은 미당첨으로 전환됩니다.|Cada "
                  "sorteo es independiente, sin garantía de premio. Probabilidad libre o agotada pasa a sin "
                  "premio.|Chaque tirage est indépendant, sans gain garanti. Probabilités libres ou épuisées "
                  "deviennent sans lot.|Jede Ziehung ist unabhängig, ohne Gewinngarantie. Freie und "
                  "ausverkaufte Wahrscheinlichkeit wird zu kein Gewinn.|Cada sorteio é independente, sem "
                  "garantia de prêmio. Probabilidade livre ou esgotada vira sem prêmio."},
        {"unsaved", "編輯後請套用並保存；抽獎使用已保存設定。|编辑后请应用并保存；抽奖使用已保存设置。|Apply "
                    "and save edits. Draws use saved "
                    "settings.|編集後は適用して保存してください。抽選には保存済み設定を使用します。|편집 후 "
                    "적용 및 저장하세요. 추첨은 저장된 설정을 사용합니다.|Aplique y guarde los cambios. Los "
                    "sorteos usan ajustes guardados.|Appliquez et enregistrez. Les tirages utilisent les "
                    "réglages enregistrés.|Änderungen anwenden und speichern. Ziehungen nutzen gespeicherte "
                    "Einstellungen.|Aplique e salve as edições. Sorteios usam configurações salvas."},
        {"lockedInfo",
         "設定已鎖定；仍可抽獎及查看紀錄。|设置已锁定；仍可抽奖及查看记录。|Settings locked; draws and "
         "history remain available.|設定ロック中。抽選と履歴表示は利用できます。|설정이 잠겼습니다. 추첨과 "
         "기록은 사용할 수 있습니다.|Ajustes bloqueados; sorteos e historial disponibles.|Réglages "
         "verrouillés ; tirages et historique disponibles.|Einstellungen gesperrt; Ziehungen und Verlauf "
         "verfügbar.|Configurações bloqueadas; sorteios e histórico disponíveis."},
        {"data", "資料位置|数据位置|Data location|データ保存先|데이터 위치|Ubicación de datos|Emplacement "
                 "des données|Datenordner|Local dos dados"},
        {"savedConfigConfirm",
         "此操作會取代目前設定，歷史紀錄保留。|此操作会替换当前设置，历史记录保留。|This replaces current "
         "settings. History is retained.|現在の設定を置換します。履歴は保持されます。|현재 설정을 "
         "교체합니다. 기록은 유지됩니다.|Se reemplazan los ajustes. Se conserva el historial.|Les réglages "
         "seront remplacés. L'historique est conservé.|Aktuelle Einstellungen werden ersetzt. Verlauf bleibt "
         "erhalten.|As configurações serão substituídas. O histórico será mantido."},
        {"audioUnavailable",
         "音效無法載入；抽獎仍可使用。|音效无法加载；抽奖仍可使用。|Sound could not load; draws remain "
         "available.|音声を読み込めません。抽選は利用できます。|소리를 불러오지 못했습니다. 추첨은 사용할 수 "
         "있습니다.|No se pudo cargar el sonido; el sorteo sigue disponible.|Le son n'a pas pu être chargé ; "
         "les tirages restent disponibles.|Ton konnte nicht geladen werden; Ziehungen bleiben verfügbar.|Não "
         "foi possível carregar o som; sorteios continuam disponíveis."}};
    int index = codes.indexOf(lang);
    if (index < 0)
        index = 0;
    auto it = strings.constFind(key);
    if (it == strings.cend())
        return key;
    auto values = it.value().split('|');
    return values.value(index, values.value(2, key));
}
