#pragma once

#include <unordered_map>
#include <QString>
#include <QShortcut>

enum class ShortcutType {
    FILE_OPEN,
    FILE_SAVE,
    TAB_NEW,
    TAB_CLOSE,
    TAB_NEXT,
    TAB_PREV,
    FOLER_OPEN,
    RUN_COMPILE,
    RUN_EXEC
};


struct Preferences {
    QString compiler_path;
    QString compiler_args;

    QFont editor_font;

    QMap <ShortcutType, QKeySequence> shortcuts;
};