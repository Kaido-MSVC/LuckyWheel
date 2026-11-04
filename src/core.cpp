#include "core.h"
#include <QCryptographicHash>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLocale>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <algorithm>
#include <cmath>
#include <climits>

namespace lw {
QString languageForUiLanguages(const QStringList &languages) {
    const QStringList supported{"en", "ja", "ko", "es", "fr", "de", "pt"};
    for (QString tag : languages) {
        tag.replace('_', '-');
        const QString language = tag.section('-', 0, 0).toLower();
        if (language == "zh") {
            const QLocale locale(tag);
            return locale.script() == QLocale::TraditionalHanScript ? "zh_TW" : "zh_CN";
        }
        if (supported.contains(language)) return language;
    }
    return "en";
}
QString validate(const Config &c) {
    if (c.prizes.isEmpty() || c.prizes.size() > 200)
        return "prize_count";
    quint64 total = 0;
    QSet<QString> ids;
    for (const auto &p : c.prizes) {
        if (p.id.isEmpty() || ids.contains(p.id))
            return "duplicate_id";
        ids.insert(p.id);
        if (p.name.trimmed().isEmpty() || p.name.size() > 120 || p.name.contains('\n') ||
            p.name.contains('\r'))
            return "invalid_name";
        if (!p.color.isValid())
            return "invalid_color";
        if (p.weight > Scale)
            return "invalid_probability";
        if (p.limit < -1 || (p.limit == -1 && p.remaining != -1) ||
            (p.limit >= 0 && (p.remaining < 0 || p.remaining > p.limit)))
            return "invalid_quantity";
        total += p.weight;
    }
    if (total > Scale)
        return "probability_over_100";
    if (c.round < 1 || c.volume < 0 || c.volume > 100)
        return "invalid_settings";
    if (!QStringList{"zh_TW", "zh_CN", "en", "ja", "ko", "es", "fr", "de", "pt"}.contains(c.language))
        return "invalid_language";
    if (c.theme != "light" && c.theme != "dark")
        return "invalid_theme";
    if (c.lockSalt.isEmpty() != c.lockHash.isEmpty())
        return "invalid_lock";
    if (!c.lockHash.isEmpty() && (c.lockSalt.size() != 32 || c.lockHash.size() != 64))
        return "invalid_lock";
    return {};
}
std::optional<quint32> parsePercent(const QString &text) {
    QString s = text.trimmed();
    static const QRegularExpression re("^(\\d{1,3})(?:\\.(\\d{1,4}))?$");
    auto m = re.match(s);
    if (!m.hasMatch())
        return std::nullopt;
    QString fractional = m.captured(2);
    while (fractional.size() < 4)
        fractional += '0';
    quint32 n = m.captured(1).toUInt() * 10000 + fractional.toUInt();
    if (n > Scale)
        return std::nullopt;
    return n;
}
QString percent(quint32 w) {
    return QString::number(w / 10000) + '.' + QString::number(w % 10000).rightJustified(4, '0');
}
quint32 missWeight(const Config &c) {
    quint32 active = 0;
    for (const auto &p : c.prizes)
        if (p.active())
            active += p.weight;
    return Scale - active;
}
Outcome sample(const Config &c, quint32 ticket) {
    if (!validate(c).isEmpty() || ticket >= Scale)
        return {-2, ticket, 0};
    quint32 start = 0;
    for (int i = 0; i < c.prizes.size(); ++i)
        if (c.prizes[i].active()) {
            quint32 w = c.prizes[i].weight;
            if (ticket < start + w)
                return {i, ticket, ticket - start};
            start += w;
        }
    return {-1, ticket, ticket - start};
}
QVector<Slice> slices(const Config &c, const QString &missLabel, const QString &soldLabel) {
    QVector<Slice> out;
    quint32 represented = 0;
    for (int i = 0; i < c.prizes.size(); ++i) {
        const auto &p = c.prizes[i];
        if (!p.weight || (!p.active() && !c.showSoldOut))
            continue;
        bool sold = !p.active();
        out.push_back({sold ? p.name + " · " + soldLabel + " → " + missLabel : p.name,
                       sold ? QColor("#a0a6b3") : p.color, p.weight, sold ? -1 : i, sold});
        represented += p.weight;
    }
    if (represented < Scale)
        out.push_back({missLabel, QColor("#cbd2da"), Scale - represented, -1, false});
    return out;
}
double landingAngle(const Config &, const Outcome &o, const QVector<Slice> &view) {
    quint32 start = 0, missOffset = o.offset;
    for (const auto &s : view) {
        if (o.index >= 0 && s.prize == o.index)
            return 360.0 * (start + s.weight / 2.0) / Scale;
        if (o.index == -1 && s.prize == -1) {
            if (missOffset < s.weight)
                return 360.0 * (start + s.weight / 2.0) / Scale;
            missOffset -= s.weight;
        }
        start += s.weight;
    }
    return 0;
}
quint32 randomTicket() {
    // Rejection sampling: avoid modulo bias over the 2^32 source domain.
    constexpr quint64 domain = (quint64(1) << 32);
    constexpr quint64 cutoff = domain - domain % Scale;
    quint32 n;
    do {
        n = QRandomGenerator::system()->generate();
    } while (quint64(n) >= cutoff);
    return n % Scale;
}
QVector<qint64> simulateCounts(const Config &c, int count) {
    if (!validate(c).isEmpty() || count < 1 || count > 1000000)
        return {};
    QVector<quint32> endpoints;
    QVector<int> indices;
    quint32 total = 0;
    for (int i = 0; i < c.prizes.size(); ++i)
        if (c.prizes[i].active() && c.prizes[i].weight) {
            total += c.prizes[i].weight;
            endpoints << total;
            indices << i + 1;
        }
    if (total < Scale) {
        endpoints << Scale;
        indices << 0;
    }
    QVector<qint64> counts(c.prizes.size() + 1);
    for (int i = 0; i < count; ++i) {
        auto it = std::upper_bound(endpoints.begin(), endpoints.end(), randomTicket());
        ++counts[indices[int(it - endpoints.begin())]];
    }
    return counts;
}
QJsonObject toJson(const Config &c) {
    QJsonArray a;
    for (const auto &p : c.prizes)
        a.append(QJsonObject{{"id", p.id},
                             {"name", p.name},
                             {"weight", int(p.weight)},
                             {"limit", p.limit},
                             {"remaining", p.remaining},
                             {"color", p.color.name()}});
    return {{"schema", 1},
            {"prizes", a},
            {"showSoldOut", c.showSoldOut},
            {"language", c.language},
            {"theme", c.theme},
            {"sound", c.sound},
            {"volume", c.volume},
            {"round", c.round},
            {"lockSalt", c.lockSalt},
            {"lockHash", c.lockHash}};
}
std::optional<Config> fromJson(const QJsonObject &obj, QString *error) {
    auto fail = [&](QString e) -> std::optional<Config> {
        if (error)
            *error = e;
        return std::nullopt;
    };
    if (!obj.value("schema").isDouble() || obj.value("schema").toDouble() != 1 || !obj.value("prizes").isArray())
        return fail("JSON / schema,prizes: invalid_schema (schema=1; prizes=array)");
    for (auto key : {"showSoldOut", "sound"})
        if (obj.contains(key) && !obj.value(key).isBool()) return fail(QString("JSON / %1: expected boolean").arg(key));
    for (auto key : {"language", "theme", "lockSalt", "lockHash"})
        if (obj.contains(key) && !obj.value(key).isString()) return fail(QString("JSON / %1: expected string").arg(key));
    for (auto key : {"volume", "round"})
        if (obj.contains(key) && (!obj.value(key).isDouble() || std::floor(obj.value(key).toDouble()) != obj.value(key).toDouble()))
            return fail(QString("JSON / %1: expected integer").arg(key));
    Config c;
    c.showSoldOut = obj.value("showSoldOut").toBool(true);
    c.language = obj.value("language").toString("zh_TW");
    c.theme = obj.value("theme").toString("light");
    c.sound = obj.value("sound").toBool(true);
    c.volume = obj.value("volume").toInt(60);
    c.round = obj.value("round").toInt(1);
    if ((obj.contains("volume") && (obj.value("volume").toDouble() < 0 || obj.value("volume").toDouble() > 100)) ||
        (obj.contains("round") && (obj.value("round").toDouble() < 1 || obj.value("round").toDouble() > INT_MAX)))
        return fail("JSON / volume,round: invalid_settings");
    c.lockSalt = obj.value("lockSalt").toString();
    c.lockHash = obj.value("lockHash").toString();
    int index = 0;
    for (auto v : obj.value("prizes").toArray()) {
        const QString path = QString("JSON / prizes[%1]").arg(index++);
        if (!v.isObject())
            return fail(path + ": invalid_prize");
        auto o = v.toObject();
        for (auto key : {"id", "name", "color"})
            if (!o.value(key).isString()) return fail(path + "/" + key + ": expected string");
        for (auto key : {"weight", "limit", "remaining"})
            if (!o.value(key).isDouble() || std::floor(o.value(key).toDouble()) != o.value(key).toDouble())
                return fail(path + "/" + key + ": invalid_number");
        double w = o.value("weight").toDouble();
        if (w < 0 || w > Scale)
            return fail(path + "/weight: invalid_probability");
        c.prizes.push_back({o.value("id").toString(), o.value("name").toString(), quint32(w),
                            o.value("limit").toInt(-2), o.value("remaining").toInt(-2),
                            QColor(o.value("color").toString())});
        Config prizeCheck; prizeCheck.prizes = c.prizes;
        const QString issue = validate(prizeCheck);
        if (!issue.isEmpty()) {
            QString field = issue == "invalid_color" ? "color" : issue == "invalid_name" ? "name" :
                            issue == "duplicate_id" ? "id" : issue == "invalid_quantity" ? "limit/remaining" : "weight/prizes";
            return fail(path + "/" + field + ": " + issue);
        }
    }
    QString e = validate(c);
    if (!e.isEmpty())
        return fail("JSON / settings (volume, round, language, theme, lockSalt/lockHash): " + e);
    return c;
}
QString encodeCsv(const QVector<QStringList> &rows, bool safe) {
    QString out;
    for (auto row : rows) {
        QStringList fields;
        for (auto s : row) {
            if (safe && !s.isEmpty() && QString("=+-@\t\r").contains(s.front()))
                s.prepend('\'');
            s.replace("\"", "\"\"");
            fields << '"' + s + '"';
        }
        out += fields.join(',') + "\r\n";
    }
    return out;
}
std::optional<QVector<QStringList>> decodeCsv(const QString &text, QString *error) {
    QVector<QStringList> rows;
    QStringList row;
    QString field;
    bool quoted = false, ended = false, start = true;
    int cursor = 0;
    QString input = text;
    if (input.startsWith(QChar(0xfeff)))
        input.remove(0, 1);
    auto fail = [&]() -> std::optional<QVector<QStringList>> {
        if (error)
            *error = QString("CSV line %1 / column %2: invalid_csv").arg(input.left(cursor).count('\n') + 1).arg(row.size() + 1);
        return std::nullopt;
    };
    for (int i = 0; i < input.size(); ++i) {
        cursor = i;
        QChar ch = input[i];
        if (quoted) {
            if (ch == '"') {
                if (i + 1 < input.size() && input[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    quoted = false;
                    ended = true;
                }
            } else
                field += ch;
            continue;
        }
        if (ch == '"') {
            if (!start || ended)
                return fail();
            quoted = true;
            start = false;
            continue;
        }
        if (ch == ',' || ch == '\r' || ch == '\n') {
            row << field;
            field.clear();
            start = true;
            ended = false;
            if (ch != ',') {
                if (ch == '\r' && i + 1 < input.size() && input[i + 1] == '\n')
                    ++i;
                rows << row;
                row.clear();
            }
        } else {
            if (ended)
                return fail();
            field += ch;
            start = false;
        }
    }
    cursor = input.size();
    if (quoted)
        return fail();
    if (!field.isEmpty() || !row.isEmpty() || ended) {
        row << field;
        rows << row;
    }
    return rows;
}
QString passwordHash(const QString &salt, const QString &password) {
    return QString::fromLatin1(
        QCryptographicHash::hash((salt + password).toUtf8(), QCryptographicHash::Sha256).toHex());
}
Store::Store() : connection(QUuid::createUuid().toString()) {}
Store::~Store() {
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connection);
}
bool Store::open(const QString &path, QString *error) {
    m_path = path;
    db = QSqlDatabase::addDatabase("QSQLITE", connection);
    db.setDatabaseName(path);
    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=3000");
    if (!db.open()) {
        if (error)
            *error = db.lastError().text();
        return false;
    }
    QSqlQuery q(db);
    for (const auto &sql : QStringList{
             "PRAGMA journal_mode=WAL", "PRAGMA synchronous=FULL",
             "CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY CHECK(id=1), json TEXT NOT NULL)",
             "CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY AUTOINCREMENT, time TEXT NOT NULL, "
             "prize_id TEXT NOT NULL, name TEXT NOT NULL, round INTEGER NOT NULL, weight INTEGER NOT NULL, "
             "ticket INTEGER NOT NULL)"})
        if (!q.exec(sql)) {
            if (error)
                *error = q.lastError().text();
            return false;
        }
    return true;
}
Config Store::load(QString *error) const {
    if (error)
        error->clear();
    QSqlQuery q(db);
    if (!q.exec("SELECT json FROM settings WHERE id=1")) {
        if (error)
            *error = q.lastError().text();
        return {};
    }
    if (!q.next()) {
        Config initial;
        // system().uiLanguages() follows Windows UI preferences, not regional formats
        // or an application-wide QLocale::setDefault() override.
        initial.language = languageForUiLanguages(QLocale::system().uiLanguages());
        return initial;
    }
    QJsonParseError parse;
    auto doc = QJsonDocument::fromJson(q.value(0).toString().toUtf8(), &parse);
    if (parse.error != QJsonParseError::NoError || !doc.isObject()) {
        if (error)
            *error = "invalid_json";
        return {};
    }
    auto c = fromJson(doc.object(), error);
    return c.value_or(Config{});
}
bool Store::write(const Config &c, QString *error) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO settings(id,json) VALUES(1,?) ON CONFLICT(id) DO UPDATE SET json=excluded.json");
    q.addBindValue(QString::fromUtf8(QJsonDocument(toJson(c)).toJson(QJsonDocument::Compact)));
    if (!q.exec()) {
        if (error)
            *error = q.lastError().text();
        return false;
    }
    return true;
}
bool Store::save(const Config &c, QString *error) {
    QString e = validate(c);
    if (!e.isEmpty()) {
        if (error)
            *error = e;
        return false;
    }
    return write(c, error);
}
bool Store::draw(Config &c, const Outcome &o, const QString &missLabel, Record *record, QString *error) {
    Outcome check = sample(c, o.ticket);
    if (check.index != o.index || check.offset != o.offset || o.index < -1) {
        if (error)
            *error = "invalid_outcome";
        return false;
    }
    if (!db.transaction()) {
        if (error)
            *error = db.lastError().text();
        return false;
    }
    QString e;
    Config latest = load(&e);
    if (!e.isEmpty() || toJson(latest) != toJson(c)) {
        db.rollback();
        if (error)
            *error = e.isEmpty() ? "stale_configuration" : e;
        return false;
    }
    Config next = c;
    Record r;
    r.time = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
    r.round = c.round;
    r.ticket = o.ticket;
    if (o.index >= 0) {
        auto &p = next.prizes[o.index];
        r.prizeId = p.id;
        r.name = p.name;
        r.weight = p.weight;
        if (p.limit >= 0)
            --p.remaining;
    } else {
        r.name = missLabel;
        r.weight = missWeight(c);
    }
    QSqlQuery q(db);
    q.prepare("INSERT INTO history(time,prize_id,name,round,weight,ticket) VALUES(?,?,?,?,?,?)");
    q.addBindValue(r.time);
    q.addBindValue(r.prizeId.isNull() ? QString("") : r.prizeId);
    q.addBindValue(r.name);
    q.addBindValue(r.round);
    q.addBindValue(r.weight);
    q.addBindValue(r.ticket);
    if (!q.exec()) {
        if (error)
            *error = q.lastError().text();
        db.rollback();
        return false;
    }
    r.id = q.lastInsertId().toLongLong();
    q.finish();
    if (!write(next, error) || !db.commit()) {
        if (error && error->isEmpty())
            *error = db.lastError().text();
        db.rollback();
        return false;
    }
    c = next;
    if (record)
        *record = r;
    return true;
}
QVector<Record> Store::history(QString *error) const {
    QVector<Record> out;
    QSqlQuery q(db);
    if (!q.exec("SELECT id,time,prize_id,name,round,weight,ticket FROM history ORDER BY time DESC,id DESC")) {
        if (error)
            *error = q.lastError().text();
        return out;
    }
    while (q.next())
        out.push_back({q.value(0).toLongLong(), q.value(1).toString(), q.value(2).toString(),
                       q.value(3).toString(), q.value(4).toInt(), q.value(5).toUInt(), q.value(6).toUInt()});
    QVector<qint64> ids;
    for (const auto &r : out) ids << r.id;
    std::sort(ids.begin(), ids.end());
    for (auto &r : out)
        r.sequence = std::lower_bound(ids.begin(), ids.end(), r.id) - ids.begin() + 1;
    return out;
}
bool Store::clearHistory(qint64 expectedCount, qint64 *deleted, QString *error) {
    if (error) error->clear();
    if (deleted) *deleted = 0;
    QSqlQuery q(db);
    auto fail = [&](const QString &message) {
        if (error) *error = message;
        db.rollback();
        return false;
    };
    // Acquire the write lock before counting; a changed count needs fresh confirmation.
    if (!q.exec("BEGIN IMMEDIATE")) {
        if (error) *error = q.lastError().text();
        return false;
    }
    if (!q.exec("SELECT COUNT(*) FROM history") || !q.next())
        return fail(q.lastError().text());
    const qint64 count = q.value(0).toLongLong();
    q.finish();
    if (count != expectedCount) return fail("history_changed");
    // Never touch settings or sqlite_sequence: committed IDs remain monotonic.
    if (!q.exec("DELETE FROM history")) return fail(q.lastError().text());
    if (q.numRowsAffected() != count) return fail("history_changed");
    q.finish();
    if (!db.commit()) return fail(db.lastError().text());
    if (deleted) *deleted = count;
    return true;
}
bool Store::backup(const QString &path, QString *error) {
    if (QFile::exists(path)) {
        if (error)
            *error = "backup_exists";
        return false;
    }
    QSqlQuery q(db);
    q.prepare("VACUUM INTO ?");
    q.addBindValue(path);
    if (!q.exec()) {
        if (error)
            *error = q.lastError().text();
        return false;
    }
    return true;
}
bool Store::restore(const QString &path, QString *error) {
    // Validate without changing the source backup, then restore both tables in one transaction.
    QString sourceName = QUuid::createUuid().toString();
    Config imported;
    QVector<Record> records;
    bool ok = false;
    qint64 sourceSequence = 0;
    {
        QSqlDatabase src = QSqlDatabase::addDatabase("QSQLITE", sourceName);
        src.setDatabaseName(path);
        src.setConnectOptions("QSQLITE_OPEN_READONLY");
        if (src.open()) {
            QSqlQuery q(src);
            if (q.exec("PRAGMA quick_check") && q.next() && q.value(0).toString() == "ok") {
                q.finish();
                if (q.exec("SELECT json FROM settings WHERE id=1") && q.next()) {
                    QJsonParseError pe;
                    auto doc = QJsonDocument::fromJson(q.value(0).toString().toUtf8(), &pe);
                    auto c = (pe.error == QJsonParseError::NoError && doc.isObject())
                                 ? fromJson(doc.object(), error)
                                 : std::nullopt;
                    if (c) {
                        imported = *c;
                        q.finish();
                        if (q.exec("SELECT id,time,prize_id,name,round,weight,ticket FROM history ORDER BY "
                                   "id")) {
                            ok = true;
                            while (q.next()) {
                                Record r{q.value(0).toLongLong(), q.value(1).toString(),
                                         q.value(2).toString(),   q.value(3).toString(),
                                         q.value(4).toInt(),      q.value(5).toUInt(),
                                         q.value(6).toUInt()};
                                if (r.id < 1 || !QDateTime::fromString(r.time, Qt::ISODateWithMs).isValid() ||
                                    r.round < 1 || r.weight > Scale || r.ticket >= Scale ||
                                    r.name.isEmpty()) {
                                    ok = false;
                                    break;
                                }
                                records << r;
                            }
                        }
                    }
                }
            }
        }
        if (ok) {
            QSqlQuery seq(src);
            ok = seq.exec("SELECT seq FROM sqlite_sequence WHERE name='history'");
            if (ok && seq.next()) sourceSequence = seq.value(0).toLongLong();
        }
        src.close();
    }
    QSqlDatabase::removeDatabase(sourceName);
    if (!ok) {
        if (error && error->isEmpty())
            *error = "invalid_backup";
        return false;
    }
    if (!db.transaction()) {
        if (error)
            *error = db.lastError().text();
        return false;
    }
    QSqlQuery q(db);
    auto fail = [&]() {
        if (error)
            *error = q.lastError().text();
        db.rollback();
        return false;
    };
    if (!q.exec("DELETE FROM history"))
        return fail();

    for (const auto &r : records) {
        q.prepare("INSERT INTO history(id,time,prize_id,name,round,weight,ticket) VALUES(?,?,?,?,?,?,?)");
        q.addBindValue(r.id);
        q.addBindValue(r.time);
        q.addBindValue(r.prizeId);
        q.addBindValue(r.name);
        q.addBindValue(r.round);
        q.addBindValue(r.weight);
        q.addBindValue(r.ticket);
        if (!q.exec())
            return fail();
    }
    q.prepare("UPDATE sqlite_sequence SET seq=MAX(seq,?) WHERE name='history'");
    q.addBindValue(sourceSequence);
    if (!q.exec()) return fail();
    q.prepare("INSERT INTO sqlite_sequence(name,seq) SELECT 'history',? WHERE NOT EXISTS "
              "(SELECT 1 FROM sqlite_sequence WHERE name='history')");
    q.addBindValue(sourceSequence);
    if (!q.exec()) return fail();
    q.finish();
    if (!write(imported, error)) {
        db.rollback();
        return false;
    }
    if (!db.commit()) {
        if (error)
            *error = db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}
} // namespace lw
