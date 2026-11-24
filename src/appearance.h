#pragma once
#include <QColor>
#include <QIcon>

class QListWidget;
class QWidget;

namespace appearance {
enum class Icon { Menu, Wheel, Prizes, History, Settings, Draw };
enum ProbabilityRole { Percent = Qt::UserRole, Details, Swatch };

QIcon icon(Icon type, bool dark, bool onAccent = false);
QColor prizeColor(int row);
void configureProbabilityList(QListWidget *list);
void applyTheme(QWidget *window, bool dark);
} // namespace appearance
