#include "formats.h"
#include <QRegularExpression>
#include <QUuid>
namespace lw {
QColor defaultColor(int row) {
    if (row == 0) return QColor("#72b8ff");
    if (row == 1) return QColor("#a8dca6");
    return QColor::fromHsv((row * 67) % 360, 90, 235);
}
Config exampleConfig() {
    Config c;
    c.prizes = {{"example-a", "特別獎, A", 100000, 3, 3, defaultColor(0)},
                {"example-b", "參加獎 B", 200000, -1, -1, defaultColor(1)}};
    return c;
}
QString prizeCsv(const Config &c) {
    QVector<QStringList> rows{{"name", "probability", "limit", "color", "remaining"}};
    for (const auto &p : c.prizes)
        rows << QStringList{p.name, percent(p.weight), QString::number(p.limit), p.color.name(),
                            QString::number(p.remaining)};
    return encodeCsv(rows);
}
std::optional<Config> importPrizeCsv(const QString &text, const Config &base, QString *error) {
    auto fail = [&](const QString &where, const QString &reason) -> std::optional<Config> {
        if (error) *error = where + ": " + reason;
        return std::nullopt;
    };
    QString e;
    auto rows = decodeCsv(text, &e);
    if (!rows) return fail("CSV", e);
    const QStringList legacy{"name", "probability", "limit", "color"};
    QStringList extended = legacy; extended << "remaining";
    if (rows->size() < 2 || (rows->first() != legacy && rows->first() != extended))
        return fail("CSV line 1 / header", "name,probability,limit,color[,remaining]");
    auto c = base;
    c.prizes.clear();
    int line = 2;
    for (int i = 1; i < rows->size(); ++i) {
        const auto &row = rows->at(i);
        const QString where = QString("CSV line %1").arg(line);
        int consumed = 1;
        for (const auto &field : row) consumed += field.count('\n');
        line += consumed;
        if (row == QStringList{QString()}) continue;
        if (row.size() != rows->first().size()) return fail(where + " / columns", "invalid_csv");
        auto weight = parsePercent(row[1]);
        if (!weight) return fail(where + " / probability", "invalid_probability (0–100, max 4 decimals)");
        bool ok;
        int limit = row[2].trimmed().toInt(&ok);
        if (!ok || limit < -1) return fail(where + " / limit", "invalid_quantity (-1 or integer >= 0)");
        int remaining = limit;
        if (row.size() == 5) {
            remaining = row[4].trimmed().toInt(&ok);
            if (!ok || (limit == -1 ? remaining != -1 : remaining < 0 || remaining > limit))
                return fail(where + " / remaining", "invalid_quantity");
        }
        static const QRegularExpression hex("^#[0-9a-fA-F]{6}$");
        if (!hex.match(row[3].trimmed()).hasMatch()) return fail(where + " / color", "invalid_color (#RRGGBB)");
        const QString name = row[0].trimmed();
        if (name.isEmpty() || name.size() > 120 || name.contains('\n') || name.contains('\r'))
            return fail(where + " / name", "invalid_name");
        c.prizes << Prize{QUuid::createUuid().toString(QUuid::WithoutBraces), name, *weight,
                          limit, remaining, QColor(row[3].trimmed())};
        e = validate(c);
        if (!e.isEmpty()) return fail(where + (e == "probability_over_100" ? " / probability total" : " / prizes"), e);
    }
    e = validate(c);
    if (!e.isEmpty()) return fail("CSV / prizes", e);
    if (error) error->clear();
    return c;
}
QString formatHelp(bool json) {
    const QString shared = QString::fromUtf8(
        "UTF-8；1–200 個獎品。名稱 1–120 字，不可換行。顏色 #RRGGBB。\n"
        "機率總和 ≤100%，未分配及售罄機率歸未中獎；獨立抽樣。\n"
        "limit=-1 為不限量（remaining 必須 -1）；否則 0≤remaining≤limit。\n"
        "匯入將取代獎品，確認前不寫入；先備份可保留舊設定。\n");
    return shared + (json ? QString::fromUtf8(
        "JSON schema=1，prizes 陣列必須含唯一 id、name、weight、limit、remaining、color。\n"
        "weight 為整數：100000=10%，1000000=100%。volume 0–100；theme light/dark；\n"
        "language zh_TW/zh_CN/en/ja/ko/es/fr/de/pt；sound/showSoldOut 為布林。\n"
        "載入保留目前輪次與鎖定；範例不含密碼。JSON 可保留獎品 ID、庫存及顏色。") : QString::fromUtf8(
        "CSV 標頭：name,probability,limit,color 或再加 remaining。欄位順序固定。\n"
        "probability 是百分比數字，最多 4 位小數，不含 %；10.0000=10%。\n"
        "省略 remaining 時庫存設為 limit；CSV 產生新的獎品 ID，不含全域設定。\n"
        "逗號或引號必須以雙引號包住；內部引號寫成兩個引號。獎品 CSV 與歷史 CSV 格式不同。\n"
        "UTF-8; fixed headers above. Percent 0–100, ≤4 decimals; color #RRGGBB.\n"
        "Optional remaining preserves stock; omission resets it to limit. IDs are regenerated."));
}
}
