#include "window.h"
#include "ms_terms.h"
#include "version.h"
#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLockFile>
#include <QMessageBox>
#include <QSaveFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <iterator>
#endif

static bool writeSqliteRuntimeReport(const QString &directory) {
    QJsonObject report;
    const QString connection = "sqlite-runtime-report";
    {
        auto db = QSqlDatabase::addDatabase("QSQLITE", connection);
        db.setDatabaseName(":memory:");
        QSqlQuery query(db);
        if (db.open() && query.exec("SELECT sqlite_version(), sqlite_source_id()") && query.next()) {
            report = {{"qtVersion", qVersion()}, {"driver", db.driverName()},
                      {"sqliteVersion", query.value(0).toString()}, {"sqliteSourceId", query.value(1).toString()}};
            query.finish();
            if (query.exec("PRAGMA compile_options")) {
                QJsonArray options;
                while (query.next()) options.append(query.value(0).toString());
                if (!query.lastError().isValid()) report.insert("compileOptions", options);
            }
#ifdef Q_OS_WIN
            const auto module = GetModuleHandleW(L"qsqlite.dll");
            wchar_t path[32768];
            if (module) {
                const DWORD length = GetModuleFileNameW(module, path, DWORD(std::size(path)));
                if (length > 0 && length < std::size(path))
                    report.insert("pluginPath", QString::fromWCharArray(path, length));
            }
#endif
        }
    }
    QSqlDatabase::removeDatabase(connection);
    if (!report.contains("compileOptions")) return false;
#ifdef Q_OS_WIN
    if (!report.contains("pluginPath")) return false;
#endif
    QSaveFile output(QDir(directory).filePath("sqlite-runtime.json"));
    const auto bytes = QJsonDocument(report).toJson();
    return output.open(QIODevice::WriteOnly) && output.write(bytes) == bytes.size() && output.commit();
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/luckywheel.ico"));
    QCoreApplication::setOrganizationName("LuckyWheel");
    QCoreApplication::setApplicationName("LuckyWheel");
    QCoreApplication::setApplicationVersion(LW_VERSION);
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    auto args = app.arguments();
    int i = args.indexOf("--data-dir");
    const bool reportSqlite = args.contains("--sqlite-runtime-report");
    if (reportSqlite && i < 0)
        return 2;
    if (i >= 0) {
        // A GUI-subsystem executable has no console, so report the usage error visibly.
        if (i + 1 >= args.size() || args[i + 1].isEmpty()) {
            QMessageBox::warning(nullptr, "LuckyWheel",
                                 "--data-dir 後面需要資料目錄路徑。\n--data-dir requires a directory path.");
            return 2;
        }
        dir = QDir(args[i + 1]).absolutePath();
    }
    QDir().mkpath(dir);
    QLockFile lock(dir + "/application.lock");
    lock.setStaleLockTime(0);
    if (!lock.tryLock(0)) {
        QMessageBox::warning(nullptr, "LuckyWheel",
                             "此資料目錄已有程式執行，或鎖定檔無法建立。\nAnother instance uses this data "
                             "directory, or its lock cannot be created.");
        return 3;
    }
    if (!acceptMicrosoftTerms(dir))
        return 4;
    if (reportSqlite)
        return writeSqliteRuntimeReport(dir) ? 0 : 5;
    MainWindow window(dir + "/luckywheel.sqlite");
    if (!window.startupError.isEmpty()) {
        QMessageBox::critical(nullptr, "LuckyWheel", window.startupError);
        return 1;
    }
    window.show();
    return app.exec();
}
