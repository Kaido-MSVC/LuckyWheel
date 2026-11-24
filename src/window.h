#pragma once
#include "core.h"
#include <QMainWindow>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSoundEffect>
#include <QWidget>
class QLabel;
class QPushButton;
class QTableWidget;
class QStackedWidget;
class QToolButton;
class QFrame;
class QSettings;
class QCloseEvent;
class QComboBox;
class QCheckBox;
class QSlider;
class QListWidget;
class QVariantAnimation;
class WheelWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double rotation READ rotation WRITE setRotation)
  public:
    explicit WheelWidget(QWidget *parent = nullptr);
    double rotation() const {
        return angle;
    }
    void setRotation(double v) {
        angle = v;
        update();
    }
    void setSlices(const QVector<lw::Slice> &s) {
        view = s;
        face = QPixmap();
        update();
    }
    void setCelebrate(bool v) {
        celebrate = v;
        update();
    }
    void setDark(bool value) {
        dark = value;
        update();
    }
    const QVector<lw::Slice> &getSlices() const {
        return view;
    }

  protected:
    void paintEvent(QPaintEvent *) override;

  private:
    struct SliceLabel {
        double bearing;
        QStringList lines;
        QColor ink;
    };
    void rebuildFace(int diameter, qreal dpr);
    double angle = 0;
    QVector<lw::Slice> view;
    QPixmap face;
    int faceDiameter = 0;
    QFont labelFont;
    QVector<SliceLabel> labels;
    bool celebrate = false;
    bool dark = false;
};
class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    explicit MainWindow(const QString &dataPath, QWidget *parent = nullptr);
    QString startupError;
    const lw::Config &configuration() const {
        return cfg;
    }
    bool isBusy() const {
        return busy;
    }
    bool isLocked() const {
        return locked;
    }
    int recordCount() const;

  private:
    lw::Store store;
    lw::Config cfg;
    bool busy = false, locked = false, filling = false;
    QString pendingName;
    bool pendingWon = false;
    WheelWidget *wheel;
    QPropertyAnimation *animation;
    QSoundEffect *winSound, *missSound;
    QStackedWidget *pages;
    QWidget *sidebar;
    QFrame *navigationCard;
    QToolButton *sidebarToggle;
    QVector<QToolButton *> navigation;
    QSettings *uiSettings;
    bool sidebarExpanded = true;
    QToolButton *moreToggle;
    QWidget *moreOptions;
    QLabel *colorGroup, *fileGroup, *stockGroup;
    QStringList savedDraft;
    QStringList prizeDraft() const;
    bool resolveDraft();
    void updateDraftHint();
    void updateSidebar(bool animate = false);
    void updateSidebarWidth(bool animate = false);
    void navigate(int index);
  protected:
    bool event(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
  private:
    enum class Status { Ready, Saved, Spinning, Won, Miss };
    void showStatus(Status status);
    QTableWidget *prizeTable = nullptr, *historyTable = nullptr;
    QWidget *resultCard;
    QLabel *pageTitle, *brandLabel, *roundLabel, *roundNumber, *resultCaption, *probabilityTitle;
    QListWidget *probabilities;
    QLabel *resultLabel, *rulesLabel, *editHint, *settingsHint, *languageLabel, *themeLabel,
        *volumeLabel, *dataLabel;
    QComboBox *languageBox, *themeBox;
    QCheckBox *showSold, *soundBox;
    QSlider *volumeSlider;
    QPushButton *spinButton, *lockButton, *clearHistoryButton;
    QVariantAnimation *sidebarMotion = nullptr;
    bool followSystem = false;
    Status status = Status::Ready;
    QLabel *historySummary;
    QVector<QWidget *> mutators;
    QVector<QPair<QPushButton *, QString>> buttons;
    QString t(const QString &key) const;
    QPushButton *button(const QString &key, const QString &objectName);
    void translate();
    void fillPrizes();
    void fillHistory();
    void updateView();
    void updateEnabled();
    bool darkTheme() const;
    void applyTheme();
    void error(const QString &message);
    bool confirm(const QString &message);
    bool persist(const lw::Config &next);
    std::optional<lw::Config> readPrizes();
    void draw();
    void applyPrizes();
    void resetStock();
    void importCsv();
    void exportPrizeCsv();
    void formatExamples();
    void chooseColor();
    void recolor(bool defaults);
    void previewDraft();
    void configFile(bool save);
    void exportHistory();
    void clearHistory();
    void backup();
    void restore();
    void simulate();
    void statistics();
    void toggleLock();
    void toggleFullscreen();
    void about();
    void showCounts(const QString &title, const QVector<QPair<QString, qint64>> &counts, qint64 total,
                    const QString &info = {});
};
