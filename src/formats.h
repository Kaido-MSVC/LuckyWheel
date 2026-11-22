#pragma once
#include "core.h"
namespace lw {
QColor defaultColor(int row);
Config exampleConfig();
QString prizeCsv(const Config &c);
[[nodiscard]] std::expected<Config, QString> importPrizeCsv(const QString &text, const Config &base);
QString formatHelp(bool json);
}
