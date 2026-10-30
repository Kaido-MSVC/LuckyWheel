#include "ms_terms.h"
#include <QCryptographicHash>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSaveFile>
#include <QVBoxLayout>

bool acceptMicrosoftTerms(const QString &dataDirectory) {
    QFile terms(":/legal/END-USER-TERMS.txt");
    if (!terms.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "LuckyWheel", "Missing Microsoft component terms.");
        return false;
    }
    const QByteArray text = terms.readAll();
    const QByteArray digest = QCryptographicHash::hash(text, QCryptographicHash::Sha256).toHex();
    const QString receiptPath = dataDirectory + "/microsoft-terms.sha256";
    QFile receipt(receiptPath);
    if (receipt.open(QIODevice::ReadOnly) && receipt.readAll() == digest)
        return true;
    receipt.close();

    QDialog dialog;
    dialog.setObjectName("microsoftTermsDialog");
    dialog.setWindowTitle("Microsoft 元件條款 / Component terms");
    dialog.resize(780, 580);
    QVBoxLayout layout(&dialog);
    QLabel label("請閱讀隨附 Microsoft 元件條款。不同意將結束程式。\n"
                 "Read the Microsoft component terms. Declining closes the application.\n"
                 "Qt / FFmpeg LGPL 權利不受限制 / LGPL rights remain unrestricted.");
    label.setWordWrap(true);
    layout.addWidget(&label);
    QPlainTextEdit body;
    body.setReadOnly(true);
    body.setPlainText(QString::fromUtf8(text));
    layout.addWidget(&body);
    QDialogButtonBox buttons;
    auto *agree = buttons.addButton("同意 / I agree", QDialogButtonBox::AcceptRole);
    agree->setObjectName("agreeMicrosoftTerms");
    agree->setAutoDefault(false);
    auto *decline = buttons.addButton("不同意並結束 / Decline and exit", QDialogButtonBox::RejectRole);
    decline->setDefault(true);
    decline->setFocus();
    layout.addWidget(&buttons);
    QObject::connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    QSaveFile saved(receiptPath);
    if (!saved.open(QIODevice::WriteOnly) || saved.write(digest) != digest.size() || !saved.commit()) {
        QMessageBox::warning(nullptr, "LuckyWheel", "無法保存同意紀錄。\nCannot save acceptance record.");
        return false;
    }
    return true;
}
