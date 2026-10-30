#pragma once
#include "core.h"
namespace lw {
QColor defaultColor(int row);
Config exampleConfig();
QString prizeCsv(const Config &c);
std::optional<Config> importPrizeCsv(const QString &text, const Config &base, QString *error);
QString formatHelp(bool json);
}
