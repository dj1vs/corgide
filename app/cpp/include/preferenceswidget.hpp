#pragma once

#include <QWidget>

#include "ds.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {class PreferencesWidget;};
class QShortcut;
QT_END_NAMESPACE

class PreferencesWidget : public QWidget {
    Q_OBJECT;
public:
    PreferencesWidget(Preferences *preferences, QWidget *parent = nullptr);
    ~PreferencesWidget();
private slots:
    void choose_editor_font();

    void save();
private:
    Ui::PreferencesWidget *ui;

    QShortcut *save_shortcut;

    Preferences *preferences;
    Preferences buf_preferences;
};