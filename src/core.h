#pragma once
#include <QColor>
#include <QDateTime>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QStringList>
#include <QVector>
#include <optional>

namespace lw {
constexpr quint32 Scale = 1000000; // 100.0000%; one unit = 0.0001 percent
struct Prize {
    QString id, name;
    quint32 weight = 0;
    int limit = -1, remaining = -1; // -1 = unlimited
    QColor color = QColor("#62b5ff");
    bool active() const {
        return limit < 0 || remaining > 0;
    }
};
struct Config {
    QVector<Prize> prizes;
    bool showSoldOut = true;
    QString language = "zh_TW", theme = "light";
    int volume = 60;
    bool sound = true;
    QString lockSalt, lockHash;
    int round = 1;
};
struct Outcome {
    int index = -1;
    quint32 ticket = 0, offset = 0;
};
struct Slice {
    QString label;
    QColor color;
    quint32 weight = 0;
    int prize = -1;
    bool soldOut = false;
};
struct Record {
    qint64 id = 0;
    QString time, prizeId, name;
    int round = 1;
    quint32 weight = 0, ticket = 0;
    qint64 sequence = 0; // Display ordinal; database id remains immutable.
};
QString validate(const Config &c);
std::optional<quint32> parsePercent(const QString &text);
QString percent(quint32 weight);
quint32 missWeight(const Config &c);
Outcome sample(const Config &c, quint32 ticket);
QVector<Slice> slices(const Config &c, const QString &missLabel, const QString &soldLabel);
double landingAngle(const Config &c, const Outcome &o, const QVector<Slice> &view);
quint32 randomTicket();
QVector<qint64> simulateCounts(const Config &c, int count);
QJsonObject toJson(const Config &c);
std::optional<Config> fromJson(const QJsonObject &obj, QString *error);
QString encodeCsv(const QVector<QStringList> &rows, bool safeExport = false);
std::optional<QVector<QStringList>> decodeCsv(const QString &text, QString *error);
QString passwordHash(const QString &salt, const QString &password);
class Store {
  public:
    Store();
    ~Store();
    bool open(const QString &path, QString *error);
    Config load(QString *error) const;
    bool save(const Config &c, QString *error);
    bool draw(Config &c, const Outcome &o, const QString &missLabel, Record *record, QString *error);
    QVector<Record> history(QString *error) const;
    bool clearHistory(qint64 expectedCount, qint64 *deleted, QString *error);
    bool backup(const QString &path, QString *error);
    bool restore(const QString &path, QString *error);
    QString path() const {
        return m_path;
    }

  private:
    bool write(const Config &c, QString *error);
    QSqlDatabase db;
    QString connection, m_path;
};
} // namespace lw
