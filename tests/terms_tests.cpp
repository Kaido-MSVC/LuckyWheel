#include "ms_terms.h"
#include <QApplication>
#include <QDialog>
#include <QFile>
#include <QPushButton>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>

class TermsTests : public QObject {
    Q_OBJECT
private slots:
    void acceptanceLifecycle() {
        QTemporaryDir data;
        QVERIFY(data.isValid());
        bool found = false;
        auto respond = [&](bool accept) {
            QTimer::singleShot(0, [&found, accept] {
                auto *dialog = qobject_cast<QDialog *>(QApplication::activeModalWidget());
                if (!dialog || dialog->objectName() != "microsoftTermsDialog") return;
                found = true;
                if (accept) dialog->findChild<QPushButton *>("agreeMicrosoftTerms")->click();
                else dialog->reject();
            });
        };
        const QString marker = data.path() + "/microsoft-terms.sha256";
        respond(false);
        QVERIFY(!acceptMicrosoftTerms(data.path()));
        QVERIFY(found);
        QVERIFY(!QFile::exists(marker));
        found = false;
        respond(true);
        QVERIFY(acceptMicrosoftTerms(data.path()));
        QVERIFY(found);
        QVERIFY(QFile::exists(marker));
        // A valid receipt skips the dialog. A stale or altered receipt prompts again.
        found = false;
        QTimer watchdog;
        connect(&watchdog, &QTimer::timeout, [] {
            if (auto *dialog = qobject_cast<QDialog *>(QApplication::activeModalWidget())) dialog->reject();
        });
        watchdog.start(2000);
        QVERIFY(acceptMicrosoftTerms(data.path()));
        watchdog.stop();
        QFile changed(marker);
        QVERIFY(changed.open(QIODevice::WriteOnly | QIODevice::Truncate));
        changed.write("outdated"); changed.close();
        respond(false);
        QVERIFY(!acceptMicrosoftTerms(data.path()));
        QVERIFY(found);
    }
};
QTEST_MAIN(TermsTests)
#include "terms_tests.moc"
