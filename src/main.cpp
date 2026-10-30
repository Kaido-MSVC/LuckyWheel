#include "window.h"
#include "ms_terms.h"
#include "version.h"
#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QLockFile>
#include <QMessageBox>
#include <QStandardPaths>
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/luckywheel.ico"));
    QCoreApplication::setOrganizationName("LuckyWheel");
    QCoreApplication::setApplicationName("LuckyWheel");
    QCoreApplication::setApplicationVersion(LW_VERSION);
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    auto args = app.arguments();
    int i = args.indexOf("--data-dir");
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
    MainWindow window(dir + "/luckywheel.sqlite");
    if (!window.startupError.isEmpty()) {
        QMessageBox::critical(nullptr, "LuckyWheel", window.startupError);
        return 1;
    }
    window.show();
    return app.exec();
}
