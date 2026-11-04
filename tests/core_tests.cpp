#include "core.h"
#include "formats.h"
#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QtTest>
#include <cmath>
using namespace lw;
static Config example() {
    Config c;
    c.prizes = {{"a", "A", 100000, 1, 1, QColor("#6699ff")}, {"b", "B", 200000, -1, -1, QColor("#99ff99")}};
    return c;
}
class CoreTests : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase() {
        const QString output = qEnvironmentVariable("LW_EXAMPLE_DIR");
        if (output.isEmpty()) return;
        for (const auto &pair : QVector<QPair<QString, QByteArray>>{
            {"prizes.csv", lw::prizeCsv(lw::exampleConfig()).toUtf8()},
            {"wheel.json", QJsonDocument(lw::toJson(lw::exampleConfig())).toJson()},
            {"FORMAT.md", (lw::formatHelp(false) + "\n\n" + lw::formatHelp(true)).toUtf8()}}) {
            QFile file(output + "/" + pair.first);
            QVERIFY(file.open(QIODevice::WriteOnly)); QCOMPARE(file.write(pair.second), pair.second.size());
        }
    }
    void firstRunLanguageMapping_data() {
        QTest::addColumn<QStringList>("languages");
        QTest::addColumn<QString>("expected");
        const QList<QPair<QString, QString>> cases{
            {"zh-TW", "zh_TW"}, {"zh-HK", "zh_TW"}, {"zh-MO", "zh_TW"},
            {"zh-Hant", "zh_TW"}, {"zh-Hant-CN", "zh_TW"}, {"zh_CN", "zh_CN"},
            {"zh-SG", "zh_CN"}, {"zh-Hans-TW", "zh_CN"}, {"zh", "zh_CN"},
            {"en-GB", "en"}, {"ja-JP", "ja"}, {"ko-KR", "ko"},
            {"es-MX", "es"}, {"fr-CA", "fr"}, {"de-AT", "de"},
            {"pt-BR", "pt"}, {"pt-PT", "pt"}, {"ru-RU", "en"}, {"C", "en"}};
        for (const auto &entry : cases)
            QTest::newRow(qPrintable(entry.first)) << QStringList{entry.first} << entry.second;
        QTest::newRow("preference-order") << QStringList{"fr-CA", "ja-JP"} << QString("fr");
        QTest::newRow("supported-fallback") << QStringList{"ru-RU", "de-DE"} << QString("de");
        QTest::newRow("empty") << QStringList{} << QString("en");
    }
    void firstRunLanguageMapping() {
        QFETCH(QStringList, languages);
        QFETCH(QString, expected);
        QCOMPARE(languageForUiLanguages(languages), expected);
    }
    void examplesAndColorStockRoundTrips() {
        const auto c = lw::exampleConfig();
        QString e;
        auto csv = lw::importPrizeCsv(lw::prizeCsv(c), c, &e);
        QVERIFY2(csv, qPrintable(e));
        auto json = lw::fromJson(lw::toJson(c), &e);
        QVERIFY2(json, qPrintable(e));
        QCOMPARE(lw::toJson(*json), lw::toJson(c));
        for (int i = 0; i < c.prizes.size(); ++i) {
            QCOMPARE(csv->prizes[i].name, c.prizes[i].name);
            QCOMPARE(csv->prizes[i].color, c.prizes[i].color);
            QCOMPARE(csv->prizes[i].remaining, c.prizes[i].remaining);
            QCOMPARE(csv->prizes[i].weight, c.prizes[i].weight);
        }
        auto consumed = c;
        consumed.prizes[0].remaining = 0;
        consumed.prizes[0].color = QColor("#123abc");
        csv = lw::importPrizeCsv(lw::prizeCsv(consumed), c, &e);
        QVERIFY(csv);
        QCOMPARE(csv->prizes[0].remaining, 0);
        QCOMPARE(csv->prizes[0].color.name(), QString("#123abc"));
        auto legacy = lw::importPrizeCsv("name,probability,limit,color\nLegacy,10,2,#abcdef\n", c, &e);
        QVERIFY(legacy);
        QCOMPARE(legacy->prizes[0].remaining, 2);
    }
    void importFieldErrorsAndNoMutation() {
        const auto c = lw::exampleConfig();
        const auto before = lw::toJson(c);
        QString e;
        for (const auto &pair : QVector<QPair<QString, QString>>{
            {"X,101,2,#abcdef", "probability"}, {"X,10,-2,#abcdef", "limit"},
            {"X,10,2,red", "color"}, {"X,10,2,#abcdef,3", "remaining"}}) {
            const bool extra = pair.second == "remaining";
            QVERIFY(!lw::importPrizeCsv(QString("name,probability,limit,color") + (extra ? ",remaining" : "") + "\n" + pair.first + "\n", c, &e));
            QVERIFY(e.contains("line 2"));
            QVERIFY(e.contains(pair.second));
        }
        QVERIFY(!lw::importPrizeCsv("name,probability,limit,color\n\"bad\"oops,1,2,#abcdef\n", c, &e));
        QVERIFY(e.contains("line 2"));
        auto obj = before; obj["sound"] = "false";
        QVERIFY(!lw::fromJson(obj, &e)); QVERIFY(e.contains("sound"));
        obj = before; obj["schema"] = 1.5;
        QVERIFY(!lw::fromJson(obj, &e));
        obj = before;
        auto a = obj["prizes"].toArray(); auto prize = a[0].toObject(); prize["color"] = "bad color";
        a[0] = prize; obj["prizes"] = a;
        QVERIFY(!lw::fromJson(obj, &e)); QVERIFY(e.contains("prizes[0]/color"));
        QCOMPARE(lw::toJson(c), before);
    }
    void clearHistoryPreservesStateAndSequence() {
        QTemporaryDir dir;
        QString e;
        Store s;
        QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
        auto c = example();
        c.round = 7;
        QVERIFY(s.save(c, &e));
        Record r;
        QVERIFY(s.draw(c, sample(c, 0), "Miss", &r, &e));
        QVERIFY(s.draw(c, sample(c, 900000), "Miss", &r, &e));
        const qint64 last = r.id;
        const auto before = toJson(c);
        qint64 deleted = -1;
        QVERIFY(!s.clearHistory(1, &deleted, &e));
        QCOMPARE(deleted, qint64(0));
        QCOMPARE(s.history(&e).size(), 2);
        QVERIFY(s.clearHistory(2, &deleted, &e));
        QCOMPARE(deleted, qint64(2));
        QCOMPARE(s.history(&e).size(), 0);
        QCOMPARE(toJson(s.load(&e)), before);
        QVERIFY(s.clearHistory(0, &deleted, &e));
        QCOMPARE(deleted, qint64(0));
        QVERIFY(s.backup(dir.filePath("empty.sqlite"), &e));
        // Restoring a cleared backup into a new DB keeps the last committed ID.
        Store restored;
        QVERIFY(restored.open(dir.filePath("restored.sqlite"), &e));
        QVERIFY(restored.restore(dir.filePath("empty.sqlite"), &e));
        auto copy = restored.load(&e);
        QVERIFY(restored.draw(copy, sample(copy, 900000), "Miss", &r, &e));
        QVERIFY(r.id > last);
        QVERIFY(s.draw(c, sample(c, 900000), "Miss", &r, &e));
        QVERIFY(r.id > last);
        QCOMPARE(s.history(&e).first().sequence, qint64(1));
        QVERIFY(s.draw(c, sample(c, 900000), "Miss", &r, &e));
        auto numbered = s.history(&e);
        QCOMPARE(numbered[0].sequence, qint64(2));
        QCOMPARE(numbered[1].sequence, qint64(1));
        QVERIFY(numbered[1].id > last);
    }
    void failedClearRollsBackEveryRow() {
        QTemporaryDir dir;
        QString e;
        const QString path = dir.filePath("data.sqlite");
        Store s;
        QVERIFY(s.open(path, &e));
        auto c = example();
        QVERIFY(s.save(c, &e));
        QVERIFY(s.draw(c, sample(c, 900000), "Miss", nullptr, &e));
        QVERIFY(s.draw(c, sample(c, 900000), "Miss", nullptr, &e));
        const auto before = toJson(c);
        {
            auto db = QSqlDatabase::addDatabase("QSQLITE", "clear-failure");
            db.setDatabaseName(path);
            QVERIFY(db.open());
            QSqlQuery q(db);
            QVERIFY(q.exec("CREATE TRIGGER reject_clear BEFORE DELETE ON history WHEN OLD.id=2 "
                           "BEGIN SELECT RAISE(ABORT,'test failure'); END"));
        }
        QSqlDatabase::removeDatabase("clear-failure");
        qint64 deleted = 42;
        QVERIFY(!s.clearHistory(2, &deleted, &e));
        QCOMPARE(deleted, qint64(0));
        QCOMPARE(s.history(&e).size(), 2);
        QCOMPARE(toJson(s.load(&e)), before);
    }
    void precision() {
        QCOMPARE(*parsePercent("0.0001"), quint32(1));
        QCOMPARE(*parsePercent("100.0000"), Scale);
        QCOMPARE(*parsePercent("12.3456"), quint32(123456));
        for (auto s : {"100.0001", "0.00001", "-1", "NaN", "1e2", ".5", "1,5", ""})
            QVERIFY(!parsePercent(s));
        QCOMPARE(percent(123456), QString("12.3456"));
    }
    void validation() {
        auto c = example();
        QVERIFY(validate(c).isEmpty());
        c.prizes[0].weight = 900000;
        QCOMPARE(validate(c), QString("probability_over_100"));
        c = example();
        c.prizes[0].remaining = 2;
        QCOMPARE(validate(c), QString("invalid_quantity"));
        c = example();
        c.prizes[0].name = "";
        QCOMPARE(validate(c), QString("invalid_name"));
        c = example();
        c.prizes[1].id = "a";
        QCOMPARE(validate(c), QString("duplicate_id"));
    }
    void boundaries() {
        auto c = example();
        QCOMPARE(sample(c, 0).index, 0);
        QCOMPARE(sample(c, 99999).index, 0);
        QCOMPARE(sample(c, 100000).index, 1);
        QCOMPARE(sample(c, 299999).index, 1);
        QCOMPARE(sample(c, 300000).index, -1);
        QCOMPARE(sample(c, 999999).index, -1);
        QCOMPARE(sample(c, Scale).index, -2);
        c.prizes[0].remaining = 0;
        QCOMPARE(missWeight(c), quint32(800000));
        QCOMPARE(sample(c, 199999).index, 1);
        QCOMPARE(sample(c, 200000).index, -1);
    }
    void geometryAndHiddenInvariance() {
        auto c = example();
        c.prizes[0].remaining = 0;
        auto visible = slices(c, "Miss", "Sold");
        c.showSoldOut = false;
        auto hidden = slices(c, "Miss", "Sold");
        QCOMPARE(visible.size(), 3);
        QCOMPARE(hidden.size(), 2);
        QCOMPARE(hidden[0].weight, quint32(200000));
        QCOMPARE(hidden[1].weight, quint32(800000));
        auto check = [&](const QVector<Slice> &view, const Outcome &o) {
            double angle = landingAngle(c, o, view);
            double pos = angle / 360. * Scale;
            quint32 start = 0;
            for (auto s : view) {
                if (pos >= start && pos < start + s.weight) {
                    QCOMPARE(s.prize, o.index);
                    return;
                }
                start += s.weight;
            }
            QFAIL("No rendered slice contains landing point");
        };
        // Exhaust every integer ticket: exact counts and invariance, with no statistical approximation.
        QVector<quint32> counts(3);
        for (quint32 n = 0; n < Scale; ++n) {
            auto a = sample(c, n);
            c.showSoldOut = true;
            auto b = sample(c, n);
            QCOMPARE(a.index, b.index);
            QCOMPARE(a.offset, b.offset);
            c.showSoldOut = false;
            ++counts[a.index + 1];
            if (n % 997 == 0 || n == 199999 || n == 200000 || n == 999999) {
                check(visible, a);
                check(hidden, a);
            }
        }
        QCOMPARE(counts[0], quint32(800000));
        QCOMPARE(counts[1], quint32(0));
        QCOMPARE(counts[2], quint32(200000));
    }
    void allSoldZeroAndHundred() {
        auto c = example();
        c.prizes[1].limit = 1;
        c.prizes[1].remaining = 0;
        c.prizes[0].remaining = 0;
        c.showSoldOut = false;
        QCOMPARE(missWeight(c), Scale);
        QCOMPARE(slices(c, "Miss", "Sold").size(), 1);
        QCOMPARE(sample(c, 0).index, -1);
        c = example();
        c.prizes[0].weight = Scale;
        c.prizes[1].weight = 0;
        QCOMPARE(sample(c, 999999).index, 0);
        c.prizes[0].weight = 1;
        QCOMPARE(sample(c, 0).index, 0);
        QCOMPARE(sample(c, 1).index, -1);
    }
    void jsonRoundTrip() {
        auto c = example();
        c.language = "ja";
        c.showSoldOut = false;
        c.round = 7;
        QString e;
        auto copy = fromJson(toJson(c), &e);
        QVERIFY(copy);
        QCOMPARE(toJson(*copy), toJson(c));
        auto obj = toJson(c);
        obj["schema"] = 2;
        QVERIFY(!fromJson(obj, &e));
        obj = toJson(c);
        auto a = obj["prizes"].toArray();
        auto p = a[0].toObject();
        p["weight"] = 1.2;
        a[0] = p;
        obj["prizes"] = a;
        QVERIFY(!fromJson(obj, &e));
    }
    void csv() {
        QString e;
        QVector<QStringList> rows{{"name", "probability", "limit", "color"},
                                  {"A,\"B\"", "10.0001", "3", "#abcdef"}};
        auto parsed = decodeCsv(encodeCsv(rows), &e);
        QVERIFY(parsed);
        QCOMPARE(*parsed, rows);
        QVERIFY(!decodeCsv("\"broken", &e));
        QVERIFY(!decodeCsv("\"a\"x", &e));
        auto safe = encodeCsv({{"=CMD()", "+test", "normal"}}, true);
        QVERIFY(safe.contains("'=CMD()"));
        QVERIFY(safe.contains("'+test"));
    }
    void persistenceAndLastStock() {
        QTemporaryDir dir;
        QString e;
        Store s;
        QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
        auto c = example();
        QVERIFY(s.save(c, &e));
        Record r;
        QVERIFY(s.draw(c, sample(c, 0), "Miss", &r, &e));
        QCOMPARE(c.prizes[0].remaining, 0);
        QCOMPARE(r.name, QString("A"));
        QCOMPARE(s.history(&e).size(), 1);
        auto reloaded = s.load(&e);
        QCOMPARE(toJson(reloaded), toJson(c));
        QVERIFY(s.draw(c, sample(c, 0), "Miss", &r, &e));
        QCOMPARE(r.name, QString("B"));
        QCOMPARE(c.prizes[0].remaining, 0);
        auto records = s.history(&e);
        QCOMPARE(records.size(), 2);
        QVERIFY(records[0].id > records[1].id);
    }
    void rejectsStaleAndInvalidWithoutSideEffects() {
        QTemporaryDir dir;
        QString e;
        Store s;
        QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
        auto c = example();
        QVERIFY(s.save(c, &e));
        auto stale = c;
        stale.prizes[0].remaining = 0;
        QVERIFY(!s.draw(stale, sample(stale, 0), "Miss", nullptr, &e));
        QCOMPARE(s.history(&e).size(), 0);
        QVERIFY(!s.draw(c, {-1, 0, 0}, "Miss", nullptr, &e));
        QCOMPARE(s.load(&e).prizes[0].remaining, 1);
    }
    void backupRestoreAndInvalidBackup() {
        QTemporaryDir dir;
        QString e;
        Store s;
        QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
        auto c = example();
        QVERIFY(s.save(c, &e));
        QVERIFY(s.draw(c, sample(c, 0), "Miss", nullptr, &e));
        QString backup = dir.filePath("backup.sqlite");
        QVERIFY2(s.backup(backup, &e), qPrintable(e));
        QVERIFY(!s.backup(backup, &e));
        QVERIFY(s.draw(c, sample(c, 0), "Miss", nullptr, &e));
        QCOMPARE(s.history(&e).size(), 2);
        e.clear();
        QVERIFY2(s.restore(backup, &e), qPrintable(e));
        QCOMPARE(s.history(&e).size(), 1);
        QCOMPARE(s.load(&e).prizes[0].remaining, 0);
        QFile bad(dir.filePath("bad.sqlite"));
        QVERIFY(bad.open(QIODevice::WriteOnly));
        bad.write("bad");
        bad.close();
        e.clear();
        QVERIFY(!s.restore(bad.fileName(), &e));
        QCOMPARE(s.history(&e).size(), 1);
    }
    void randomDistribution() {
        QVector<int> bins(10);
        constexpr int n = 100000;
        for (int i = 0; i < n; ++i) {
            auto ticket = randomTicket();
            QVERIFY(ticket < Scale);
            ++bins[ticket / 100000];
        }
        for (int count : bins)
            QVERIFY2(std::abs(count - n / 10) < 600,
                     "Random source exceeds conservative distribution tolerance");
    }
    void password() {
        QString a = passwordHash("01234567890123456789012345678901", "1234");
        QCOMPARE(a.size(), 64);
        QVERIFY(a != passwordHash("11234567890123456789012345678901", "1234"));
        QVERIFY(a != passwordHash("01234567890123456789012345678901", "5678"));
    }
    void failedHistoryWriteRollsBackStock() {
        QTemporaryDir dir;
        QString e;
        QString path = dir.filePath("data.sqlite");
        Store s;
        QVERIFY(s.open(path, &e));
        auto c = example();
        QVERIFY(s.save(c, &e));
        QString connection = "failure-injection";
        {
            auto db = QSqlDatabase::addDatabase("QSQLITE", connection);
            db.setDatabaseName(path);
            QVERIFY(db.open());
            QSqlQuery q(db);
            QVERIFY(q.exec("CREATE TRIGGER reject_record BEFORE INSERT ON history BEGIN SELECT "
                           "RAISE(ABORT,'test failure'); END"));
            q.finish();
            db.close();
        }
        QSqlDatabase::removeDatabase(connection);
        QVERIFY(!s.draw(c, sample(c, 0), "Miss", nullptr, &e));
        QCOMPARE(c.prizes[0].remaining, 1);
        QCOMPARE(s.load(&e).prizes[0].remaining, 1);
        QCOMPARE(s.history(&e).size(), 0);
    }
    void independentSimulation() {
        QTemporaryDir dir;
        QString e;
        Store s;
        QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
        auto c = example();
        c.prizes[0].remaining = 0;
        QVERIFY(s.save(c, &e));
        auto before = toJson(c);
        auto counts = simulateCounts(c, 100000);
        QCOMPARE(counts.size(), 3);
        QCOMPARE(counts[1], qint64(0));
        QCOMPARE(counts[0] + counts[2], qint64(100000));
        QVERIFY(std::abs(counts[2] - 20000) < 1000);
        QCOMPARE(toJson(c), before);
        QCOMPARE(toJson(s.load(&e)), before);
        QCOMPARE(s.history(&e).size(), 0);
    }
};
QTEST_GUILESS_MAIN(CoreTests)
#include "core_tests.moc"
