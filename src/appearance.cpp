#include "appearance.h"
#include "window.h"
#include <QListWidget>
#include <QPainter>
#include <QPainterPath>
#include <QStyledItemDelegate>
#include <QTextLayout>
#include <array>
#include <cmath>
#include <numbers>

namespace {
QStringList fitLabel(const QString &label, const QFont &font, int width, int maxLines) {
    if (maxLines < 1 || width < 1)
        return {};
    const QFontMetrics metrics(font);
    if (maxLines == 1 || metrics.horizontalAdvance(label) <= width)
        return {metrics.elidedText(label, Qt::ElideRight, width)};
    QTextLayout layout(label, font);
    QTextOption option;
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    layout.setTextOption(option);
    layout.beginLayout();
    auto first = layout.createLine();
    first.setLineWidth(width);
    const int length = first.textLength();
    layout.endLayout();
    return {label.left(length).trimmed(), metrics.elidedText(label.mid(length).trimmed(), Qt::ElideRight, width)};
}

double linearChannel(double channel) {
    return channel <= .04045 ? channel / 12.92 : std::pow((channel + .055) / 1.055, 2.4);
}

QColor contrastingInk(const QColor &surface) {
    const double luminance = .2126 * linearChannel(surface.redF()) + .7152 * linearChannel(surface.greenF()) +
                             .0722 * linearChannel(surface.blueF());
    return luminance > .179 ? QColor(Qt::black) : QColor(Qt::white);
}

class ProbabilityDelegate : public QStyledItemDelegate {
  public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        const bool hasDetails = !index.data(appearance::Details).toString().isEmpty();
        const int line = option.fontMetrics.height();
        return {0, hasDetails ? line * 2 + 11 : line + 13};
    }

    void paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        p->save();
        const QRect area = option.rect.adjusted(0, 0, -1, 0);
        if (option.state & QStyle::State_Selected)
            p->fillRect(area, option.palette.highlight());
        const bool selected = option.state & QStyle::State_Selected;
        const QColor text = option.palette.color(selected ? QPalette::HighlightedText : QPalette::Text);
        const QString percentage = index.data(appearance::Percent).toString();
        const QString details = index.data(appearance::Details).toString();
        QFont numberFont = option.font;
        numberFont.setWeight(QFont::DemiBold);
        const int numberWidth = QFontMetrics(numberFont).horizontalAdvance(percentage);
        const int line = option.fontMetrics.height();
        const int baselineTop = area.top() + (area.height() - (details.isEmpty() ? line : line * 2)) / 2;
        const QRect number(area.right() - 8 - numberWidth, baselineTop, numberWidth + 1, line);
        const QRect name(area.left() + 25, baselineTop, qMax(0, number.left() - area.left() - 38), line);

        p->setRenderHint(QPainter::Antialiasing);
        p->setBrush(index.data(appearance::Swatch).value<QColor>());
        p->setPen(QPen(option.palette.color(QPalette::Mid), 1));
        p->drawEllipse(QPointF(area.left() + 11, baselineTop + line / 2.), 5, 5);
        p->setPen(text);
        p->setFont(option.font);
        p->drawText(name, Qt::AlignLeft | Qt::AlignVCenter,
                    option.fontMetrics.elidedText(index.data().toString(), Qt::ElideRight, name.width()));
        p->setFont(numberFont);
        p->drawText(number, Qt::AlignRight | Qt::AlignVCenter, percentage);
        if (!details.isEmpty()) {
            p->setFont(option.font);
            p->setPen(selected ? text : option.palette.color(QPalette::PlaceholderText));
            const QRect detail(name.left(), name.top() + line, area.right() - name.left() - 8, line);
            p->drawText(detail, Qt::AlignLeft | Qt::AlignVCenter,
                        option.fontMetrics.elidedText(details, Qt::ElideRight, detail.width()));
        }
        p->setPen(option.palette.color(QPalette::Mid));
        p->drawLine(area.bottomLeft(), area.bottomRight());
        p->restore();
    }
};
} // namespace

namespace appearance {
QColor prizeColor(int row) {
    static constexpr std::array palette{
        "#6380da", "#55b4a3", "#e8a45c", "#bd85c3", "#e2858d", "#88a961", "#73aec9", "#ba966d"};
    return QColor(palette[row % palette.size()]).lighter(100 + (row / palette.size() % 3) * 12);
}

QIcon icon(Icon type, bool dark, bool onAccent) {
    QIcon result;
    for (const qreal dpr : {1., 1.25, 1.5, 2., 3.}) {
        constexpr int size = 20;
        QPixmap image(qRound(size * dpr), qRound(size * dpr));
        image.setDevicePixelRatio(dpr);
        image.fill(Qt::transparent);
        QPainter p(&image);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor(onAccent ? "#ffffff" : dark ? "#d4dff0" : "#475a77"), 1.55, Qt::SolidLine,
                      Qt::RoundCap, Qt::RoundJoin));
        switch (type) {
        case Icon::Menu:
            for (int y : {5, 10, 15})
                p.drawLine(3, y, 17, y);
            break;
        case Icon::Wheel:
            p.drawEllipse(QPointF(10, 10), 7, 7);
            p.drawLine(10, 3, 10, 10);
            p.drawLine(10, 10, 16, 13);
            p.drawLine(10, 10, 5, 15);
            break;
        case Icon::Prizes:
            p.drawRoundedRect(QRectF(3.5, 7, 13, 10), 1, 1);
            p.drawLine(3.5, 10, 16.5, 10);
            p.drawLine(10, 7, 10, 17);
            p.drawArc(QRectF(5.5, 3, 4.5, 4), 0, 360 * 16);
            p.drawArc(QRectF(10, 3, 4.5, 4), 0, 360 * 16);
            break;
        case Icon::History:
            p.drawArc(QRectF(3, 3, 14, 14), 145 * 16, -315 * 16);
            p.drawPolyline(QPolygonF{{2.6, 4.5}, {3.3, 8}, {6.7, 7.1}});
            p.drawPolyline(QPolygonF{{10, 6}, {10, 10}, {13, 12}});
            break;
        case Icon::Settings:
            p.drawLine(3, 5, 17, 5);
            p.drawLine(3, 10, 17, 10);
            p.drawLine(3, 15, 17, 15);
            p.setPen(QPen(p.pen().color(), 2.5, Qt::SolidLine, Qt::RoundCap));
            p.drawLine(7, 3.3, 7, 6.7);
            p.drawLine(13, 8.3, 13, 11.7);
            p.drawLine(8, 13.3, 8, 16.7);
            break;
        case Icon::Draw:
            p.drawPolygon(QPolygonF{{7, 4}, {15.5, 10}, {7, 16}});
            break;
        }
        p.end();
        result.addPixmap(image);
    }
    return result;
}

void configureProbabilityList(QListWidget *list) {
    list->setItemDelegate(new ProbabilityDelegate(list));
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    list->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setMinimumHeight(90);
}

void applyTheme(QWidget *window, bool dark) {
    const QString canvas = dark ? "#121821" : "#edf1f7";
    const QString text = dark ? "#edf2fb" : "#17243a";
    const QString surface = dark ? "#1b2533" : "#ffffff";
    const QString line = dark ? "#354456" : "#d7e0ec";
    const QString quiet = dark ? "#242f40" : "#f3f6fb";
    const QString muted = dark ? "#aabbd0" : "#4e607a";
    const QString hover = dark ? "#2b3c54" : "#e8effc";
    const QString pressed = dark ? "#344c6e" : "#dce7fd";
    const QString accent = dark ? "#476bda" : "#345acb";
    const QString strong = dark ? "#a9bfff" : "#274ba9";
    const QString focus = dark ? "#9ab6ff" : "#345acb";
    const QString disabled = dark ? "#8191a7" : "#64748a";
    const QString won = dark ? "#16372f" : "#e7f6ee";
    const QString miss = dark ? "#303948" : "#e8ecf2";
    const QString primaryHover = dark ? "#526fd7" : "#284bb0";
    const QString primaryPressed = dark ? "#3956b4" : "#203c8e";

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(canvas));
    palette.setColor(QPalette::WindowText, QColor(text));
    palette.setColor(QPalette::Base, QColor(surface));
    palette.setColor(QPalette::AlternateBase, QColor(quiet));
    palette.setColor(QPalette::Text, QColor(text));
    palette.setColor(QPalette::Button, QColor(surface));
    palette.setColor(QPalette::ButtonText, QColor(text));
    palette.setColor(QPalette::Mid, QColor(line));
    palette.setColor(QPalette::PlaceholderText, QColor(muted));
    palette.setColor(QPalette::Highlight, QColor(pressed));
    palette.setColor(QPalette::HighlightedText, QColor(text));
    palette.setColor(QPalette::ToolTipBase, QColor(surface));
    palette.setColor(QPalette::ToolTipText, QColor(text));
    for (const auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText})
        palette.setColor(QPalette::Disabled, role, QColor(disabled));

    window->setStyleSheet(QStringLiteral(
        "QMainWindow, QDialog, QScrollArea#pagesScroll, QStackedWidget#pages, QWidget#drawPage { background: %1; color: %2; }"
        "QMessageBox { background: %3; }"
        "QLabel, QCheckBox { color: %2; }"
        "QToolTip { background: %3; color: %2; border: 1px solid %4; padding: 5px; }"
        "QFrame#topBar, QFrame[card=\"true\"] { background: %3; border: 1px solid %4; border-radius: 14px; }"
        "QFrame#resultCard { background: %5; border-radius: 10px; }"
        "QFrame#resultCard[status=\"spinning\"] { background: %7; }"
        "QFrame#resultCard[status=\"won\"] { background: %13; }"
        "QFrame#resultCard[status=\"miss\"] { background: %14; }"
        "QLabel#brand, QLabel#roundLabel, QLabel#resultCaption, QLabel#drawRules, QLabel#dataPath { color: %6; }"
        "QLabel#pageTitle { font-size: 17px; font-weight: 600; }"
        "QLabel#roundNumber { font-size: 15px; font-weight: 700; color: %10; }"
        "QLabel#resultLabel { font-size: 23px; font-weight: 700; }"
        "QLabel#probabilityTitle, QLabel[groupHeading=\"true\"], QLabel#historySummary { font-weight: 600; }"
        "QPushButton, QToolButton { border: 1px solid %4; border-radius: 7px; background: %3;"
        "color: %2; padding: 6px 11px; min-height: 20px; }"
        "QPushButton:hover, QToolButton:hover { background: %7; border-color: %11; }"
        "QPushButton:focus, QToolButton:focus { border-color: %11; }"
        "QPushButton:pressed, QToolButton:pressed { background: %8; border-color: %11; }"
        "QPushButton:disabled, QToolButton:disabled { background: %5; color: %12; border-color: %4; }"
        "QPushButton[primary=\"true\"] { background: %9; color: white; border-color: %9; font-weight: 600; }"
        "QPushButton[primary=\"true\"]:hover { background: %15; border-color: %15; }"
        "QPushButton[primary=\"true\"]:focus { border-color: %2; }"
        "QPushButton[primary=\"true\"]:pressed { background: %16; border-color: %16; }"
        "QPushButton[primary=\"true\"]:disabled { background: %5; color: %12; border-color: %4; }"
        "QPushButton#spinButton { font-size: 16px; border-width: 2px; border-radius: 9px; }"
        "QToolButton#sidebarToggle, QToolButton[navigation=\"true\"] { background: transparent; border-color: transparent; }"
        "QToolButton#sidebarToggle:hover, QToolButton[navigation=\"true\"]:hover { background: %7; }"
        "QToolButton#sidebarToggle:pressed, QToolButton[navigation=\"true\"]:pressed { background: %8; }"
        "QToolButton[navigation=\"true\"]:checked { background: %8; color: %10; }"
        "QToolButton#sidebarToggle:focus, QToolButton[navigation=\"true\"]:focus { border-color: %11; }"
        "QTableWidget, QPlainTextEdit, QLineEdit, QSpinBox, QComboBox { background: %3; color: %2;"
        "border: 1px solid %4; border-radius: 6px; selection-background-color: %8; selection-color: %2; }"
        "QComboBox, QLineEdit, QSpinBox { padding: 5px 8px; min-height: 20px; }"
        "QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QListWidget#probabilitySummary:focus { border-color: %11; }"
        "QComboBox:disabled, QLineEdit:disabled, QTableWidget:disabled { color: %12; }"
        "QListWidget#probabilitySummary { background: %3; color: %2; border: 1px solid transparent;"
        "border-radius: 6px; selection-background-color: %8; selection-color: %2; }"
        "QHeaderView::section { padding: 7px; font-weight: 600; color: %2; background: %5;"
        "border: 0; border-bottom: 1px solid %4; border-right: 1px solid %4; }"
        "QTableWidget { alternate-background-color: %5; gridline-color: %4; }"
        "QTabBar::tab { padding: 8px 16px; background: %5; border: 1px solid %4; }"
        "QTabBar::tab:hover { background: %7; } QTabBar::tab:pressed { background: %8; }"
        "QTabBar::tab:selected { background: %3; border-bottom: 2px solid %9; }"
        "QTabBar:focus, QCheckBox:focus { outline: 1px solid %11; }"
        "QCheckBox { spacing: 7px; } QCheckBox:hover { color: %10; }"
        "QCheckBox:pressed { background: %8; } QCheckBox:disabled { color: %12; }"
        "QScrollBar:vertical { width: 10px; background: %5; }"
        "QScrollBar:horizontal { height: 10px; background: %5; }"
        "QScrollBar::handle { background: %12; border: 2px solid %5; border-radius: 4px; }"
        "QScrollBar::handle:vertical { min-height: 18px; } QScrollBar::handle:horizontal { min-width: 18px; }"
        "QScrollBar::handle:hover { background: %6; }"
        "QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }"
        "QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }"
        "QStatusBar { color: %6; }"
    ).arg(canvas, text, surface, line, quiet, muted, hover, pressed, accent, strong, focus, disabled,
          won, miss, primaryHover, primaryPressed));
    window->setPalette(palette);
}
} // namespace appearance

WheelWidget::WheelWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(180, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setObjectName("wheelCanvas");
}

void WheelWidget::rebuildFace(int diameter, qreal dpr) {
    faceDiameter = diameter;
    const int pixels = qCeil((diameter + 2) * dpr);
    face = QPixmap(pixels, pixels);
    face.setDevicePixelRatio(dpr);
    face.fill(Qt::transparent);
    labels.clear();
    const qreal radius = diameter / 2.;
    const QRectF circle(-radius, -radius, diameter, diameter);
    labelFont = font();
    labelFont.setPixelSize(qBound(11, qRound(diameter * .034), 21));
    labelFont.setWeight(QFont::DemiBold);
    const QFontMetrics metrics(labelFont);

    QPainter p(&face);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(face.deviceIndependentSize().width() / 2, face.deviceIndependentSize().height() / 2);
    p.setPen(Qt::NoPen);
    double start = 0;
    for (const auto &slice : view) {
        const double span = 360. * slice.weight / lw::Scale;
        QPainterPath sector;
        if (slice.weight == lw::Scale) {
            sector.addEllipse(circle);
        } else {
            sector.moveTo(0, 0);
            sector.arcTo(circle, 90 - start, -span);
            sector.closeSubpath();
        }
        p.fillPath(sector, slice.color);
        // 以靠近圓心的較窄邊計算，確保整個文字區域落在扇形內。
        const double innerRoom = radius * .72 * std::tan(qMin(span / 2, 60.) * std::numbers::pi / 180);
        const int maxLines = qMin(2, int((innerRoom - 3) / metrics.height()));
        auto lines = fitLabel(slice.label, labelFont, int(radius * .56), maxLines);
        if (!lines.isEmpty())
            labels.push_back({start + span / 2, std::move(lines), contrastingInk(slice.color)});
        start += span;
    }
    if (view.size() < 2)
        return;
    p.setPen(QPen(QColor(255, 255, 255, 155), 1));
    start = 0;
    for (int i = 0; i < view.size(); ++i) {
        const auto &previous = i ? view[i - 1] : view.back();
        if (radius * 2 * std::numbers::pi * qMin(previous.weight, view[i].weight) / lw::Scale >= 2) {
            const double rad = start * std::numbers::pi / 180;
            p.drawLine(QPointF(), QPointF(radius * std::sin(rad), -radius * std::cos(rad)));
        }
        start += 360. * view[i].weight / lw::Scale;
    }
}

void WheelWidget::paintEvent(QPaintEvent *) {
    const int diameter = qMin(width(), height()) - 64;
    if (diameter <= 0)
        return;
    const qreal dpr = devicePixelRatioF();
    if (face.isNull() || faceDiameter != diameter || !qFuzzyCompare(face.devicePixelRatio(), dpr))
        rebuildFace(diameter, dpr);

    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    const double radius = diameter / 2.;
    p.translate(width() / 2., height() / 2. + 4);
    const QColor rim(dark ? "#465973" : "#c7d2e3");
    const QColor enamel(dark ? "#223045" : "#ffffff");
    const QColor metal(dark ? "#bdcce4" : "#344960");

    QRadialGradient shadow(QPointF(0, 8), radius + 21);
    shadow.setColorAt(radius / (radius + 21), QColor(13, 27, 53, dark ? 125 : 52));
    shadow.setColorAt(1, Qt::transparent);
    p.setPen(Qt::NoPen);
    p.setBrush(shadow);
    p.drawEllipse(QPointF(0, 8), radius + 21, radius + 21);
    p.setPen(QPen(rim, 1));
    p.setBrush(enamel);
    p.drawEllipse(QPointF(), radius + 7, radius + 7);

    p.save();
    p.rotate(angle);
    const auto faceSize = face.deviceIndependentSize();
    p.drawPixmap(QPointF(-faceSize.width() / 2, -faceSize.height() / 2), face);
    p.setFont(labelFont);
    const int lineHeight = p.fontMetrics().height();
    const double labelWidth = radius * .56;
    for (const auto &label : labels) {
        p.save();
        p.rotate(label.bearing);
        p.translate(0, -radius * .64);
        double direction = std::fmod(angle + label.bearing - 90, 360.);
        if (direction < 0)
            direction += 360.;
        p.rotate(direction > 90 && direction <= 270 ? 90 : -90);
        p.setPen(label.ink);
        const double top = -lineHeight * label.lines.size() / 2.;
        for (int i = 0; i < label.lines.size(); ++i)
            p.drawText(QRectF(-labelWidth / 2, top + i * lineHeight, labelWidth, lineHeight),
                       Qt::AlignCenter, label.lines[i]);
        p.restore();
    }
    p.restore();

    p.setPen(QPen(rim, 1));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPointF(), radius + .5, radius + .5);
    const double hub = qBound(13., radius * .12, 33.);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(13, 27, 53, 42));
    p.drawEllipse(QPointF(0, 2), hub + 1, hub + 1);
    p.setPen(QPen(metal, 1.5));
    p.setBrush(enamel);
    p.drawEllipse(QPointF(), hub, hub);
    p.setPen(QPen(rim, 1));
    p.drawEllipse(QPointF(), hub * .73, hub * .73);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(dark ? "#90abff" : "#3d60d0"));
    p.drawEllipse(QPointF(), hub * .23, hub * .23);

    QPainterPath pointer;
    pointer.moveTo(-12, -radius - 18);
    pointer.quadTo(-15, -radius - 18, -14, -radius - 14);
    pointer.lineTo(-2, -radius + 9);
    pointer.quadTo(0, -radius + 12, 2, -radius + 9);
    pointer.lineTo(14, -radius - 14);
    pointer.quadTo(15, -radius - 18, 12, -radius - 18);
    pointer.closeSubpath();
    p.setPen(QPen(enamel, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(QColor(dark ? "#9cb4ff" : "#2c4eb7"));
    p.drawPath(pointer);

    if (celebrate) {
        p.setPen(Qt::NoPen);
        for (int i = 0; i < 34; ++i) {
            const double a = i * 2.399963;
            const double distance = diameter * (.42 + .10 * (i % 3));
            p.save();
            p.translate(std::cos(a) * distance, std::sin(a) * distance);
            p.rotate(i * 23);
            p.setBrush(appearance::prizeColor(i));
            p.drawRoundedRect(QRectF(-2, -4, 4, 8), 1, 1);
            p.restore();
        }
    }
}
