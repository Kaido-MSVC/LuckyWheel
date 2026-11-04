#include "translations.h"
#include "window.h"
#include "formats.h"
#include <QColorDialog>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QJsonDocument>
#include <QCursor>
#include <QScreen>
#include <QStyleOptionButton>
#include <QPainter>
#ifdef Q_OS_WIN
#define NOMINMAX
#include <windows.h>
#endif
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QLocale>
#include <QHeaderView>
#include <QLineEdit>
#include <QEnterEvent>
#include <QHoverEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QSoundEffect>
#include <QStatusBar>
#include <QStackedWidget>
#include <QToolButton>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTimer>
#include <QScrollArea>
#include <QScrollBar>
#include <QSqlQuery>
#include <QTranslator>
#include <QtTest>
#include <cmath>
#include <functional>
#include <memory>
using DialogAction = std::function<bool(QWidget *)>;
static void dialogs(MainWindow &w, QVector<DialogAction> actions) {
    auto *timer = new QTimer(&w);
    auto step = std::make_shared<int>(0);
    auto attempts = std::make_shared<int>(0);
    QObject::connect(timer, &QTimer::timeout, &w, [timer, step, attempts, actions] {
        if (*step >= actions.size()) {
            timer->stop();
            timer->deleteLater();
            return;
        }
        auto *modal = QApplication::activeModalWidget();
        if (modal && actions[*step](modal)) {
            ++*step;
            *attempts = 0;
        } else if (++*attempts > 120) {
            if (auto *d = qobject_cast<QDialog *>(modal))
                d->reject();
            timer->stop();
        }
    });
    timer->start(50);
}
static DialogAction selectFile(const QString &path) {
    return [path](QWidget *m) {
        auto *d = qobject_cast<QFileDialog *>(m);
        if (!d)
            return false;
        auto *edit = d->findChild<QLineEdit *>("fileNameEdit");
        if (!edit)
            return false;
        edit->setText(path);
        QTimer::singleShot(150, d, [d] { QMetaObject::invokeMethod(d, "accept", Qt::QueuedConnection); });
        return true;
    };
}
static DialogAction confirmMessage() {
    return [](QWidget *m) {
        auto *d = qobject_cast<QMessageBox *>(m);
        if (!d)
            return false;
        for (auto *b : d->buttons())
            if (d->buttonRole(b) == QMessageBox::AcceptRole) {
                b->click();
                return true;
            }
        return false;
    };
}
static DialogAction confirmPreview() {
    return [](QWidget *m) {
        if (qobject_cast<QFileDialog *>(m) || qobject_cast<QMessageBox *>(m))
            return false;
        auto *box = m->findChild<QDialogButtonBox *>();
        if (!box)
            return false;
        for (auto *b : box->buttons())
            if (box->buttonRole(b) == QDialogButtonBox::AcceptRole) {
                b->click();
                return true;
            }
        return false;
    };
}
class UiTests : public QObject {
    Q_OBJECT
  private slots:
    void smallDesktopCanReachSettings() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite")); w.show();
        const QSize desktop = w.screen()->availableGeometry().size() - QSize(40, 80);
        QVERIFY(w.minimumWidth() <= desktop.width());
        QVERIFY(w.minimumHeight() <= desktop.height());
        w.resize(w.minimumSize());
        w.findChild<QStackedWidget *>("pages")->setCurrentIndex(3);
        auto *scroll = w.findChild<QScrollArea *>("pagesScroll");
        auto *about = w.findChild<QPushButton *>("about");
        scroll->ensureWidgetVisible(about, 0, 0); QTest::qWait(25);
        QVERIFY(scroll->viewport()->rect().contains(QRect(about->mapTo(scroll->viewport(), QPoint()), about->size())));
        w.findChild<QStackedWidget *>("pages")->setCurrentIndex(2);
        QCOMPARE(scroll->verticalScrollBar()->value(), 0);
        QCOMPARE(scroll->horizontalScrollBar()->value(), 0);
    }
    void firstRunSystemLanguageAndManualPersistence() {
        QTemporaryDir dir;
        const QString db = dir.filePath("data.sqlite");
        const auto expected = lw::languageForUiLanguages(QLocale::system().uiLanguages());
        qInfo() << "Windows UI preferences:" << QLocale::system().uiLanguages() << "selected:" << expected;
        {
            MainWindow w(db);
            QVERIFY2(w.startupError.isEmpty(), qPrintable(w.startupError));
            QCOMPARE(w.configuration().language, expected);
            QCOMPARE(w.findChild<QComboBox *>("languageBox")->currentData().toString(), expected);
        }
        for (const auto &code : QStringList{"zh_TW", "zh_CN", "en", "ja", "ko", "es", "fr", "de", "pt"}) {
            QJsonObject saved;
            {
                MainWindow w(db);
                const auto before = lw::toJson(w.configuration());
                auto *box = w.findChild<QComboBox *>("languageBox");
                box->setCurrentIndex(box->findData(code));
                saved = lw::toJson(w.configuration());
                auto unchanged = saved; unchanged["language"] = before["language"];
                QCOMPARE(unchanged, before);
            }
            MainWindow reopened(db);
            QCOMPARE(lw::toJson(reopened.configuration()), saved);
            QCOMPARE(reopened.configuration().language, code);
        }
    }
    void literalPrizeNamesAndHistoryColumns() {
        QTemporaryDir dir;
        const auto db = dir.filePath("data.sqlite");
        const QString name = "<b>Prize & gift</b>";
        QString error;
        {
            lw::Store store; QVERIFY(store.open(db, &error));
            auto cfg = lw::exampleConfig();
            cfg.prizes = {{"html", name, lw::Scale, -1, -1, QColor("#72b8ff")}};
            QVERIFY(store.save(cfg, &error));
        }
        MainWindow w(db); w.show(); w.resize(820, 620);
        auto *summary = w.findChild<QLabel *>("probabilitySummary");
        auto *result = w.findChild<QLabel *>("resultLabel");
        QCOMPARE(summary->textFormat(), Qt::PlainText);
        QVERIFY(summary->text().contains(name));
        QCOMPARE(result->textFormat(), Qt::PlainText);
        w.findChild<QPushButton *>("spinButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(!w.isBusy(), 6000);
        QVERIFY(result->text().contains(name));
        auto *history = w.findChild<QTableWidget *>("historyTable");
        QCOMPARE(history->item(0, 3)->text(), name);
        w.findChild<QStackedWidget *>("pages")->setCurrentIndex(2);
        for (int lang = 0; lang < 9; ++lang) {
            w.findChild<QComboBox *>("languageBox")->setCurrentIndex(lang);
            QTest::qWait(25);
            for (int col : {0, 1, 2, 4, 5}) {
                QCOMPARE(history->horizontalHeader()->sectionResizeMode(col), QHeaderView::ResizeToContents);
                QVERIFY(history->columnWidth(col) >= history->fontMetrics().horizontalAdvance(history->item(0, col)->text()));
            }
        }
    }
    void languageSwitchRefreshesSavedAndLastMiss() {
        QTemporaryDir dir;
        QString error;
        {
            lw::Store store; QVERIFY(store.open(dir.filePath("data.sqlite"), &error));
            auto cfg = lw::exampleConfig();
            for (auto &prize : cfg.prizes) prize.weight = 0;
            QVERIFY(store.save(cfg, &error));
        }
        MainWindow w(dir.filePath("data.sqlite")); w.show();
        auto *languages = w.findChild<QComboBox *>("languageBox");
        w.findChild<QPushButton *>("savePrizes")->click();
        languages->setCurrentIndex(2);
        QCOMPARE(w.findChild<QLabel *>("resultLabel")->text(), uiText("saved", "en"));
        w.findChild<QPushButton *>("spinButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(!w.isBusy(), 6000);
        for (int i = 0; i < languages->count(); ++i) {
            languages->setCurrentIndex(i);
            const auto code = languages->currentData().toString();
            QCOMPARE(w.findChild<QLabel *>("resultLabel")->text(), uiText("lastResult", code) + "\n" + uiText("miss", code));
            QCOMPARE(w.findChild<QTableWidget *>("historyTable")->item(0, 3)->text(), uiText("miss", code));
        }
        QCOMPARE(w.recordCount(), 1);
    }
    void moreOptionsPersistenceAndNineLanguageLayout() {
        QTemporaryDir dir;
        const auto db = dir.filePath("data.sqlite");
        {
            MainWindow w(db); w.show();
            auto *pages = w.findChild<QStackedWidget *>("pages");
            auto *toggle = w.findChild<QToolButton *>("moreOptionsToggle");
            auto *scroll = w.findChild<QScrollArea *>("moreOptionsScroll");
            auto *language = w.findChild<QComboBox *>("languageBox");
            const auto before = lw::toJson(w.configuration());
            QVERIFY(!toggle->isChecked());
            pages->setCurrentIndex(1);
            for (int lang = 0; lang < 9; ++lang) {
                language->setCurrentIndex(lang);
                const auto code = language->currentData().toString();
                QCOMPARE(w.findChild<QLabel *>("resultLabel")->text(), uiText("ready", code));
                QCOMPARE(toggle->text(), uiText("moreOptions", code));
                QTranslator translator;
                // English is the Qt source language; other eight languages must load from the EXE.
                if (code != "en") QVERIFY(translator.load(":/translations/qtbase_" + (code == "pt" ? QString("pt_BR") : code) + ".qm"));
                for (const auto &size : {QSize(820, 620), QSize(1160, 830)}) {
                    w.resize(size);
                    for (bool expanded : {false, true}) {
                        toggle->setChecked(expanded);
                        QTest::qWait(25);
                        QCOMPARE(scroll->isVisible(), expanded);
                        for (auto name : {"addPrize", "removePrize", "savePrizes"}) {
                            auto *b = w.findChild<QPushButton *>(name);
                            QVERIFY(b->isVisible());
                            w.findChild<QScrollArea *>("pagesScroll")->ensureWidgetVisible(b, 0, 0);
                            QTest::qWait(5);
                            QVERIFY(w.rect().contains(QRect(b->mapTo(&w, QPoint()), b->size())));
                            QVERIFY2(b->width() >= b->minimumSizeHint().width(), qPrintable(code + ": " + name));
                        }
                        if (expanded) for (auto *b : scroll->findChildren<QPushButton *>()) {
                            scroll->ensureWidgetVisible(b); QTest::qWait(5);
                            QVERIFY2(b->width() >= b->minimumSizeHint().width(), qPrintable(code + ": " + b->objectName()));
                            QVERIFY(scroll->viewport()->rect().intersects(QRect(b->mapTo(scroll->viewport(), QPoint()), b->size())));
                        }
                        const auto output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
                        if (!output.isEmpty()) {
                            QDir().mkpath(output);
                            QVERIFY(w.grab().save(output + QString("/prizes-%1-%2-%3.png").arg(code).arg(size.width()).arg(expanded)));
                        }
                    }
                }
                for (int page = 0; page < 4; ++page) {
                    pages->setCurrentIndex(page); QTest::qWait(15);
                    const auto output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
                    if (!output.isEmpty()) QVERIFY(w.grab().save(output + QString("/page-%1-%2.png").arg(code).arg(page)));
                }
                pages->setCurrentIndex(1);
                bool helpMatches = false;
                dialogs(w, {[&](QWidget *modal) {
                    if (modal->objectName() != "formatExamplesDialog") return false;
                    auto *format = modal->findChild<QComboBox *>("exampleFormat");
                    auto *help = modal->findChild<QPlainTextEdit *>("formatHelp");
                    helpMatches = help->toPlainText() == uiText("formatShared", code) + "\n" + uiText("formatCsv", code);
                    format->setCurrentIndex(1);
                    helpMatches = helpMatches && help->toPlainText() == uiText("formatShared", code) + "\n" + uiText("formatJson", code);
                    qobject_cast<QDialog *>(modal)->reject(); return true;
                }});
                w.findChild<QPushButton *>("formatExamples")->click();
                QVERIFY(helpMatches);
            }
            auto after = lw::toJson(w.configuration()); after["language"] = before["language"];
            QCOMPARE(after, before);
            toggle->setChecked(true);
        }
        MainWindow reopened(db);
        QVERIFY(reopened.findChild<QToolButton *>("moreOptionsToggle")->isChecked());
        reopened.findChild<QToolButton *>("moreOptionsToggle")->setChecked(false);
        MainWindow collapsed(db);
        QVERIFY(!collapsed.findChild<QToolButton *>("moreOptionsToggle")->isChecked());
    }
    void aboutShowsLicenses() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        const QString lang = w.configuration().language;
        QString shown;
        dialogs(w, {[&](QWidget *modal) {
            if (modal->objectName() != "aboutDialog") return false;
            auto *aboutText = modal->findChild<QPlainTextEdit *>("aboutText");
            shown = aboutText->toPlainText();
            const QString output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
            if (!output.isEmpty()) {
                QDir().mkpath(output);
                modal->grab().save(output + "/about-license.png");
                QTextCursor cursor = aboutText->textCursor();
                cursor.setPosition(shown.indexOf("Qt translations 6.12.0 — BSD-3-Clause"));
                aboutText->setTextCursor(cursor);
                aboutText->ensureCursorVisible();
                modal->grab().save(output + "/about-bsd.png");
            }
            qobject_cast<QDialog *>(modal)->reject(); return true;
        }});
        w.findChild<QPushButton *>("about")->click();
        QVERIFY(shown.startsWith(uiText("aboutLicense", lang)));
        QVERIFY(shown.contains(uiText("thirdParty", lang)));
        QVERIFY(shown.contains("GNU LGPL v3"));
        QVERIFY(shown.contains("Qt translations 6.12.0"));
        QVERIFY(shown.contains("BSD-3-Clause"));
        QVERIFY(shown.contains("Copyright (C) The Qt Company Ltd. and other contributors."));
        QVERIFY(shown.contains("Redistributions in binary form"));
        QVERIFY(shown.contains("THIS SOFTWARE IS PROVIDED"));
        QVERIFY(shown.contains("SOURCE-DELIVERY.md"));
        QVERIFY(shown.contains("GNU LGPL v2.1 or later"));
        QVERIFY(shown.contains("Copyright (c) 2026 kaido-MSVC"));
        QVERIFY(shown.contains("Permission is hereby granted"));
    }
    void failedSaveRetainsDraftAndCanRetry() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        auto *table = w.findChild<QTableWidget *>("prizeTable");
        const auto before = lw::toJson(w.configuration());
        table->item(0, 0)->setText("Retry draft");
        QSqlDatabase connection;
        for (const auto &name : QSqlDatabase::connectionNames()) {
            auto candidate = QSqlDatabase::database(name);
            if (candidate.databaseName() == dir.filePath("data.sqlite")) connection = candidate;
        }
        QVERIFY(connection.isValid());
        QSqlQuery query(connection);
        QVERIFY(query.exec("PRAGMA query_only=ON"));
        bool sawFailure = false;
        dialogs(w, {[&](QWidget *modal) {
            auto *box = qobject_cast<QMessageBox *>(modal);
            if (!box) return false;
            sawFailure = box->text().contains(uiText("saveFailed", w.configuration().language));
            for (auto *b : box->buttons()) if (box->buttonRole(b) == QMessageBox::AcceptRole) { b->click(); return true; }
            return false;
        }});
        w.findChild<QPushButton *>("savePrizes")->click();
        QVERIFY(sawFailure);
        QCOMPARE(lw::toJson(w.configuration()), before);
        QCOMPARE(table->item(0, 0)->text(), QString("Retry draft"));
        QVERIFY(w.isWindowModified());
        QVERIFY(query.exec("PRAGMA query_only=OFF"));
        w.findChild<QPushButton *>("savePrizes")->click();
        QCOMPARE(w.configuration().prizes[0].name, QString("Retry draft"));
        QVERIFY(!w.isWindowModified());
    }
    void importedColorDoubleClickKeepsStockAndHex() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite")); w.show();
        const auto path = dir.filePath("stock.csv");
        QFile file(path); QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("name,probability,limit,color,remaining\nImported,10.0000,5,#abcdef,2\n"); file.close();
        dialogs(w, {selectFile(path), confirmPreview()});
        w.findChild<QPushButton *>("importCsv")->click();
        auto *table = w.findChild<QTableWidget *>("prizeTable");
        QCOMPARE(table->currentColumn(), 4);
        QCOMPARE(w.configuration().prizes[0].remaining, 2);
        w.findChild<QStackedWidget *>("pages")->setCurrentIndex(1);
        const auto before = lw::toJson(w.configuration());
        bool sawInitial = false;
        dialogs(w, {[&](QWidget *modal) {
            auto *dialog = qobject_cast<QColorDialog *>(modal); if (!dialog) return false;
            sawInitial = dialog->currentColor() == QColor("#abcdef");
            dialog->setCurrentColor(QColor("#123abc")); dialog->accept(); return true;
        }});
        QMetaObject::invokeMethod(table, "cellDoubleClicked", Qt::DirectConnection, Q_ARG(int, 0), Q_ARG(int, 4));
        QVERIFY(sawInitial);
        QCOMPARE(table->item(0, 4)->text(), QString("#123abc"));
        QCOMPARE(lw::toJson(w.configuration()), before);
        w.findChild<QPushButton *>("savePrizes")->click();
        QCOMPARE(w.configuration().prizes[0].color.name(), QString("#123abc"));
        QCOMPARE(w.configuration().prizes[0].remaining, 2);
    }
    void buttonHoverStyleRendering() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        w.show();
        auto *pages = w.findChild<QStackedWidget *>("pages");
        auto render = [](QPushButton *button, bool hover) {
            QImage image(button->size(), QImage::Format_ARGB32_Premultiplied);
            image.fill(Qt::transparent);
            QStyleOptionButton option; option.initFrom(button);
            option.text = button->text(); option.icon = button->icon();
            option.state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus | QStyle::State_Sunken);
            option.state |= QStyle::State_Enabled;
            if (hover) option.state |= QStyle::State_MouseOver;
            QPainter painter(&image);
            button->style()->drawControl(QStyle::CE_PushButton, &option, &painter, button);
            return image;
        };
        for (int theme = 0; theme < 2; ++theme) {
            w.findChild<QComboBox *>("themeBox")->setCurrentIndex(theme);
            for (int page = 0; page < pages->count(); ++page) {
                pages->setCurrentIndex(page); QTest::qWait(20);
                for (auto *button : pages->widget(page)->findChildren<QPushButton *>())
                    QVERIFY2(render(button, false) != render(button, true), qPrintable(button->objectName()));
            }
        }
    }
    void sequenceScreenAndExportAfterClear() {
        QTemporaryDir dir;
        QString e;
        qint64 last = 0;
        {
            lw::Store s;
            QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
            auto c = lw::exampleConfig(); c.round = 8;
            QVERIFY(s.save(c, &e));
            lw::Record r;
            for (int i = 0; i < 2; ++i) QVERIFY(s.draw(c, lw::sample(c, 900000), "Miss", &r, &e));
            last = r.id;
            const auto before = lw::toJson(c);
            qint64 deleted;
            QVERIFY(s.clearHistory(2, &deleted, &e));
            QCOMPARE(lw::toJson(s.load(&e)), before);
            for (int i = 0; i < 2; ++i) QVERIFY(s.draw(c, lw::sample(c, 900000), "Miss", &r, &e));
            QVERIFY(s.history(&e).last().id > last);
        }
        MainWindow w(dir.filePath("data.sqlite"));
        auto *table = w.findChild<QTableWidget *>("historyTable");
        QCOMPARE(table->item(0, 0)->text(), QString("2"));
        QCOMPARE(table->item(1, 0)->text(), QString("1"));
        const auto path = dir.filePath("history.csv");
        dialogs(w, {selectFile(path)});
        w.findChild<QPushButton *>("exportHistory")->click();
        QFile file(path); QVERIFY(file.open(QIODevice::ReadOnly));
        auto rows = lw::decodeCsv(QString::fromUtf8(file.readAll()), &e); QVERIFY(rows);
        QCOMPARE(rows->first().first(), QString("sequence"));
        for (int i = 0; i < 2; ++i) QCOMPARE(rows->at(i + 1).first(), table->item(i, 0)->text());
        QCOMPARE(w.configuration().round, 8);
    }
    void builtInExamplesSaveAndImport() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        const auto before = lw::toJson(w.configuration());
        for (int format = 0; format < 2; ++format) {
            const auto path = dir.filePath(format ? "example.json" : "example.csv");
            bool sawExample = false;
            dialogs(w, {
                [&, format](QWidget *m) {
                    if (m->objectName() != "formatExamplesDialog") return false;
                    m->findChild<QComboBox *>("exampleFormat")->setCurrentIndex(format);
                    sawExample = !m->findChild<QPlainTextEdit *>("exampleContent")->toPlainText().isEmpty();
                    auto *box = m->findChild<QDialogButtonBox *>();
                    for (auto *b : box->buttons()) if (box->buttonRole(b) == QDialogButtonBox::ActionRole) {
                        QTimer::singleShot(0, b, [b] { b->click(); }); return true;
                    }
                    return false;
                }, selectFile(path), [](QWidget *m) {
                    if (m->objectName() != "formatExamplesDialog") return false;
                    qobject_cast<QDialog *>(m)->reject(); return true;
                }});
            w.findChild<QPushButton *>("formatExamples")->click();
            QVERIFY(sawExample); QVERIFY(QFile::exists(path));
            QCOMPARE(lw::toJson(w.configuration()), before);
        }
        dialogs(w, {selectFile(dir.filePath("example.csv")), confirmPreview()});
        w.findChild<QPushButton *>("importCsv")->click();
        QCOMPARE(w.configuration().prizes[0].name, lw::exampleConfig().prizes[0].name);
        dialogs(w, {selectFile(dir.filePath("example.json")), confirmMessage()});
        w.findChild<QPushButton *>("importConfig")->click();
        QCOMPARE(lw::toJson(w.configuration()), lw::toJson(lw::exampleConfig()));
    }
    void invalidImportPreservesSavedDataAndDraft() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        auto *table = w.findChild<QTableWidget *>("prizeTable");
        table->item(0, 0)->setText("Uncommitted draft");
        const auto before = lw::toJson(w.configuration());
        for (int json = 0; json < 2; ++json) {
            auto path = dir.filePath(json ? "bad.json" : "bad.csv");
            QFile file(path); QVERIFY(file.open(QIODevice::WriteOnly));
            if (json) {
                auto obj = before; obj["sound"] = "false";
                file.write(QJsonDocument(obj).toJson());
            } else file.write("name,probability,limit,color\nBad,101,2,#abcdef\n");
            file.close();
            bool detailed = false;
            dialogs(w, {selectFile(path), [&, json](QWidget *m) {
                auto *box = qobject_cast<QMessageBox *>(m); if (!box) return false;
                const auto diagnostic = box->text() + "\n" + box->detailedText();
                detailed = json ? diagnostic.contains("sound") : diagnostic.contains("line 2 / probability");
                box->reject(); return true;
            }});
            w.findChild<QPushButton *>(json ? "importConfig" : "importCsv")->click();
            QVERIFY(detailed);
            QCOMPARE(lw::toJson(w.configuration()), before);
            QCOMPARE(table->item(0, 0)->text(), QString("Uncommitted draft"));
            QVERIFY(w.isWindowModified());
        }
        w.findChild<QPushButton *>("reloadPrizes");
        dialogs(w, {[](QWidget *m) {
            auto *box = qobject_cast<QMessageBox *>(m); if (!box) return false;
            for (auto *b : box->buttons()) if (box->buttonRole(b) == QMessageBox::DestructiveRole) { b->click(); return true; }
            return false;
        }});
        w.findChild<QPushButton *>("reloadPrizes")->click();
        QVERIFY(!w.isWindowModified());
    }
    void colorDraftPickerPreviewAndReload() {
        QTemporaryDir dir;
        const auto db = dir.filePath("data.sqlite");
        MainWindow w(db); w.show();
        auto *table = w.findChild<QTableWidget *>("prizeTable");
        w.findChild<QStackedWidget *>("pages")->setCurrentIndex(1);
        table->setCurrentCell(0, 4);
        const auto before = lw::toJson(w.configuration());
        const auto originalColor = w.configuration().prizes[0].color;
        dialogs(w, {[](QWidget *m) {
            auto *d = qobject_cast<QColorDialog *>(m); if (!d) return false;
            d->setCurrentColor(QColor("#123abc")); d->accept(); return true;
        }});
        w.findChild<QPushButton *>("chooseColor")->click();
        QCOMPARE(table->item(0, 4)->text(), QString("#123abc"));
        QVERIFY(w.isWindowModified());
        QCOMPARE(lw::toJson(w.configuration()), before);
        QCOMPARE(w.findChild<WheelWidget *>("wheelCanvas")->getSlices()[0].color, originalColor);
        dialogs(w, {[](QWidget *m) {
            auto *d = qobject_cast<QColorDialog *>(m); if (!d) return false;
            d->setCurrentColor(QColor("#ffffff")); d->reject(); return true;
        }});
        w.findChild<QPushButton *>("chooseColor")->click();
        QCOMPARE(table->item(0, 4)->text(), QString("#123abc"));
        // The preview commits an active HEX editor into the draft, never into the database.
        table->editItem(table->item(0, 4));
        QTest::qWait(20);
        auto *editor = table->findChild<QLineEdit *>(); QVERIFY(editor);
        editor->setText("#fedcba");
        bool draftColorSeen = false;
        dialogs(w, {[&](QWidget *m) {
            if (m->objectName() != "draftPreviewDialog") return false;
            draftColorSeen = m->findChild<WheelWidget *>("draftPreviewWheel")->getSlices()[0].color == QColor("#fedcba");
            const auto output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
            if (!output.isEmpty()) m->grab().save(output + "/draft-preview.png");
            qobject_cast<QDialog *>(m)->reject(); return true;
        }});
        w.findChild<QPushButton *>("draftPreview")->click();
        QVERIFY(draftColorSeen); QCOMPARE(lw::toJson(w.configuration()), before);
        w.findChild<QPushButton *>("autoColors")->click();
        QVERIFY(table->item(0, 4)->text() != table->item(1, 4)->text());
        QCOMPARE(lw::toJson(w.configuration()), before);
        w.findChild<QPushButton *>("defaultColors")->click();
        QCOMPARE(table->item(0, 4)->text(), lw::defaultColor(0).name());
        QCOMPARE(table->item(1, 4)->text(), lw::defaultColor(1).name());
        table->item(0, 4)->setText("#123abc");
        w.findChild<QPushButton *>("savePrizes")->click();
        QCOMPARE(w.configuration().prizes[0].color.name(), QString("#123abc"));
        MainWindow reopened(db);
        QCOMPARE(reopened.configuration().prizes[0].color.name(), QString("#123abc"));
        const auto path = dir.filePath("colors.csv");
        dialogs(w, {selectFile(path)});
        w.findChild<QPushButton *>("exportPrizeCsv")->click();
        dialogs(w, {selectFile(path), confirmPreview()});
        w.findChild<QPushButton *>("importCsv")->click();
        QCOMPARE(w.configuration().prizes[0].color.name(), QString("#123abc"));
    }
    void sidebarNavigationAndPersistence() {
        QTemporaryDir dir;
        const auto db = dir.filePath("data.sqlite");
        {
            MainWindow w(db);
            w.show();
            auto *pages = w.findChild<QStackedWidget *>("pages");
            auto *toggle = w.findChild<QToolButton *>("sidebarToggle");
            auto *wheel = w.findChild<WheelWidget *>("wheelCanvas");
            QTest::qWait(80);
            const auto before = lw::toJson(w.configuration());
            const int expanded = wheel->width();
            toggle->click();
            QTest::qWait(80);
            QVERIFY(wheel->width() > expanded);
            QCOMPARE(w.findChild<QWidget *>("sidebar")->width(), 56);
            for (int i = 0; i < 4; ++i) {
                auto *nav = w.findChild<QToolButton *>(QString("nav%1").arg(i));
                QVERIFY(!nav->accessibleName().isEmpty());
                nav->click();
                QCOMPARE(pages->currentIndex(), i);
                QVERIFY(nav->isChecked());
            }
            QCOMPARE(lw::toJson(w.configuration()), before);
        }
        MainWindow reopened(db);
        QCOMPARE(reopened.findChild<QWidget *>("sidebar")->width(), 56);
        reopened.findChild<QToolButton *>("sidebarToggle")->click();
        MainWindow expanded(db);
        QCOMPARE(expanded.findChild<QWidget *>("sidebar")->width(), 206);
    }
    void unsavedNavigationCloseAndDraw() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        w.show();
        auto *table = w.findChild<QTableWidget *>("prizeTable");
        auto *pages = w.findChild<QStackedWidget *>("pages");
        auto *navPrize = w.findChild<QToolButton *>("nav1");
        auto *navDraw = w.findChild<QToolButton *>("nav0");
        auto choose = [](QMessageBox::ButtonRole role) -> DialogAction {
            return [role](QWidget *modal) {
                auto *box = qobject_cast<QMessageBox *>(modal);
                if (!box || box->objectName() != "unsavedDialog") return false;
                if (box->buttonRole(box->defaultButton()) != QMessageBox::RejectRole) return false;
                for (auto *button : box->buttons())
                    if (box->buttonRole(button) == role) { button->click(); return true; }
                return false;
            };
        };
        navPrize->click();
        const auto before = lw::toJson(w.configuration());
        table->item(0, 0)->setText("Draft");
        QVERIFY(w.isWindowModified());
        dialogs(w, {choose(QMessageBox::RejectRole)});
        navDraw->click();
        QCOMPARE(pages->currentIndex(), 1);
        QVERIFY(navPrize->isChecked());
        QCOMPARE(table->item(0, 0)->text(), QString("Draft"));
        QCOMPARE(lw::toJson(w.configuration()), before);
        dialogs(w, {choose(QMessageBox::RejectRole)});
        QVERIFY(!w.close());
        QVERIFY(w.isVisible());
        dialogs(w, {choose(QMessageBox::RejectRole)});
        w.findChild<QPushButton *>("spinButton")->click();
        QVERIFY(!w.isBusy());
        QCOMPARE(w.recordCount(), 0);
        dialogs(w, {choose(QMessageBox::DestructiveRole)});
        navDraw->click();
        QCOMPARE(pages->currentIndex(), 0);
        QVERIFY(!w.isWindowModified());
        QCOMPARE(lw::toJson(w.configuration()), before);
        navPrize->click();
        w.activateWindow();
        table->setFocus();
        QTest::qWait(50);
        table->setCurrentCell(0, 0);
        table->editItem(table->item(0, 0));
        QTRY_VERIFY(qobject_cast<QLineEdit *>(QApplication::focusWidget()));
        qobject_cast<QLineEdit *>(QApplication::focusWidget())->setText("Committed editor");
        dialogs(w, {choose(QMessageBox::AcceptRole)});
        navDraw->click();
        QCOMPARE(w.configuration().prizes[0].name, QString("Committed editor"));
        QCOMPARE(pages->currentIndex(), 0);
        QVERIFY(!w.isWindowModified());
        navPrize->click();
        table->item(0, 1)->setText("101");
        dialogs(w, {choose(QMessageBox::AcceptRole), confirmMessage()});
        navDraw->click();
        QCOMPARE(pages->currentIndex(), 1);
        QVERIFY(w.isWindowModified());
        dialogs(w, {choose(QMessageBox::DestructiveRole)});
        w.findChild<QPushButton *>("reloadPrizes")->click();
        QVERIFY(!w.isWindowModified());
        w.findChild<QPushButton *>("addPrize")->click();
        QVERIFY(w.isWindowModified());
        table->selectRow(table->rowCount() - 1);
        w.findChild<QPushButton *>("removePrize")->click();
        QVERIFY(!w.isWindowModified());
        table->item(0, 0)->setText("Saved on close");
        dialogs(w, {choose(QMessageBox::AcceptRole)});
        QVERIFY(w.close());
        MainWindow reopened(dir.filePath("data.sqlite"));
        QCOMPARE(reopened.configuration().prizes[0].name, QString("Saved on close"));
    }
    void longLabelsAndRotationScreenshots() {
        QTemporaryDir dir;
        QString e;
        {
            lw::Store store;
            QVERIFY(store.open(dir.filePath("data.sqlite"), &e));
            lw::Config cfg;
            cfg.prizes = {{"long", "特別獎：雙人豪華住宿與精選下午茶體驗套組", 300000, -1, -1, QColor("#72b8ff")},
                          {"latin", "Premium weekend getaway with afternoon tea", 300000, 5, 5, QColor("#a8dca6")}};
            QVERIFY(store.save(cfg, &e));
        }
        MainWindow w(dir.filePath("data.sqlite"));
        w.show();
        auto *wheel = w.findChild<WheelWidget *>("wheelCanvas");
        QCOMPARE(wheel->getSlices()[0].label, w.configuration().prizes[0].name);
        QVERIFY(w.findChild<QLabel *>("probabilitySummary")->text().contains(w.configuration().prizes[0].name));
        const QString output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
        if (output.isEmpty()) return;
        QDir().mkpath(output);
        for (int rotation : {0, 90, 180, 270, 359}) {
            wheel->setRotation(rotation);
            QTest::qWait(40);
            QVERIFY(w.grab().save(output + QString("/rotation-%1.png").arg(rotation)));
        }
        wheel->setRotation(0);
        for (int expanded = 1; expanded >= 0; --expanded) {
            for (const QSize requested : {QSize(820, 620), QSize(1160, 830), QSize(1600, 1000)}) {
                const QSize desktop = w.screen()->availableGeometry().size() - QSize(40, 80);
                const QSize size = requested.boundedTo(desktop).expandedTo(w.minimumSize());
                w.resize(size);
                QTest::qWait(60);
                QVERIFY(w.grab().save(output + QString("/layout-%1-%2x%3.png").arg(expanded).arg(size.width()).arg(size.height())));
                QCOMPARE(w.size(), size);
            }
            if (expanded) w.findChild<QToolButton *>("sidebarToggle")->click();
        }
    }
    void clearHistoryConfirmCancelAndRefresh() {
        QTemporaryDir dir;
        QString e;
        {
            lw::Store s;
            QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
            lw::Config c;
            c.round = 5;
            c.prizes = {{"a", "A", lw::Scale, 3, 3, QColor("#abcdef")}};
            QVERIFY(s.save(c, &e));
            QVERIFY(s.draw(c, lw::sample(c, 0), "Miss", nullptr, &e));
        }
        MainWindow w(dir.filePath("data.sqlite"));
        const auto before = lw::toJson(w.configuration());
        auto *clear = w.findChild<QPushButton *>("clearHistory");
        QVERIFY(clear && clear->isEnabled());
        auto reject = [](QWidget *m) {
            auto *box = qobject_cast<QMessageBox *>(m);
            if (!box) return false;
            if (!box->text().contains("1")) return false;
            box->reject();
            return true;
        };
        dialogs(w, {reject});
        clear->click();
        QCOMPARE(w.recordCount(), 1);
        dialogs(w, {confirmMessage(), reject});
        clear->click();
        QCOMPARE(w.recordCount(), 1);
        dialogs(w, {confirmMessage(), confirmMessage(), confirmMessage()});
        clear->click();
        QCOMPARE(w.recordCount(), 0);
        QVERIFY(!clear->isEnabled());
        QCOMPARE(lw::toJson(w.configuration()), before);
        QVERIFY(w.findChild<QLabel *>("historySummary")->text().contains("0"));
        dialogs(w, {confirmPreview()});
        w.findChild<QPushButton *>("statistics")->click();
        MainWindow reopened(dir.filePath("data.sqlite"));
        QCOMPARE(reopened.recordCount(), 0);
        QCOMPARE(lw::toJson(reopened.configuration()), before);
    }
    void initTestCase() {
        QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    }
    void doubleClickAndRestart() {
        QTemporaryDir dir;
        {
            MainWindow w(dir.filePath("data.sqlite"));
            QVERIFY2(w.startupError.isEmpty(), qPrintable(w.startupError));
            w.show();
            auto *spin = w.findChild<QPushButton *>("spinButton");
            auto *table = w.findChild<QTableWidget *>("prizeTable");
            QVERIFY(spin);
            spin->click();
            QVERIFY(w.isBusy());
            QVERIFY(!spin->isEnabled());
            QVERIFY(!table->isEnabled());
            spin->click();
            spin->click();
            QTRY_VERIFY_WITH_TIMEOUT(!w.isBusy(), 6000);
            QCOMPARE(w.recordCount(), 1);
            QVERIFY(spin->isEnabled());
            QVERIFY(table->isEnabled());
        }
        MainWindow reopened(dir.filePath("data.sqlite"));
        QCOMPARE(reopened.recordCount(), 1);
    }
    void nineLanguagesAndTheme() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        auto *box = w.findChild<QComboBox *>("languageBox");
        auto *tabs = w.findChild<QStackedWidget *>("pages");
        QCOMPARE(box->count(), 9);
        for (int i = 0; i < 9; ++i) {
            box->setCurrentIndex(i);
            QCOMPARE(w.findChild<QToolButton *>("nav0")->toolTip(), uiText("wheel", box->itemData(i).toString()));
            QCOMPARE(w.findChild<QPushButton *>("spinButton")->text(),
                     uiText("spin", box->itemData(i).toString()));
        }
        auto *theme = w.findChild<QComboBox *>("themeBox");
        theme->setCurrentIndex(1);
        QCOMPARE(w.configuration().theme, QString("dark"));
        MainWindow reload(dir.filePath("data.sqlite"));
        QCOMPARE(reload.configuration().language, QString("pt"));
        QCOMPARE(reload.configuration().theme, QString("dark"));
    }
    void savedEditingAndSoldOutGeometry() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        auto *table = w.findChild<QTableWidget *>("prizeTable");
        table->item(0, 3)->setText("0");
        QCOMPARE(w.configuration().prizes[0].remaining, 3);
        w.findChild<QPushButton *>("savePrizes")->click();
        QCOMPARE(w.configuration().prizes[0].remaining, 0);
        auto *show = w.findChild<QCheckBox *>("showSold");
        show->setChecked(false);
        auto view = w.findChild<WheelWidget *>("wheelCanvas")->getSlices();
        QCOMPARE(view.size(), 2);
        QCOMPARE(view[0].weight, quint32(200000));
        QCOMPARE(view[1].weight, quint32(800000));
    }
    void addedProbabilityAndActiveEditor() {
        QTemporaryDir dir;
        const QString db = dir.filePath("data.sqlite");
        {
            MainWindow w(db);
            w.show();
            auto *tabs = w.findChild<QStackedWidget *>("pages");
            tabs->setCurrentIndex(1);
            auto *table = w.findChild<QTableWidget *>("prizeTable");
            auto *save = w.findChild<QPushButton *>("savePrizes");
            auto *wheel = w.findChild<WheelWidget *>("wheelCanvas");
            w.findChild<QPushButton *>("addPrize")->click();
            table->item(2, 0)->setText("C");
            save->click();
            QCOMPARE(w.configuration().prizes.size(), 3);
            QCOMPARE(wheel->getSlices().size(), 3); // A, B, no prize; C is 0%.
            QCOMPARE(lw::missWeight(w.configuration()), quint32(700000));
            QVERIFY(w.statusBar()->currentMessage().contains("0%"));
            table->setCurrentCell(2, 1);
            table->editItem(table->item(2, 1));
            QTRY_VERIFY(qobject_cast<QLineEdit *>(QApplication::focusWidget()));
            auto *editor = qobject_cast<QLineEdit *>(QApplication::focusWidget());
            editor->setText("12.3456");
            // A click invoked without moving focus must commit the pending editor.
            save->click();
            QCOMPARE(w.configuration().prizes[2].weight, quint32(123456));
            QCOMPARE(wheel->getSlices().size(), 4);
            QCOMPARE(wheel->getSlices()[2].label, QString("C"));
            QCOMPARE(wheel->getSlices()[2].weight, quint32(123456));
            QCOMPARE(lw::missWeight(w.configuration()), quint32(576544));
            QVERIFY(w.findChild<QLabel *>("probabilitySummary")->text().contains("57.6544%"));
            const QString output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
            if (!output.isEmpty()) {
                QDir().mkpath(output);
                tabs->setCurrentIndex(0);
                QTest::qWait(50);
                QVERIFY(w.grab().save(output + "/saved-positive.png"));
                tabs->setCurrentIndex(1);
                QVERIFY(w.grab().save(output + "/saved-prizes.png"));
            }
            w.findChild<QPushButton *>("reloadPrizes")->click();
            QCOMPARE(wheel->getSlices()[2].weight, quint32(123456));
            table->item(2, 1)->setText("90.0000");
            dialogs(w, {confirmMessage()});
            save->click();
            QCOMPARE(w.configuration().prizes[2].weight, quint32(123456));
            QCOMPARE(wheel->getSlices()[2].weight, quint32(123456));
        }
        MainWindow reopened(db);
        QCOMPARE(reopened.configuration().prizes[2].weight, quint32(123456));
        QCOMPARE(reopened.findChild<WheelWidget *>("wheelCanvas")->getSlices()[2].label, QString("C"));
    }
    void iconAndButtonFeedback() {
        QTemporaryDir dir;
        {
            lw::Store s;
            QString e;
            QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
            lw::Config c;
            c.prizes = {{"a", "A", 100000, -1, -1, QColor("#abcdef")}};
            QVERIFY(s.save(c, &e));
            QVERIFY(s.draw(c, lw::sample(c, 900000), "Miss", nullptr, &e));
        }
        MainWindow w(dir.filePath("data.sqlite"));
        w.show();
        QVERIFY(!w.windowIcon().isNull());
        w.raise();
        w.activateWindow();
        QVERIFY(QTest::qWaitForWindowExposed(&w));
        // Earlier fullscreen/large-window tests can leave a new window partly outside the screen.
        // QWidget mouseMove uses QCursor::setPos in Qt 6.12: capture only after a real hit.
        w.move(w.screen()->availableGeometry().topLeft() + QPoint(8, 8));
        QTest::qWait(150);
        qInfo() << "hover screen/window" << w.screen()->geometry() << w.geometry();
        for (int size : {16, 24, 32, 48, 64, 128, 256})
            QVERIFY(!w.windowIcon().pixmap(size, size).isNull());
        auto *button = w.findChild<QPushButton *>("spinButton");
        auto *tabs = w.findChild<QStackedWidget *>("pages");
        for (int theme = 0; theme < 2; ++theme) {
            w.findChild<QComboBox *>("themeBox")->setCurrentIndex(theme);
            for (int page = 0; page < tabs->count(); ++page) {
                tabs->setCurrentIndex(page);
                for (auto *b : tabs->widget(page)->findChildren<QPushButton *>()) {
                    if (!b->isVisible()) continue;
                    b->clearFocus();
                    QTest::mouseMove(&w, QPoint(1, 1));
                    QTest::qWait(100);
                    if (QGuiApplication::platformName() == "windows") QTRY_VERIFY(!b->underMouse());
                    QImage normal = b->grab().toImage();
                    const bool normalUnderMouse = b->underMouse();
                    QTest::mouseMove(b, b->rect().center());
                    QTest::qWait(100);
                    if (QGuiApplication::platformName() == "windows") {
                        qInfo() << "native hit before assert" << b->objectName() << "visible/active" << b->isVisible() << w.isActiveWindow()
                                << "cursor/target" << QCursor::pos() << b->mapToGlobal(b->rect().center())
                                << "widgetAt/underMouse" << QApplication::widgetAt(QCursor::pos()) << b->underMouse()
                                << "mouseGrabber" << QWidget::mouseGrabber();
#ifdef Q_OS_WIN
                        POINT point; GetCursorPos(&point);
                        DWORD foregroundPid = 0, hitPid = 0;
                        GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPid);
                        GetWindowThreadProcessId(WindowFromPoint(point), &hitPid);
                        qInfo() << "Win32 foreground/hit/self PID" << foregroundPid << hitPid << GetCurrentProcessId()
                                << "Win32 cursor" << point.x << point.y << "modal" << QApplication::activeModalWidget();
#endif
                        QTRY_VERIFY2(QApplication::widgetAt(QCursor::pos()) == b, "Native cursor did not hit target button");
                        QTRY_VERIFY(b->underMouse());
                    }
                    QImage hover = b->grab().toImage();
                    qInfo() << "hover diagnostic" << b->objectName() << "theme/page" << theme << page
                            << "active" << w.isActiveWindow() << "normal/hover underMouse" << normalUnderMouse << b->underMouse()
                            << "cursor/target" << QCursor::pos() << b->mapToGlobal(b->rect().center())
                            << "widgetAt" << QApplication::widgetAt(QCursor::pos()) << "image changed" << (normal != hover);
                    QVERIFY2(normal != hover, qPrintable(b->objectName()));
                    b->setDown(true);
                    QVERIFY2(hover != b->grab().toImage(), qPrintable(b->objectName()));
                    b->setDown(false);
                    QTest::mouseMove(&w, QPoint(1, 1));
                    b->setFocus(Qt::TabFocusReason);
                    QTest::qWait(20);
                    QVERIFY2(normal != b->grab().toImage(), qPrintable(b->objectName()));
                    b->setEnabled(false);
                    QVERIFY2(normal != b->grab().toImage(), qPrintable(b->objectName()));
                    b->setEnabled(true);
                }
            }
        }
        tabs->setCurrentIndex(0);
        QString output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
        if (output.isEmpty()) return;
        QDir().mkpath(output);
        for (int theme = 0; theme < 2; ++theme) {
            w.findChild<QComboBox *>("themeBox")->setCurrentIndex(theme);
            button->clearFocus();
            QTest::mouseMove(&w, QPoint(1, 1));
            QTest::qWait(50);
            QImage normal = button->grab().toImage();
            QTest::mouseMove(button, button->rect().center());
            QTest::qWait(50);
            QImage hover = button->grab().toImage();
            QVERIFY(normal != hover);
            QTest::mousePress(button, Qt::LeftButton);
            QImage pressed = button->grab().toImage();
            QVERIFY(hover != pressed);
            QVERIFY(w.grab().save(output + QString("/pressed-%1.png").arg(theme)));
            QTest::mouseRelease(button, Qt::LeftButton, Qt::NoModifier, QPoint(-5, -5));
            button->setFocus();
            QTest::mouseMove(&w, QPoint(1, 1));
            QTest::qWait(50);
            QVERIFY(w.grab().save(output + QString("/focus-%1.png").arg(theme)));
            button->setEnabled(false);
            QImage disabled = button->grab().toImage();
            QVERIFY(disabled != normal);
            QVERIFY(w.grab().save(output + QString("/disabled-%1.png").arg(theme)));
            button->setEnabled(true);
        }
    }
    void lockedAtStartup() {
        QTemporaryDir dir;
        QString e;
        {
            lw::Store s;
            QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
            lw::Config c;
            c.prizes = {{"a", "A", 100000, -1, -1, QColor("#abcdef")}};
            c.lockSalt = "01234567890123456789012345678901";
            c.lockHash = lw::passwordHash(c.lockSalt, "test");
            QVERIFY(s.save(c, &e));
        }
        MainWindow w(dir.filePath("data.sqlite"));
        QVERIFY(w.isLocked());
        QVERIFY(!w.findChild<QTableWidget *>("prizeTable")->isEnabled());
        QVERIFY(w.findChild<QPushButton *>("spinButton")->isEnabled());
        QVERIFY(!w.findChild<QPushButton *>("restore")->isEnabled());
        QVERIFY(!w.findChild<QPushButton *>("clearHistory")->isEnabled());
    }
    void soundResources() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        for (auto name : {"winSound", "missSound"}) {
            auto *sound = w.findChild<QSoundEffect *>(name);
            QVERIFY(sound);
            QTRY_VERIFY_WITH_TIMEOUT(sound->status() == QSoundEffect::Ready, 5000);
        }
    }
    void lastStockAnimationAndRefresh() {
        QTemporaryDir dir;
        QString e;
        {
            lw::Store s;
            QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
            lw::Config c;
            c.showSoldOut = false;
            c.prizes = {{"a", "Last", lw::Scale, 1, 1, QColor("#abcdef")}};
            QVERIFY(s.save(c, &e));
        }
        MainWindow w(dir.filePath("data.sqlite"));
        w.show();
        w.findChild<QPushButton *>("spinButton")->click();
        QTest::qWait(3500);
        QVERIFY(w.isBusy());
        QVERIFY(!w.findChild<QPushButton *>("clearHistory")->isEnabled());
        auto *wheel = w.findChild<WheelWidget *>("wheelCanvas");
        QCOMPARE(wheel->getSlices()[0].prize, 0);
        double landing = std::fmod(360. - wheel->rotation(), 360.);
        if (landing < 0)
            landing += 360.;
        QVERIFY(std::abs(landing - 180.) < 0.1);
        QCOMPARE(w.findChild<QLabel *>("resultLabel")->text(), QString("Last"));
        QTRY_VERIFY_WITH_TIMEOUT(!w.isBusy(), 2500);
        QCOMPARE(w.configuration().prizes[0].remaining, 0);
        QCOMPARE(wheel->getSlices().size(), 1);
        QCOMPARE(wheel->getSlices()[0].prize, -1);
        QCOMPARE(wheel->getSlices()[0].weight, lw::Scale);
    }
    void resetKeepsHistory() {
        QTemporaryDir dir;
        QString e;
        {
            lw::Store s;
            QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
            lw::Config c;
            c.prizes = {{"a", "A", lw::Scale, 1, 1, QColor("#abcdef")}};
            QVERIFY(s.save(c, &e));
            QVERIFY(s.draw(c, lw::sample(c, 0), "Miss", nullptr, &e));
        }
        MainWindow w(dir.filePath("data.sqlite"));
        dialogs(w, {confirmMessage()});
        w.findChild<QPushButton *>("resetStock")->click();
        QCOMPARE(w.configuration().round, 2);
        QCOMPARE(w.configuration().prizes[0].remaining, 1);
        QCOMPARE(w.recordCount(), 1);
    }
    void csvPreviewAndConfigFiles() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        QString csv = dir.filePath("input.csv");
        QFile file(csv);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("name,probability,limit,color\nImported,12.3456,2,#abcdef\n");
        file.close();
        dialogs(w, {selectFile(csv), confirmPreview()});
        w.findChild<QPushButton *>("importCsv")->click();
        QCOMPARE(w.configuration().prizes.size(), 1);
        QCOMPARE(w.configuration().prizes[0].name, QString("Imported"));
        QCOMPARE(w.configuration().prizes[0].weight, quint32(123456));
        QString json = dir.filePath("config.json");
        dialogs(w, {selectFile(json)});
        w.findChild<QPushButton *>("exportConfig")->click();
        QVERIFY(QFile::exists(json));
        w.findChild<QTableWidget *>("prizeTable")->item(0, 0)->setText("Changed");
        w.findChild<QPushButton *>("savePrizes")->click();
        dialogs(w, {selectFile(json), confirmMessage()});
        w.findChild<QPushButton *>("importConfig")->click();
        QCOMPARE(w.configuration().prizes[0].name, QString("Imported"));
    }
    void backupRestoreAndCsvExport() {
        QTemporaryDir dir;
        QString e;
        {
            lw::Store s;
            QVERIFY(s.open(dir.filePath("data.sqlite"), &e));
            lw::Config c;
            c.prizes = {{"a", "=Formula", lw::Scale, 2, 2, QColor("#abcdef")}};
            QVERIFY(s.save(c, &e));
            QVERIFY(s.draw(c, lw::sample(c, 0), "Miss", nullptr, &e));
        }
        MainWindow w(dir.filePath("data.sqlite"));
        QString backup = dir.filePath("backup.sqlite");
        dialogs(w, {selectFile(backup)});
        w.findChild<QPushButton *>("backup")->click();
        QVERIFY(QFile::exists(backup));
        QString csv = dir.filePath("history.csv");
        dialogs(w, {selectFile(csv)});
        w.findChild<QPushButton *>("exportHistory")->click();
        QFile file(csv);
        QVERIFY(file.open(QIODevice::ReadOnly));
        QVERIFY(file.readAll().contains("'=Formula"));
        file.close();
        w.findChild<QTableWidget *>("prizeTable")->item(0, 0)->setText("Changed");
        w.findChild<QPushButton *>("savePrizes")->click();
        dialogs(w, {selectFile(backup), confirmMessage()});
        w.findChild<QPushButton *>("restore")->click();
        QCOMPARE(w.configuration().prizes[0].name, QString("=Formula"));
        QCOMPARE(w.configuration().prizes[0].remaining, 1);
        QCOMPARE(w.recordCount(), 1);
        QVERIFY(!QDir(dir.path()).entryList({"before-restore-*.sqlite"}).isEmpty());
    }
    void fullscreenAndScreenshots() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        w.show();
        auto *button = w.findChild<QPushButton *>("fullscreen");
        button->click();
        QTRY_VERIFY(w.isFullScreen());
        button->click();
        QTRY_VERIFY(!w.isFullScreen());
        QString output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
        if (!output.isEmpty()) {
            QDir().mkpath(output);
            auto *tabs = w.findChild<QStackedWidget *>("pages");
            auto *language = w.findChild<QComboBox *>("languageBox");
            for (int i = 0; i < 4; ++i) {
                tabs->setCurrentIndex(i);
                QTest::qWait(50);
                QVERIFY(w.grab().save(output + QString("/tab-%1.png").arg(i)));
            }
            for (int i = 0; i < 9; ++i) {
                language->setCurrentIndex(i);
                tabs->setCurrentIndex(3);
                QTest::qWait(50);
                QVERIFY(w.grab().save(output + QString("/language-%1.png").arg(i)));
            }
            w.findChild<QComboBox *>("themeBox")->setCurrentIndex(1);
            QTest::qWait(50);
            QVERIFY(w.grab().save(output + "/dark.png"));
        }
    }
    void minimumWindowKeepsActionsVisible() {
        QTemporaryDir dir;
        MainWindow w(dir.filePath("data.sqlite"));
        w.show();
        w.resize(w.minimumSize());
        auto *tabs = w.findChild<QStackedWidget *>("pages");
        auto *language = w.findChild<QComboBox *>("languageBox");
        for (int lang = 0; lang < language->count(); ++lang) {
            language->setCurrentIndex(lang);
            for (int page = 0; page < tabs->count(); ++page) {
                tabs->setCurrentIndex(page);
                QTest::qWait(20);
                for (auto *b : tabs->widget(page)->findChildren<QPushButton *>()) {
                    if (!b->isVisible()) continue;
                    auto *pageScroll = w.findChild<QScrollArea *>("pagesScroll");
                    pageScroll->ensureWidgetVisible(b, 0, 0);
                    QTest::qWait(5);
                    QRect rect(b->mapTo(&w, QPoint()), b->size());
                    QVERIFY2(w.rect().contains(rect), qPrintable(b->objectName()));
                    if (b->width() < b->minimumSizeHint().width())
                        qInfo() << "caption layout" << b->objectName() << b->size() << b->minimumSizeHint()
                                << "panel" << b->parentWidget()->size() << "window" << w.size();
                    QVERIFY2(b->width() >= b->minimumSizeHint().width(),
                             qPrintable(language->currentData().toString() + ": " + b->objectName()));
                }
            }
        }
        const QString output = qEnvironmentVariable("LW_SCREENSHOT_DIR");
        if (!output.isEmpty()) {
            language->setCurrentIndex(0);
            tabs->setCurrentIndex(1);
            QVERIFY(w.grab().save(output + "/minimum-prizes.png"));
            tabs->setCurrentIndex(2);
            QVERIFY(w.grab().save(output + "/minimum-history.png"));
        }
    }
};
QTEST_MAIN(UiTests)
#include "ui_tests.moc"

