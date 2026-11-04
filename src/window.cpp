#include "window.h"
#include "formats.h"
#include <QColorDialog>
#include <QTranslator>
#include <QRegularExpression>
#include <QPlainTextEdit>
#include <QSignalBlocker>
#include "translations.h"
#include "version.h"
#include <QApplication>
#include <QAbstractItemDelegate>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QJsonDocument>
#include <QLabel>
#include <QStatusBar>
#include <QIcon>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSaveFile>
#include <QScrollArea>
#include <QScrollBar>
#include <QScreen>
#include <QShortcut>
#include <QSlider>
#include <QStackedWidget>
#include <QToolButton>
#include <QSettings>
#include <QCloseEvent>
#include <QStyle>
#include <QGridLayout>
#include <QResizeEvent>
#include <QTextLayout>
#include <QTableWidget>
#include <QTimer>
#include <QUuid>
#include <QVBoxLayout>
#include <algorithm>
#include <climits>
#include <cmath>

// Fit translated button captions without reducing the window's supported minimum size.
class ActionPanel : public QWidget {
  public:
    ActionPanel(const QList<QPushButton *> &buttons, int columns) : actions(buttons), maxColumns(columns) {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        grid = new QGridLayout(this);
        grid->setContentsMargins(0, 0, 0, 0);
        place(columns);
    }
    void arrange() {
        const int gap = qMax(0, grid->horizontalSpacing());
        int columns = maxColumns;
        for (; columns > 1; --columns) {
            QVector<int> widths(columns, 0);
            for (int i = 0; i < actions.size(); ++i)
                widths[i % columns] = qMax(widths[i % columns], actions[i]->minimumSizeHint().width());
            int required = (columns - 1) * gap;
            for (int width : widths) required += width;
            if (required <= width()) break;
        }
        if (columns != currentColumns) place(columns);
    }
  protected:
    bool event(QEvent *event) override {
        if (event->type() == QEvent::LayoutRequest) arrange();
        return QWidget::event(event);
    }
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        arrange();
    }
  private:
    QList<QPushButton *> actions;
    QGridLayout *grid;
    int maxColumns, currentColumns = 0;
    void place(int columns) {
        while (auto *item = grid->takeAt(0)) delete item;
        for (int i = 0; i < actions.size(); ++i)
            grid->addWidget(actions[i], i / columns, i % columns);
        currentColumns = columns;
    }
};

// Commit even when Apply is invoked by a shortcut or a programmatic click.
class PrizeTable : public QTableWidget {
  public:
    PrizeTable() : QTableWidget(0, 5) {}
    void finishEditing() {
        if (state() != QAbstractItemView::EditingState) return;
        QWidget *editor = QApplication::focusWidget();
        // A programmatic preview can run while focus is still on a previously closed dialog.
        if (!editor || !viewport()->isAncestorOf(editor)) {
            editor = nullptr;
            for (auto *candidate : viewport()->findChildren<QLineEdit *>())
                if (candidate->isVisible()) { editor = candidate; break; }
        }
        if (!editor) return;
        while (editor->parentWidget() != viewport()) editor = editor->parentWidget();
        commitData(editor);
        closeEditor(editor, QAbstractItemDelegate::NoHint);
    }
};
WheelWidget::WheelWidget(QWidget *p) : QWidget(p) {
    setMinimumSize(280, 280);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setObjectName("wheelCanvas");
}
void WheelWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    double size = qMin(width(), height()) - 48;
    QRectF circle(-size / 2, -size / 2, size, size);
    p.translate(width() / 2., height() / 2.);
    p.save();
    p.rotate(angle);
    double start = 0;
    for (const auto &s : view) {
        double span = 360. * s.weight / lw::Scale;
        p.setBrush(s.color);
        p.setPen(QPen(Qt::white, 1.5));
        QPainterPath slice;
        slice.moveTo(0, 0);
        slice.arcTo(circle, 90 - start, -span);
        slice.closeSubpath();
        p.drawPath(slice);
        if (span >= 8) {
            p.save();
            p.rotate(start + span / 2);
            p.translate(0, -size * .34);
            // Account for both the slice and the current animated rotation.
            double textAngle = std::fmod(angle + start + span / 2 - 90, 360.);
            if (textAngle < 0) textAngle += 360.;
            p.rotate(-90 + ((textAngle > 90 && textAngle <= 270) ? 180 : 0));
            QFont f = font();
            f.setPixelSize(int(qBound(11., size * .035, 18.)));
            f.setBold(true);
            p.setFont(f);
            p.setPen(QColor("#142237"));
            // Two bounded lines on wider slices; narrow slices use one elided line.
            const double textWidth = size * .32;
            const int maxLines = span >= 16 ? 2 : 1;
            QTextLayout layout(s.label, f);
            QTextOption option;
            option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            layout.setTextOption(option);
            layout.beginLayout();
            QStringList lines;
            int offset = 0;
            for (int i = 0; i < maxLines; ++i) {
                auto line = layout.createLine();
                if (!line.isValid()) break;
                line.setLineWidth(textWidth);
                int length = line.textLength();
                lines << (i == maxLines - 1
                    ? p.fontMetrics().elidedText(s.label.mid(offset), Qt::ElideRight, int(textWidth))
                    : s.label.mid(offset, length).trimmed());
                offset += length;
            }
            layout.endLayout();
            const double height = p.fontMetrics().height();
            for (int i = 0; i < lines.size(); ++i)
                p.drawText(QRectF(-textWidth / 2, -height * lines.size() / 2 + i * height,
                                 textWidth, height), Qt::AlignCenter, lines[i]);
            p.restore();
        }
        start += span;
    }
    p.restore();
    p.setPen(QPen(QColor("#304365"), 3));
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(0, 0), 20, 20);
    QPolygonF arrow;
    arrow << QPointF(0, -size / 2 + 17) << QPointF(-13, -size / 2 - 12) << QPointF(13, -size / 2 - 12);
    p.setBrush(QColor("#ef4565"));
    p.setPen(Qt::NoPen);
    p.drawPolygon(arrow);
    if (celebrate) {
        for (int i = 0; i < 34; ++i) {
            double a = i * 2.399963;
            double r = size * (.42 + .1 * (i % 3));
            p.setBrush(QColor::fromHsv((i * 37) % 360, 190, 245));
            p.save();
            p.translate(std::cos(a) * r, std::sin(a) * r);
            p.rotate(i * 23);
            p.drawRect(QRectF(-3, -5, 6, 10));
            p.restore();
        }
    }
}
QString MainWindow::t(const QString &key) const {
    return uiText(key, cfg.language);
}
QPushButton *MainWindow::button(const QString &key, const QString &name) {
    auto *b = new QPushButton(QString(t(key)).replace("&", "&&"));
    b->setObjectName(name);
    buttons.push_back({b, key});
    return b;
}
MainWindow::MainWindow(const QString &dataPath, QWidget *parent) : QMainWindow(parent) {
    setWindowIcon(QIcon(":/icons/luckywheel.ico"));
    const QSize desktop = screen() ? screen()->availableGeometry().size() - QSize(40, 80) : QSize(1160, 830);
    setMinimumSize(QSize(820, 620).boundedTo(desktop));
    resize(QSize(1160, 830).boundedTo(desktop));
    QDir().mkpath(QFileInfo(dataPath).absolutePath());
    if (!store.open(dataPath, &startupError))
        return;
    cfg = store.load(&startupError);
    if (!startupError.isEmpty())
        return;
    if (cfg.prizes.isEmpty()) {
        cfg.prizes = {
            {QUuid::createUuid().toString(QUuid::WithoutBraces), "A", 100000, 3, 3, QColor("#72b8ff")},
            {QUuid::createUuid().toString(QUuid::WithoutBraces), "B", 200000, 5, 5, QColor("#a8dca6")}};
        if (!store.save(cfg, &startupError))
            return;
    }
    locked = !cfg.lockHash.isEmpty();
    auto *central = new QWidget;
    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);
    uiSettings = new QSettings(QFileInfo(dataPath).absolutePath() + "/ui.ini", QSettings::IniFormat, this);
    sidebarExpanded = uiSettings->value("sidebarExpanded", true).toBool();
    sidebar = new QWidget;
    sidebar->setObjectName("sidebar");
    auto *navLayout = new QVBoxLayout(sidebar);
    navLayout->setContentsMargins(2, 2, 2, 2);
    sidebarToggle = new QToolButton;
    sidebarToggle->setObjectName("sidebarToggle");
    sidebarToggle->setMinimumHeight(44);
    navLayout->addWidget(sidebarToggle);
    const QList<QStyle::StandardPixmap> icons{QStyle::SP_MediaPlay, QStyle::SP_FileDialogDetailedView,
                                             QStyle::SP_FileDialogContentsView, QStyle::SP_ComputerIcon};
    for (int i = 0; i < 4; ++i) {
        auto *nav = new QToolButton;
        nav->setObjectName(QString("nav%1").arg(i));
        nav->setIcon(style()->standardIcon(icons[i]));
        nav->setIconSize(QSize(22, 22));
        nav->setMinimumHeight(48);
        nav->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        nav->setCheckable(true);
        navigation << nav;
        navLayout->addWidget(nav);
        connect(nav, &QToolButton::clicked, this, [this, i] { navigate(i); });
    }
    navLayout->addStretch();
    root->addWidget(sidebar);
    pages = new QStackedWidget;
    pages->setObjectName("pages");
    // Keep page content usable when high DPI leaves less than 620 logical pixels
    // of desktop height. Navigation stays visible while the page can scroll.
    pages->setMinimumSize(560, 560);
    auto *pagesScroll = new QScrollArea;
    pagesScroll->setObjectName("pagesScroll");
    pagesScroll->setFrameShape(QFrame::NoFrame);
    pagesScroll->setWidgetResizable(true);
    pagesScroll->setWidget(pages);
    root->addWidget(pagesScroll, 1);
    connect(pages, &QStackedWidget::currentChanged, this, [this, pagesScroll] {
        updateSidebar();
        pagesScroll->verticalScrollBar()->setValue(0);
        pagesScroll->horizontalScrollBar()->setValue(0);
    });
    connect(sidebarToggle, &QToolButton::clicked, this, [this] {
        sidebarExpanded = !sidebarExpanded;
        uiSettings->setValue("sidebarExpanded", sidebarExpanded);
        uiSettings->sync();
        updateSidebar();
    });
    setCentralWidget(central);
    auto *drawPage = new QWidget;
    auto *drawLayout = new QVBoxLayout(drawPage);
    drawLayout->setSpacing(10);
    auto *content = new QHBoxLayout;
    wheel = new WheelWidget;
    content->addWidget(wheel, 3);
    auto *side = new QVBoxLayout;
    resultLabel = new QLabel;
    resultLabel->setTextFormat(Qt::PlainText);
    resultLabel->setObjectName("resultLabel");
    resultLabel->setWordWrap(true);
    resultLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet(
        "font-size:26px;font-weight:700;padding:18px;border-radius:12px;background:#193659;color:white;");
    side->addStretch();
    side->addWidget(resultLabel);
    summaryLabel = new QLabel;
    summaryLabel->setTextFormat(Qt::PlainText);
    summaryLabel->setObjectName("probabilitySummary");
    summaryLabel->setWordWrap(true);
    summaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    summaryLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    auto *summaryScroll = new QScrollArea;
    summaryScroll->setWidgetResizable(true);
    summaryScroll->setFrameShape(QFrame::NoFrame);
    summaryScroll->setWidget(summaryLabel);
    summaryScroll->setMinimumHeight(150);
    summaryScroll->setMaximumHeight(350);
    side->addWidget(summaryScroll);
    showSold = new QCheckBox;
    showSold->setObjectName("showSold");
    side->addWidget(showSold);
    mutators << showSold;
    spinButton = button("spin", "spinButton");
    spinButton->setMinimumHeight(56);
    side->addWidget(spinButton);
    side->addStretch();
    auto *drawControls = new QWidget;
    drawControls->setLayout(side);
    drawControls->setMinimumWidth(230);
    drawControls->setMaximumWidth(310);
    content->addWidget(drawControls, 1);
    drawLayout->addLayout(content, 1);
    rulesLabel = new QLabel;
    rulesLabel->setWordWrap(true);
    drawLayout->addWidget(rulesLabel);
    pages->addWidget(drawPage);
    auto *prizePage = new QWidget;
    auto *prizeLayout = new QVBoxLayout(prizePage);
    prizeLayout->setAlignment(Qt::AlignTop);
    editHint = new QLabel;
    editHint->setWordWrap(true);
    editHint->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    prizeLayout->addWidget(editHint);
    prizeTable = new PrizeTable;
    prizeTable->setObjectName("prizeTable");
    prizeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    prizeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    prizeTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    prizeTable->horizontalHeader()->setMinimumSectionSize(110);
    for (int col = 1; col < 5; ++col)
        prizeTable->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    prizeTable->verticalHeader()->setDefaultSectionSize(32);
    auto resizePrizeTable = [this] {
        prizeTable->setMaximumHeight(qBound(100, 80 + prizeTable->rowCount() * 32, 400));
    };
    connect(prizeTable->model(), &QAbstractItemModel::rowsInserted, this, resizePrizeTable);
    connect(prizeTable->model(), &QAbstractItemModel::rowsRemoved, this, resizePrizeTable);
    prizeTable->setMinimumHeight(90);
    prizeLayout->addWidget(prizeTable, 1);
    mutators << prizeTable;
    moreToggle = new QToolButton;
    moreToggle->setObjectName("moreOptionsToggle");
    moreToggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    moreToggle->setCheckable(true);
    moreToggle->setChecked(uiSettings->value("prizeMoreExpanded", false).toBool());
    moreOptions = new QWidget;
    moreOptions->setObjectName("moreOptions");
    auto *advanced = new QVBoxLayout(moreOptions);
    advanced->setContentsMargins(0, 0, 0, 0);
    stockGroup = new QLabel;
    colorGroup = new QLabel;
    fileGroup = new QLabel;
    for (auto *label : {stockGroup, colorGroup, fileGroup}) {
        label->setWordWrap(true);
        label->setStyleSheet("font-weight:bold;");
    }
    auto *prizeActions = new QGridLayout;
    int actionIndex = 0;
    auto *add = button("add", "addPrize");
    auto *remove = button("remove", "removePrize");
    auto *save = button("save", "savePrizes");
    auto *reload = button("reload", "reloadPrizes");
    auto *reset = button("reset", "resetStock");
    mutators << add << remove << save;
    prizeLayout->addWidget(new ActionPanel({add, remove, save}, 3));
    prizeLayout->addWidget(moreToggle);
    auto *stockActions = new QVBoxLayout;
    stockActions->addWidget(reload);
    stockActions->addWidget(reset);
    mutators << reload << reset;
    auto *color = button("chooseColor", "chooseColor");
    auto *autoColor = button("autoColors", "autoColors");
    auto *defaults = button("defaultColors", "defaultColors");
    auto *preview = button("draftPreview", "draftPreview");
    for (auto *b : {color, autoColor, defaults, preview}) {
        prizeActions->addWidget(b, actionIndex / 2, actionIndex % 2);
        ++actionIndex;
        mutators << b;
    }
    connect(color, &QPushButton::clicked, this, &MainWindow::chooseColor);
    connect(autoColor, &QPushButton::clicked, this, [this] { recolor(false); });
    connect(defaults, &QPushButton::clicked, this, [this] { recolor(true); });
    connect(preview, &QPushButton::clicked, this, &MainWindow::previewDraft);
    advanced->addWidget(colorGroup);
    advanced->addLayout(prizeActions);
    auto *files = new QGridLayout;
    auto *csv = button("import", "importCsv");
    auto *exportCfg = button("exportConfig", "exportConfig");
    auto *importCfg = button("importConfig", "importConfig");
    auto *exportCsvPrizes = button("exportPrizeCsv", "exportPrizeCsv");
    auto *examples = button("formatExamples", "formatExamples");
    files->addWidget(csv, 0, 0);
    files->addWidget(exportCsvPrizes, 1, 0);
    files->addWidget(importCfg, 2, 0);
    files->addWidget(exportCfg, 3, 0);
    files->addWidget(examples, 4, 0);
    connect(exportCsvPrizes, &QPushButton::clicked, this, &MainWindow::exportPrizeCsv);
    connect(examples, &QPushButton::clicked, this, &MainWindow::formatExamples);
    mutators << csv << importCfg;
    advanced->addWidget(fileGroup);
    advanced->addLayout(files);
    advanced->addWidget(stockGroup);
    auto *stockHelp = new QLabel;
    stockHelp->setObjectName("stockRules");
    stockHelp->setWordWrap(true);
    stockHelp->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    advanced->addWidget(stockHelp);
    advanced->addLayout(stockActions);
    auto *advancedScroll = new QScrollArea;
    advancedScroll->setObjectName("moreOptionsScroll");
    advancedScroll->setWidgetResizable(true);
    advancedScroll->setFrameShape(QFrame::NoFrame);
    advancedScroll->setWidget(moreOptions);
    advancedScroll->setVisible(moreToggle->isChecked());
    prizeLayout->addWidget(advancedScroll, 1);
    connect(moreToggle, &QToolButton::toggled, this, [this, advancedScroll](bool expanded) {
        advancedScroll->setVisible(expanded);
        moreToggle->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        uiSettings->setValue("prizeMoreExpanded", expanded);
        uiSettings->sync();
    });
    connect(prizeTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int col) {
        if (col == 4) { prizeTable->setCurrentCell(row, col); chooseColor(); }
    });
    
    pages->addWidget(prizePage);
    auto *historyPage = new QWidget;
    auto *historyLayout = new QVBoxLayout(historyPage);
    historySummary = new QLabel;
    historySummary->setObjectName("historySummary");
    historyLayout->addWidget(historySummary);
    historyTable = new QTableWidget(0, 6);
    historyTable->setObjectName("historyTable");
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->horizontalHeader()->setMinimumSectionSize(90);
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    historyLayout->addWidget(historyTable);
    auto *exportCsv = button("exportCsv", "exportHistory");
    auto *stats = button("statistics", "statistics");
    clearHistoryButton = button("clearHistory", "clearHistory");
    mutators << clearHistoryButton;
    historyLayout->addWidget(new ActionPanel({exportCsv, stats, clearHistoryButton}, 3));
    pages->addWidget(historyPage);
    auto *settingsPage = new QWidget;
    auto *settingsLayout = new QVBoxLayout(settingsPage);
    settingsHint = new QLabel;
    settingsHint->setWordWrap(true);
    settingsLayout->addWidget(settingsHint);
    auto *form = new QFormLayout;
    languageLabel = new QLabel;
    languageBox = new QComboBox;
    languageBox->setObjectName("languageBox");
    QStringList codes{"zh_TW", "zh_CN", "en", "ja", "ko", "es", "fr", "de", "pt"},
        names{"繁體中文", "简体中文", "English", "日本語",   "한국어",
              "Español",  "Français", "Deutsch", "Português"};
    for (int i = 0; i < codes.size(); ++i)
        languageBox->addItem(names[i], codes[i]);
    form->addRow(languageLabel, languageBox);
    themeLabel = new QLabel;
    themeBox = new QComboBox;
    themeBox->setObjectName("themeBox");
    themeBox->addItem("", "light");
    themeBox->addItem("", "dark");
    form->addRow(themeLabel, themeBox);
    soundBox = new QCheckBox;
    soundBox->setObjectName("soundBox");
    form->addRow(soundBox);
    volumeLabel = new QLabel;
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setObjectName("volumeSlider");
    volumeSlider->setRange(0, 100);
    form->addRow(volumeLabel, volumeSlider);
    settingsLayout->addLayout(form);
    mutators << languageBox << themeBox << soundBox << volumeSlider;
    auto *full = button("fullscreen", "fullscreen");
    auto *backupBtn = button("backup", "backup");
    auto *restoreBtn = button("restore", "restore");
    auto *sim = button("simulateShort", "simulate");
    lockButton = button("lock", "lockSettings");
    auto *aboutBtn = button("about", "about");
    settingsLayout->addWidget(new ActionPanel({full, backupBtn, restoreBtn, sim, lockButton, aboutBtn}, 2));
    connect(aboutBtn, &QPushButton::clicked, this, &MainWindow::about);
    mutators << restoreBtn;
    dataLabel = new QLabel;
    dataLabel->setTextFormat(Qt::PlainText);
    dataLabel->setWordWrap(true);
    dataLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    settingsLayout->addWidget(dataLabel);
    settingsLayout->addStretch();
    pages->addWidget(settingsPage);
    animation = new QPropertyAnimation(wheel, "rotation", this);
    animation->setDuration(3200);
    animation->setEasingCurve(QEasingCurve::OutQuart);
    winSound = new QSoundEffect(this);
    missSound = new QSoundEffect(this);
    winSound->setObjectName("winSound");
    missSound->setObjectName("missSound");
    winSound->setSource(QUrl("qrc:/sounds/win.wav"));
    missSound->setSource(QUrl("qrc:/sounds/miss.wav"));
    connect(winSound, &QSoundEffect::statusChanged, this, [this] {
        if (winSound->status() == QSoundEffect::Error)
            settingsHint->setText(t("audioUnavailable"));
    });
    connect(missSound, &QSoundEffect::statusChanged, this, [this] {
        if (missSound->status() == QSoundEffect::Error)
            settingsHint->setText(t("audioUnavailable"));
    });
    connect(spinButton, &QPushButton::clicked, this, &MainWindow::draw);
    connect(animation, &QPropertyAnimation::finished, this, [this] {
        resultLabel->setText(pendingName);
        wheel->setCelebrate(pendingWon);
        fillPrizes();
        fillHistory();
        if (cfg.sound) {
            auto *audio = pendingWon ? winSound : missSound;
            audio->setVolume(cfg.volume / 100.);
            audio->play();
        }
        QTimer::singleShot(1600, this, [this] {
            wheel->setCelebrate(false);
            wheel->setRotation(0);
            busy = false;
            updateView();
            updateEnabled();
            hasLastResult = true;
            resultLabel->setText(t("lastResult") + "\n" + pendingName);
        });
    });
    connect(add, &QPushButton::clicked, this, [this] {
        if (busy || locked)
            return;
        int row = prizeTable->rowCount();
        if (row >= 200) {
            error("prize_count");
            return;
        }
        prizeTable->insertRow(row);
        QStringList values{t("name") + QString::number(row + 1), "0.0000", "-1", "-1",
                           lw::defaultColor(row).name()};
        for (int j = 0; j < 5; ++j)
            prizeTable->setItem(row, j, new QTableWidgetItem(values[j]));
        prizeTable->item(row, 0)->setData(Qt::UserRole, QUuid::createUuid().toString(QUuid::WithoutBraces));
        prizeTable->setCurrentCell(row, 1);
        statusBar()->showMessage(t("zeroInfo"), 10000);
    });
    connect(remove, &QPushButton::clicked, this, [this] {
        if (busy || locked)
            return;
        auto selection = prizeTable->selectionModel()->selectedRows();
        QList<int> rows;
        for (auto index : selection)
            rows << index.row();
        std::sort(rows.begin(), rows.end(), std::greater<int>());
        for (int row : rows)
            prizeTable->removeRow(row);
        if (!rows.isEmpty()) statusBar()->showMessage(t("unsaved"), 6000);
    });
    connect(prizeTable, &QTableWidget::itemChanged, this, [this] { if (!filling) updateDraftHint(); });
    connect(prizeTable->model(), &QAbstractItemModel::rowsInserted, this, [this] { if (!filling) updateDraftHint(); });
    connect(prizeTable->model(), &QAbstractItemModel::rowsRemoved, this, [this] { if (!filling) updateDraftHint(); });
    connect(save, &QPushButton::clicked, this, &MainWindow::applyPrizes);
    connect(reload, &QPushButton::clicked, this, [this] {
        if (busy || locked)
            return;
        if (!resolveDraft()) return;
        QString e;
        auto c = store.load(&e);
        if (!e.isEmpty()) {
            error(e);
            return;
        }
        cfg = c;
        translate();
        fillPrizes();
        fillHistory();
        applyTheme();
        updateView();
        statusBar()->showMessage(t("reload") + " · " + t("saved"), 6000);
    });
    connect(reset, &QPushButton::clicked, this, &MainWindow::resetStock);
    connect(csv, &QPushButton::clicked, this, &MainWindow::importCsv);
    connect(exportCfg, &QPushButton::clicked, this, [this] { configFile(true); });
    connect(importCfg, &QPushButton::clicked, this, [this] { configFile(false); });
    connect(exportCsv, &QPushButton::clicked, this, &MainWindow::exportHistory);
    connect(stats, &QPushButton::clicked, this, &MainWindow::statistics);
    connect(clearHistoryButton, &QPushButton::clicked, this, &MainWindow::clearHistory);
    connect(full, &QPushButton::clicked, this, &MainWindow::toggleFullscreen);
    connect(backupBtn, &QPushButton::clicked, this, &MainWindow::backup);
    connect(restoreBtn, &QPushButton::clicked, this, &MainWindow::restore);
    connect(sim, &QPushButton::clicked, this, &MainWindow::simulate);
    connect(lockButton, &QPushButton::clicked, this, &MainWindow::toggleLock);
    connect(showSold, &QCheckBox::toggled, this, [this](bool value) {
        if (filling || busy || locked)
            return;
        auto c = cfg;
        c.showSoldOut = value;
        if (persist(c))
            updateView();
        else
            translate();
    });
    connect(languageBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (filling || busy || locked)
            return;
        auto c = cfg;
        c.language = languageBox->itemData(index).toString();
        if (persist(c)) {
            translate();
            fillHistory();
            updateView();
        } else
            translate();
    });
    connect(themeBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (filling || busy || locked)
            return;
        auto c = cfg;
        c.theme = themeBox->itemData(index).toString();
        if (persist(c))
            applyTheme();
        else
            translate();
    });
    connect(soundBox, &QCheckBox::toggled, this, [this](bool value) {
        if (filling || busy || locked)
            return;
        auto c = cfg;
        c.sound = value;
        if (!persist(c))
            translate();
    });
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        if (filling || busy || locked)
            return;
        auto c = cfg;
        c.volume = value;
        if (!persist(c))
            translate();
    });
    auto *shortcut = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::toggleFullscreen);
    auto *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escape, &QShortcut::activated, this, [this] {
        if (isFullScreen())
            showNormal();
    });
    translate();
    fillPrizes();
    fillHistory();
    updateView();
    applyTheme();
    updateEnabled();
    resultLabel->setText(t("ready"));
}
void MainWindow::translate() {
    filling = true;
    static QTranslator qtTranslator;
    qApp->removeTranslator(&qtTranslator);
    QString qtLanguage = cfg.language == "pt" ? "pt_BR" : cfg.language;
    if (qtTranslator.load(":/translations/qtbase_" + qtLanguage + ".qm"))
        qApp->installTranslator(&qtTranslator);
    moreToggle->setText(t("moreOptions"));
    moreToggle->setAccessibleName(t("moreOptions"));
    moreToggle->setArrowType(moreToggle->isChecked() ? Qt::DownArrow : Qt::RightArrow);
    colorGroup->setText(t("colorGroup"));
    fileGroup->setText(t("fileGroup"));
    stockGroup->setText(t("stockGroup"));
    moreOptions->findChild<QLabel *>("stockRules")->setText(t("stockRules"));
    statusBar()->clearMessage();
    if (!busy) resultLabel->setText(hasLastResult
        ? t("lastResult") + "\n" + (pendingWon ? pendingName : t("miss")) : t(resultKey));
    setWindowTitle(t("title") + " " + LW_VERSION + "[*]");
    setWindowModified(prizeDraft() != savedDraft);
    for (auto pair : buttons)
        pair.first->setText(QString(t(pair.second)).replace("&", "&&"));
    updateSidebar();
    prizeTable->setHorizontalHeaderLabels({t("name"), t("prob"), t("limit"), t("remaining"), t("color")});
    historyTable->setHorizontalHeaderLabels(
        {t("sequence"), t("time"), t("round"), t("result"), t("prob"), t("ticket")});
    rulesLabel->setText(t("rules"));
    prizeTable->setToolTip(t("stockRules") + "\n" + t("colorHint"));
    for (auto pair : buttons) pair.first->setToolTip(t(pair.second));
    clearHistoryButton->setToolTip(t("clearBackupHint"));
    findChild<QPushButton *>("simulate")->setToolTip(t("simulate") + "\n" + t("simulationInfo"));
    updateDraftHint();
    showSold->setText(t("showSoldShort"));
    showSold->setToolTip(t("showSold"));
    showSold->setChecked(cfg.showSoldOut);
    languageLabel->setText(t("language"));
    languageBox->setCurrentIndex(languageBox->findData(cfg.language));
    themeLabel->setText(t("theme"));
    themeBox->setItemText(0, t("light"));
    themeBox->setItemText(1, t("dark"));
    themeBox->setCurrentIndex(themeBox->findData(cfg.theme));
    soundBox->setText(t("sound"));
    soundBox->setChecked(cfg.sound);
    volumeLabel->setText(t("volume"));
    volumeSlider->setValue(cfg.volume);
    dataLabel->setText(t("data") + ": " + store.path());
    settingsHint->setText(locked ? t("lockedInfo") : t("rules"));
    lockButton->setText(t(locked ? "unlock" : "lock"));
    for (auto *widget : findChildren<QWidget *>())
        if (auto *panel = dynamic_cast<ActionPanel *>(widget)) panel->arrange();
    filling = false;
}
void MainWindow::fillPrizes() {
    const bool wasFilling = filling;
    filling = true;
    prizeTable->setRowCount(cfg.prizes.size());
    for (int i = 0; i < cfg.prizes.size(); ++i) {
        const auto &p = cfg.prizes[i];
        QStringList values{p.name, lw::percent(p.weight), QString::number(p.limit),
                           QString::number(p.remaining), p.color.name()};
        for (int j = 0; j < 5; ++j)
            prizeTable->setItem(i, j, new QTableWidgetItem(values[j]));
        prizeTable->item(i, 0)->setData(Qt::UserRole, p.id);
        prizeTable->item(i, 0)->setToolTip(p.name);
    }
    savedDraft = prizeDraft();
    filling = wasFilling;
    updateDraftHint();
}
void MainWindow::fillHistory() {
    QString e;
    auto rows = store.history(&e);
    if (!e.isEmpty()) {
        error(e);
        return;
    }
    historyTable->setRowCount(rows.size());
    qint64 won = 0;
    for (const auto &r : rows) if (!r.prizeId.isEmpty()) ++won;
    historySummary->setText(t("historyTotals").arg(rows.size()).arg(won).arg(rows.size() - won));
    clearHistoryButton->setEnabled(!busy && !locked && !rows.isEmpty());
    for (int i = 0; i < rows.size(); ++i) {
        const auto &r = rows[i];
        QStringList values{QString::number(r.sequence),
                           QDateTime::fromString(r.time, Qt::ISODateWithMs)
                               .toLocalTime()
                               .toString("yyyy-MM-dd HH:mm:ss.zzz"),
                           QString::number(r.round),
                           r.prizeId.isEmpty() ? t("miss") : r.name,
                           lw::percent(r.weight),
                           QString::number(r.ticket)};
        for (int j = 0; j < 6; ++j)
            historyTable->setItem(i, j, new QTableWidgetItem(values[j]));
    }
}
int MainWindow::recordCount() const {
    return historyTable ? historyTable->rowCount() : 0;
}
void MainWindow::updateView() {
    wheel->setSlices(lw::slices(cfg, t("miss"), t("sold")));
    QString summary = t("round") + ": " + QString::number(cfg.round) + "\n";
    for (const auto &p : cfg.prizes)
        summary +=
            p.name + ": " + lw::percent(p.active() ? p.weight : 0) + "%" +
            (p.limit >= 0 ? " · " + QString::number(p.remaining) + "/" + QString::number(p.limit) : "") +
            (!p.active() ? " · " + t("sold") : p.weight == 0 ? " · " + t("zero") : "") + "\n";
    summary += t("miss") + ": " + lw::percent(lw::missWeight(cfg)) + "%";
    summaryLabel->setText(summary);
}
void MainWindow::updateEnabled() {
    spinButton->setEnabled(!busy);
    for (auto *w : mutators)
        w->setEnabled(!busy && !locked);
    clearHistoryButton->setEnabled(!busy && !locked && recordCount() > 0);
    lockButton->setEnabled(!busy);
    for (auto pair : buttons)
        if (pair.second == "exportConfig" || pair.second == "exportCsv" || pair.second == "backup" ||
            pair.second == "simulateShort")
            pair.first->setEnabled(!busy);
    lockButton->setText(t(locked ? "unlock" : "lock"));
    settingsHint->setText(locked ? t("lockedInfo") : t("rules"));
}
void MainWindow::applyTheme() {
    QString bg = cfg.theme == "dark" ? "#172033" : "#f1f5fb",
            fg = cfg.theme == "dark" ? "#eef4ff" : "#182840",
            panel = cfg.theme == "dark" ? "#25334b" : "#ffffff";
    const bool dark = cfg.theme == "dark";
    const QString hover = dark ? "#344e72" : "#e1edff";
    const QString pressed = dark ? "#16385f" : "#bdd6fa";
    const QString disabled = dark ? "#202a3a" : "#e3e8ef";
    setStyleSheet(QString(
        "QMainWindow,QWidget{background:%1;color:%2;font-family:'Segoe UI','Microsoft JhengHei';font-size:11pt;}"
        "QPushButton,QToolButton{padding:9px;border:2px solid #7b91b3;border-radius:7px;background:%3;}"
        "QPushButton:hover,QToolButton:hover{background:%4;border-color:#397dd0;}"
        "QPushButton:focus,QToolButton:focus{border:2px solid #ed9b19;}"
        "QPushButton:pressed,QToolButton:pressed{background:%5;border-style:inset;padding-top:11px;padding-bottom:7px;}"
        "QPushButton:disabled,QToolButton:disabled{background:%6;color:#8692a6;border-color:#99a1ae;}"
        "QPushButton#spinButton{background:#176fe5;color:white;font-size:20px;font-weight:bold;border-radius:10px;}"
        "QPushButton#spinButton:hover{background:#095cc8;border-color:#76b4ff;}"
        "QPushButton#spinButton:focus{border-color:#ed9b19;}"
        "QPushButton#spinButton:pressed{background:#08418d;border-style:inset;}"
        "QPushButton#spinButton:disabled{background:%6;color:#8692a6;border-color:#99a1ae;}"
        "QToolButton:checked{background:%4;border-color:#397dd0;}"
        "QTableWidget,QComboBox{background:%3;selection-background-color:#397dd0;}"
        "QHeaderView::section{padding:8px;background:%3;border:1px solid #7b91b3;}"
        "QTabBar::tab{padding:10px 20px;border:2px solid transparent;}"
        "QTabBar::tab:hover{background:%4;}QTabBar::tab:pressed{background:%5;}"
        "QTabBar::tab:selected{background:%3;border-bottom:3px solid #397dd0;}"
        "QTabBar:focus{border:2px solid #ed9b19;}QTabBar::tab:disabled{color:#8692a6;}"
        "QCheckBox:hover{color:#397dd0;}QCheckBox:focus{outline:2px solid #ed9b19;}"
        "QCheckBox:pressed{background:%5;}QCheckBox:disabled{color:#8692a6;}"
    ).arg(bg, fg, panel, hover, pressed, disabled));
    for (auto *widget : findChildren<QWidget *>())
        if (auto *actions = dynamic_cast<ActionPanel *>(widget)) actions->arrange();
}
void MainWindow::error(const QString &e) {
    QString message = t(e);
    if (message == e) {
        const QStringList keys{"invalid_probability", "invalid_quantity", "invalid_color", "invalid_name",
                               "invalid_csv", "probability_over_100", "prize_count", "invalid_settings",
                               "invalid_language", "invalid_theme", "invalid_lock", "duplicate_id"};
        for (const auto &key : keys) {
            if (e.contains(key)) { message = t(key); break; }
        }
    }
    if (message == e) {
        if (e.startsWith("invalid_") || e == "duplicate_id")
            message = t("genericInvalid") + "\n" + e;
        else
            message = t("error") + "\n" + e;
    }
    QMessageBox box(QMessageBox::Warning, t("error"), message, QMessageBox::NoButton, this);
    statusBar()->showMessage(t("error"), 10000);
    box.setObjectName("operationErrorDialog");
    box.setTextFormat(Qt::PlainText);
    if (message != e) box.setDetailedText(e);
    box.addButton(t("confirm"), QMessageBox::AcceptRole);
    box.exec();
}
bool MainWindow::confirm(const QString &message) {
    QMessageBox box(QMessageBox::Question, t("confirm"), message, QMessageBox::NoButton, this);
    box.setTextFormat(Qt::PlainText);
    auto *yes = box.addButton(t("confirm"), QMessageBox::AcceptRole);
    auto *no = box.addButton(t("cancel"), QMessageBox::RejectRole);
    box.setDefaultButton(no);
    box.exec();
    return box.clickedButton() == yes;
}
bool MainWindow::persist(const lw::Config &next) {
    QString e;
    if (!store.save(next, &e)) {
        error(t("saveFailed") + "\n" + t(e) + "\n" + store.path());
        return false;
    }
    cfg = next;
    statusBar()->showMessage(t("saved"), 4000);
    return true;
}
std::optional<lw::Config> MainWindow::readPrizes() {
    auto c = cfg;
    c.prizes.clear();
    for (int i = 0; i < prizeTable->rowCount(); ++i) {
        auto cell = [&](int col) {
            auto *item = prizeTable->item(i, col);
            return item ? item->text().trimmed() : QString();
        };
        auto weight = lw::parsePercent(cell(1));
        if (!weight) {
            prizeTable->setCurrentCell(i, 1);
            error(t("rowError").arg(i + 1) + "\n" + t("invalid_probability"));
            return std::nullopt;
        }
        bool ok1, ok2;
        int limit = cell(2).toInt(&ok1), remain = cell(3).toInt(&ok2);
        if (!ok1 || !ok2) {
            prizeTable->setCurrentCell(i, !ok1 ? 2 : 3);
            error(t("rowError").arg(i + 1) + "\n" + t("invalid_quantity"));
            return std::nullopt;
        }
        QString id =
            prizeTable->item(i, 0) ? prizeTable->item(i, 0)->data(Qt::UserRole).toString() : QString();
        c.prizes.push_back({id, cell(0), *weight, limit, remain, QColor(cell(4))});
    }
    QString e = lw::validate(c);
    if (!e.isEmpty()) {
        error(e);
        return std::nullopt;
    }
    return c;
}
void MainWindow::applyPrizes() {
    if (busy || locked)
        return;
    static_cast<PrizeTable *>(prizeTable)->finishEditing();
    auto c = readPrizes();
    if (c && persist(*c)) {
        updateView();
        fillPrizes();
        const bool hasZero = std::any_of(cfg.prizes.cbegin(), cfg.prizes.cend(),
                                        [](const lw::Prize &prize) { return prize.weight == 0; });
        const QString zeroHint = hasZero ? "\n" + t("zeroInfo") : QString();
        editHint->setText(t("saveSuccess") + "\n" + t("colorHint") + zeroHint);
        statusBar()->showMessage(t("saveSuccess") + zeroHint + ": " + store.path(), 10000);
        hasLastResult = false; resultKey = "saved";
        resultLabel->setText(t("saved"));
    }
}
void MainWindow::draw() {
    if (busy || !resolveDraft())
        return;
    auto snapshot = cfg;
    auto o = lw::sample(snapshot, lw::randomTicket());
    lw::Record record;
    QString e;
    if (!store.draw(cfg, o, t("miss"), &record, &e)) {
        error(e);
        return;
    }
    busy = true;
    updateEnabled();
    wheel->setCelebrate(false);
    auto view = lw::slices(snapshot, t("miss"), t("sold"));
    wheel->setSlices(view);
    double selected = lw::landingAngle(snapshot, o, view);
    double current = std::fmod(wheel->rotation(), 360.);
    if (current < 0)
        current += 360.;
    wheel->setRotation(current);
    double target = 360. - selected;
    while (target < current + 1800.)
        target += 360.;
    animation->setStartValue(current);
    animation->setEndValue(target);
    pendingName = o.index < 0 ? t("miss") : snapshot.prizes[o.index].name;
    pendingWon = o.index >= 0;
    resultLabel->setText(t("spinning"));
    animation->start();
}
void MainWindow::resetStock() {
    if (busy || locked || !resolveDraft() || !confirm(t("resetConfirm")))
        return;
    auto c = cfg;
    if (c.round == INT_MAX) {
        error("invalid_settings");
        return;
    }
    ++c.round;
    for (auto &p : c.prizes)
        p.remaining = p.limit;
    if (persist(c)) {
        fillPrizes();
        updateView();
        statusBar()->showMessage(t("reset") + " · " + t("saved"), 6000);
    }
}
void MainWindow::importCsv() {
    if (busy || locked)
        return;
    QString path = QFileDialog::getOpenFileName(this, t("import"), {}, "CSV (*.csv)");
    if (path.isEmpty())
        return;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) { error(t("fileFailed") + "\n" + path + "\n" + file.errorString()); return; }
    if (file.size() > 2 * 1024 * 1024) { error("fileTooLarge"); return; }
    QString e;
    auto parsed = lw::importPrizeCsv(QString::fromUtf8(file.readAll()), cfg, &e);
    if (!parsed) { error(e); return; }
    const auto c = *parsed;
    QDialog dialog(this);
    dialog.setWindowTitle(t("preview"));
    dialog.resize(850, 500);
    QVBoxLayout layout(&dialog);
    QLabel label(t("preview"));
    label.setWordWrap(true);
    layout.addWidget(&label);
    QTableWidget table(c.prizes.size(), 5);
    table.setHorizontalHeaderLabels({t("name"), t("prob"), t("limit"), t("remaining"), t("color")});
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table.horizontalHeader()->setMinimumSectionSize(110);
    for (int col = 1; col < 5; ++col)
        table.horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    for (int i = 0; i < c.prizes.size(); ++i) {
        auto p = c.prizes[i];
        QStringList values{p.name, lw::percent(p.weight), QString::number(p.limit), QString::number(p.remaining), p.color.name()};
        for (int j = 0; j < 5; ++j)
            table.setItem(i, j, new QTableWidgetItem(values[j]));
        table.item(i, 4)->setData(Qt::DecorationRole, p.color);
    }
    layout.addWidget(&table);
    QDialogButtonBox actions;
    actions.addButton(t("confirm"), QDialogButtonBox::AcceptRole);
    actions.addButton(t("cancel"), QDialogButtonBox::RejectRole);
    connect(&actions, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&actions, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&actions);
    if (dialog.exec() == QDialog::Accepted && resolveDraft() && persist(c)) {
        fillPrizes();
        prizeTable->setCurrentCell(0, 4);
        updateView();
        statusBar()->showMessage(t("import") + " · " + t("saved"), 6000);
    }
}
void MainWindow::configFile(bool save) {
    if (busy || (!save && locked) || (save && !resolveDraft()))
        return;
    QString path = save ? QFileDialog::getSaveFileName(this, t("exportConfig"), "wheel.json", "JSON (*.json)")
                        : QFileDialog::getOpenFileName(this, t("importConfig"), {}, "JSON (*.json)");
    if (path.isEmpty())
        return;
    if (save) {
        auto c = cfg;
        c.lockHash.clear();
        c.lockSalt.clear();
        QSaveFile file(path);
        QByteArray bytes = QJsonDocument(lw::toJson(c)).toJson();
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit())
            error(t("fileFailed") + "\n" + path + "\n" + file.errorString());
        else
            statusBar()->showMessage(t("exportConfig") + " · " + t("saved") + ": " + path, 8000);
    } else {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) { error(t("fileFailed") + "\n" + path + "\n" + file.errorString()); return; }
        if (file.size() > 2 * 1024 * 1024) { error("fileTooLarge"); return; }
        QJsonParseError pe;
        const QByteArray input = file.readAll();
        auto doc = QJsonDocument::fromJson(input, &pe);
        QString e;
        auto c = pe.error == QJsonParseError::NoError && doc.isObject() ? lw::fromJson(doc.object(), &e)
                                                                        : std::nullopt;
        if (!c) {
            error(e.isEmpty() ? QString("JSON line %1 / byte %2: %3")
                  .arg(input.left(int(pe.offset)).count('\n') + 1).arg(pe.offset)
                  .arg(pe.error == QJsonParseError::NoError ? "root must be object" : pe.errorString()) : e);
            return;
        }
        c->lockHash = cfg.lockHash;
        c->lockSalt = cfg.lockSalt;
        c->round = cfg.round;
        if (confirm(t("savedConfigConfirm")) && resolveDraft() && persist(*c)) {
            translate();
            fillPrizes();
            fillHistory();
            updateView();
            applyTheme();
            statusBar()->showMessage(t("importConfig") + " · " + t("saved"), 6000);
        }
    }
}
void MainWindow::exportHistory() {
    if (busy)
        return;
    QString path = QFileDialog::getSaveFileName(this, t("exportCsv"), "history.csv", "CSV (*.csv)");
    if (path.isEmpty())
        return;
    QString e;
    auto records = store.history(&e);
    if (!e.isEmpty()) {
        error(e);
        return;
    }
    QVector<QStringList> rows{
        {"sequence", "time_utc", "round", "prize_id", "name", "effective_probability_percent", "ticket"}};
    for (const auto &r : records)
        rows.push_back({QString::number(r.sequence), r.time, QString::number(r.round), r.prizeId, r.name,
                        lw::percent(r.weight), QString::number(r.ticket)});
    QSaveFile file(path);
    QByteArray bytes = QByteArray("\xef\xbb\xbf") + lw::encodeCsv(rows, true).toUtf8();
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit())
        error(t("fileFailed") + "\n" + path + "\n" + file.errorString());
    else
        statusBar()->showMessage(t("exportCsv") + " · " + t("saved") + ": " + path, 8000);
}
void MainWindow::backup() {
    if (busy || !resolveDraft())
        return;
    QString path = QFileDialog::getSaveFileName(
        this, t("backup"),
        "LuckyWheel-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss") + ".sqlite",
        "SQLite (*.sqlite)");
    if (path.isEmpty())
        return;
    QString e;
    if (!store.backup(path, &e))
        error(e);
    else {
        hasLastResult = false; resultKey = "saved";
        resultLabel->setText(t("saved"));
        statusBar()->showMessage(t("backup") + " · " + t("saved") + ": " + path, 8000);
    }
}
void MainWindow::restore() {
    if (busy || locked || !resolveDraft())
        return;
    QString path = QFileDialog::getOpenFileName(this, t("restore"), {}, "SQLite (*.sqlite)");
    if (path.isEmpty() || !confirm(t("restoreConfirm")))
        return;
    QString e;
    QString safety = QFileInfo(store.path()).absolutePath() + "/before-restore-" +
                     QUuid::createUuid().toString(QUuid::WithoutBraces) + ".sqlite";
    if (!store.backup(safety, &e)) {
        error(e);
        return;
    }
    if (!store.restore(path, &e)) {
        error(e);
        return;
    }
    cfg = store.load(&e);
    if (!e.isEmpty()) {
        error(e);
        return;
    }
    locked = !cfg.lockHash.isEmpty();
    translate();
    fillPrizes();
    fillHistory();
    updateView();
    updateEnabled();
    applyTheme();
    hasLastResult = false; resultKey = "saved";
    resultLabel->setText(t("saved"));
    statusBar()->showMessage(t("restore") + " · " + t("saved"), 6000);
}
void MainWindow::showCounts(const QString &title, const QVector<QPair<QString, qint64>> &counts, qint64 total,
                            const QString &info) {
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.resize(640, 480);
    QVBoxLayout layout(&dialog);
    QLabel label(t("total") + ": " + QString::number(total) + (info.isEmpty() ? "" : "\n" + info));
    label.setWordWrap(true);
    layout.addWidget(&label);
    QTableWidget table(counts.size(), 3);
    table.setHorizontalHeaderLabels({t("name"), t("count"), t("rate")});
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i = 0; i < counts.size(); ++i) {
        QStringList values{counts[i].first, QString::number(counts[i].second),
                           QString::number(total ? 100. * counts[i].second / total : 0., 'f', 4)};
        for (int j = 0; j < 3; ++j)
            table.setItem(i, j, new QTableWidgetItem(values[j]));
    }
    layout.addWidget(&table);
    QDialogButtonBox actions;
    actions.addButton(t("confirm"), QDialogButtonBox::AcceptRole);
    connect(&actions, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout.addWidget(&actions);
    dialog.exec();
}
void MainWindow::simulate() {
    if (busy)
        return;
    QInputDialog input(this);
    input.setWindowTitle(t("simulate"));
    input.setLabelText(t("count"));
    input.setInputMode(QInputDialog::IntInput);
    input.setIntRange(1, 1000000);
    input.setIntValue(100000);
    input.setOkButtonText(t("confirm"));
    input.setCancelButtonText(t("cancel"));
    if (input.exec() != QDialog::Accepted)
        return;
    int n = input.intValue();
    auto snapshot = cfg;
    auto counts = lw::simulateCounts(snapshot, n);
    // simulateCounts returns an empty vector for an invalid configuration.
    if (counts.size() != snapshot.prizes.size() + 1) {
        error(lw::validate(snapshot));
        return;
    }
    QVector<QPair<QString, qint64>> display{{t("miss"), counts[0]}};
    for (int i = 0; i < snapshot.prizes.size(); ++i)
        display.push_back({snapshot.prizes[i].name, counts[i + 1]});
    showCounts(t("simulate"), display, n, t("simulationInfo"));
}
void MainWindow::clearHistory() {
    if (busy || locked) return;
    QString e;
    auto records = store.history(&e);
    if (!e.isEmpty()) { error(e); return; }
    const qint64 count = records.size();
    if (!count) { fillHistory(); return; }
    if (!confirm(t("clearFirst").arg(count))) return;
    if (!confirm(t("clearFinal").arg(count))) return;
    qint64 deleted = 0;
    if (!store.clearHistory(count, &deleted, &e)) { error(e); fillHistory(); return; }
    fillHistory();
    updateEnabled();
    QMessageBox box(QMessageBox::Information, t("clearHistory"), t("clearDone").arg(deleted),
                    QMessageBox::NoButton, this);
    box.addButton(t("confirm"), QMessageBox::AcceptRole);
    box.exec();
    statusBar()->showMessage(t("clearDone").arg(deleted), 10000);
}
void MainWindow::statistics() {
    QString e;
    auto records = store.history(&e);
    if (!e.isEmpty()) {
        error(e);
        return;
    }
    QMap<QString, QPair<QString, qint64>> counts;
    for (const auto &r : records) {
        QString key = r.prizeId;
        auto &entry = counts[key];
        entry.first = key.isEmpty() ? t("miss") : r.name;
        ++entry.second;
    }
    QVector<QPair<QString, qint64>> display;
    for (auto it = counts.begin(); it != counts.end(); ++it)
        display << it.value();
    showCounts(t("statistics"), display, records.size());
}
void MainWindow::toggleLock() {
    if (busy || !resolveDraft())
        return;
    auto ask = [this](const QString &title, const QString &label) -> std::optional<QString> {
        QInputDialog input(this);
        input.setWindowTitle(title);
        input.setLabelText(label);
        input.setInputMode(QInputDialog::TextInput);
        input.setTextEchoMode(QLineEdit::Password);
        input.setOkButtonText(t("confirm"));
        input.setCancelButtonText(t("cancel"));
        if (input.exec() != QDialog::Accepted)
            return std::nullopt;
        return input.textValue();
    };
    auto password = ask(t(locked ? "unlock" : "lock"), t("password"));
    if (!password)
        return;
    QString pwd = *password;
    if (locked) {
        if (lw::passwordHash(cfg.lockSalt, pwd) != cfg.lockHash) {
            error("wrongPassword");
            return;
        }
        locked = false;
        updateEnabled();
        return;
    }
    if (!cfg.lockHash.isEmpty()) {
        if (lw::passwordHash(cfg.lockSalt, pwd) != cfg.lockHash) {
            error("wrongPassword");
            return;
        }
        locked = true;
        updateEnabled();
        return;
    }
    auto repeat = ask(t("lock"), t("confirmPassword"));
    if (!repeat)
        return;
    if (pwd.size() < 4 || pwd != *repeat) {
        error("wrongPassword");
        return;
    }
    auto c = cfg;
    c.lockSalt = QUuid::createUuid().toString(QUuid::Id128);
    c.lockHash = lw::passwordHash(c.lockSalt, pwd);
    if (persist(c)) {
        locked = true;
        updateEnabled();
    }
}
void MainWindow::toggleFullscreen() {
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::chooseColor() {
    if (busy || locked) return;
    static_cast<PrizeTable *>(prizeTable)->finishEditing();
    const int row = prizeTable->currentRow();
    if (row < 0) { statusBar()->showMessage(t("selectPrize"), 6000); return; }
    auto *item = prizeTable->item(row, 4);
    QColor initial(item->text().trimmed());
    QColorDialog dialog(initial.isValid() ? initial : lw::defaultColor(row), this);
    dialog.setObjectName("prizeColorDialog");
    dialog.setWindowTitle(t("chooseColor"));
    dialog.setOption(QColorDialog::DontUseNativeDialog);
    if (dialog.exec() == QDialog::Accepted && dialog.selectedColor().isValid())
        item->setText(dialog.selectedColor().name());
}
void MainWindow::recolor(bool defaults) {
    if (busy || locked) return;
    static_cast<PrizeTable *>(prizeTable)->finishEditing();
    for (int i = 0; i < prizeTable->rowCount(); ++i)
        prizeTable->item(i, 4)->setText((defaults ? lw::defaultColor(i) : QColor::fromHsv((i * 137 + 25) % 360, 110, 235)).name());
}
void MainWindow::previewDraft() {
    if (busy || locked) return;
    static_cast<PrizeTable *>(prizeTable)->finishEditing();
    auto draft = readPrizes();
    if (!draft) return;
    QDialog dialog(this);
    dialog.setObjectName("draftPreviewDialog");
    dialog.setWindowTitle(t("draftPreview"));
    dialog.resize(520, 560);
    QVBoxLayout layout(&dialog);
    QLabel hint(t("draftPreviewHint")); hint.setWordWrap(true); layout.addWidget(&hint);
    WheelWidget preview;
    preview.setObjectName("draftPreviewWheel");
    preview.setSlices(lw::slices(*draft, t("miss"), t("sold")));
    layout.addWidget(&preview, 1);
    QDialogButtonBox actions;
    actions.addButton(t("cancel"), QDialogButtonBox::RejectRole);
    connect(&actions, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&actions);
    dialog.exec();
}
void MainWindow::exportPrizeCsv() {
    if (busy || !resolveDraft()) return;
    const QString path = QFileDialog::getSaveFileName(this, t("exportPrizeCsv"), "prizes.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QSaveFile file(path);
    const QByteArray bytes = QByteArray("\xef\xbb\xbf") + lw::prizeCsv(cfg).toUtf8();
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) error(t("fileFailed") + "\n" + path + "\n" + file.errorString());
    else statusBar()->showMessage(t("saved") + ": " + path, 6000);
}
void MainWindow::formatExamples() {
    if (busy) return;
    QDialog dialog(this);
    dialog.setObjectName("formatExamplesDialog");
    dialog.setWindowTitle(t("formatExamples"));
    dialog.resize(850, 650);
    QVBoxLayout layout(&dialog);
    QComboBox format;
    format.setObjectName("exampleFormat");
    format.addItems({"CSV", "JSON"}); layout.addWidget(&format);
    QPlainTextEdit text;
    text.setObjectName("exampleContent"); text.setReadOnly(true);
    QPlainTextEdit help;
    help.setObjectName("formatHelp"); help.setReadOnly(true);
    auto refresh = [&] {
        const auto example = lw::exampleConfig();
        text.setPlainText(format.currentIndex() == 0 ? lw::prizeCsv(example) : QString::fromUtf8(QJsonDocument(lw::toJson(example)).toJson()));
        help.setPlainText(t("formatShared") + "\n" + t(format.currentIndex() == 1 ? "formatJson" : "formatCsv"));
    };
    connect(&format, &QComboBox::currentIndexChanged, &dialog, refresh);
    refresh(); layout.addWidget(&help, 1); layout.addWidget(&text, 1);
    QDialogButtonBox actions;
    auto *save = actions.addButton(t("saveExample"), QDialogButtonBox::ActionRole);
    actions.addButton(t("cancel"), QDialogButtonBox::RejectRole);
    connect(save, &QPushButton::clicked, &dialog, [&] {
        const bool json = format.currentIndex() == 1;
        const auto path = QFileDialog::getSaveFileName(&dialog, t("saveExample"), json ? "example.json" : "example.csv", json ? "JSON (*.json)" : "CSV (*.csv)");
        if (path.isEmpty()) return;
        QSaveFile file(path);
        const QByteArray bytes = text.toPlainText().toUtf8();
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) error(t("fileFailed") + "\n" + path + "\n" + file.errorString());
        else statusBar()->showMessage(t("saved") + ": " + path, 6000);
    });
    connect(&actions, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&actions); dialog.exec();
}

void MainWindow::about() {
    QDialog dialog(this);
    dialog.setObjectName("aboutDialog");
    dialog.setWindowTitle(t("about"));
    dialog.resize(760, 620);
    QVBoxLayout layout(&dialog);
    QLabel heading(QString("<b>%1 %2</b><br>Copyright © 2026 kaido-MSVC · MIT License")
                       .arg(t("title").toHtmlEscaped(), LW_VERSION));
    heading.setObjectName("aboutHeading");
    heading.setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout.addWidget(&heading);
    QFile licenseFile(":/legal/LICENSE.txt");
    const QString license = licenseFile.open(QIODevice::ReadOnly) ? QString::fromUtf8(licenseFile.readAll()) : QString();
    QFile translationNoticeFile(":/legal/Qt-Translations-BSD-3-Clause.txt");
    const QString translationNotice = translationNoticeFile.open(QIODevice::ReadOnly)
        ? QString::fromUtf8(translationNoticeFile.readAll()) : QString();
    // Component list mirrors THIRD-PARTY.md; update both when the deployed Qt modules change.
    const QString components = QStringLiteral(
        "• Qt %1 (qtbase, qtmultimedia, qtsvg, qtimageformats)\n"
        "  © The Qt Company Ltd. and contributors — GNU LGPL v3\n"
        "  https://download.qt.io/official_releases/qt/6.12/6.12.0/single/\n"
        "• Qt translations 6.12.0 (nine embedded qtbase_*.qm files) — BSD-3-Clause\n"
        "  Copyright (C) The Qt Company Ltd. and other contributors.\n"
        "  Full copyright, conditions and disclaimer are reproduced below.\n"
        "• FFmpeg 9.0.1 (avcodec, avformat, avutil, swresample, swscale) — GNU LGPL v2.1 or later\n"
        "  https://ffmpeg.org/releases/ffmpeg-9.0.1.tar.gz\n"
        "• zlib, libpng, libjpeg, FreeType, HarfBuzz, PCRE2, SQLite, md4c, libtiff, libwebp, libpsl, Unicode data\n"
        "  (bundled in Qt) — BSD / MIT / Zlib / FTL / Unicode / public domain\n"
        "• Mesa llvmpipe (opengl32sw.dll) — MIT\n"
        "• Microsoft Direct3D / DirectX Shader Compiler — Windows SDK redistributable terms\n"
        "• Microsoft Visual C++ Runtime — Visual Studio redistributable terms").arg(qVersion());
    QPlainTextEdit text;
    text.setObjectName("aboutText");
    text.setReadOnly(true);
    text.setPlainText(t("aboutLicense") + "\n\n" + t("thirdParty") + "\n" + components + "\n\n" + t("licenseText") +
                      "\n" + license + "\n\n" + translationNotice +
                      "\n\nLGPL corresponding source and rebuild information: licenses/SOURCE-DELIVERY.md\n"
                      "Publication and redistribution information: RELEASE-STATUS.md.");
    layout.addWidget(&text, 1);
    QDialogButtonBox actions;
    auto *folder = actions.addButton(t("openLicenses"), QDialogButtonBox::ActionRole);
    folder->setObjectName("openLicenses");
    auto *qt = actions.addButton(t("aboutQt"), QDialogButtonBox::ActionRole);
    actions.addButton(t("close"), QDialogButtonBox::RejectRole);
    connect(folder, &QPushButton::clicked, &dialog, [this] {
        const QString path = QCoreApplication::applicationDirPath() + "/licenses";
        if (!QFileInfo(path).isDir() || !QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
            error(t("licensesMissing") + "\n" + QDir::toNativeSeparators(path));
    });
    connect(qt, &QPushButton::clicked, &dialog, [&dialog] { QMessageBox::aboutQt(&dialog); });
    connect(&actions, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&actions);
    dialog.exec();
}

QStringList MainWindow::prizeDraft() const {
    QStringList draft;
    for (int row = 0; row < prizeTable->rowCount(); ++row) {
        for (int col = 0; col < prizeTable->columnCount(); ++col) {
            auto *item = prizeTable->item(row, col);
            draft << (item ? item->text() : QString());
        }
        auto *item = prizeTable->item(row, 0);
        draft << (item ? item->data(Qt::UserRole).toString() : QString());
    }
    return draft;
}
void MainWindow::updateDraftHint() {
    const bool dirty = prizeDraft() != savedDraft;
    editHint->setText(t(dirty ? "unsaved" : "saved") + "\n" + t("colorHint"));
    QSignalBlocker blocker(prizeTable);
    for (int i = 0; i < prizeTable->rowCount(); ++i) {
        auto *item = prizeTable->item(i, 4);
        if (!item) continue;
        QColor color(item->text().trimmed());
        item->setData(Qt::DecorationRole, color.isValid() ? QVariant(color) : QVariant());
        item->setToolTip(t("colorHint"));
    }
    setWindowTitle(t("title") + " " + LW_VERSION + "[*]");
    setWindowModified(dirty);
}
bool MainWindow::resolveDraft() {
    static_cast<PrizeTable *>(prizeTable)->finishEditing();
    if (prizeDraft() == savedDraft) return true;
    QMessageBox box(QMessageBox::Question, t("unsavedTitle"), t("unsavedPrompt"), QMessageBox::NoButton, this);
    box.setObjectName("unsavedDialog");
    auto *save = box.addButton(t("save"), QMessageBox::AcceptRole);
    auto *discard = box.addButton(t("discard"), QMessageBox::DestructiveRole);
    auto *cancel = box.addButton(t("cancel"), QMessageBox::RejectRole);
    box.setDefaultButton(cancel);
    box.setEscapeButton(cancel);
    box.exec();
    if (box.clickedButton() == discard) { fillPrizes(); return true; }
    if (box.clickedButton() == save) {
        applyPrizes();
        return prizeDraft() == savedDraft;
    }
    return false;
}
void MainWindow::updateSidebar() {
    sidebar->setFixedWidth(sidebarExpanded ? 206 : 56);
    sidebarToggle->setText(sidebarExpanded ? "‹ " + sidebarToggle->fontMetrics().elidedText(t("collapseNav"), Qt::ElideRight, 152) : "☰");
    sidebarToggle->setToolTip(t(sidebarExpanded ? "collapseNav" : "expandNav"));
    sidebarToggle->setAccessibleName(sidebarToggle->toolTip());
    const QStringList keys{"wheel", "prizes", "history", "settings"};
    for (int i = 0; i < navigation.size(); ++i) {
        auto *nav = navigation[i];
        const QString label = t(keys[i]);
        nav->setText(sidebarExpanded ? nav->fontMetrics().elidedText(label, Qt::ElideRight, 146).replace("&", "&&") : QString());
        nav->setToolButtonStyle(sidebarExpanded ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly);
        nav->setToolTip(label);
        nav->setAccessibleName(label);
        nav->setChecked(pages->currentIndex() == i);
    }
}
void MainWindow::navigate(int index) {
    if (index != pages->currentIndex() && !resolveDraft()) { updateSidebar(); return; }
    pages->setCurrentIndex(index);
    updateSidebar();
}
void MainWindow::closeEvent(QCloseEvent *event) {
    if (!startupError.isEmpty() || resolveDraft()) event->accept();
    else event->ignore();
}
