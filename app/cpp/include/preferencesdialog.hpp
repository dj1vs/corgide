#pragma once

#include <QDialog>

#include "ds/preferences.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {class PreferencesDialog;};
class QShortcut;
QT_END_NAMESPACE

class PreferencesDialog : public QDialog {
    Q_OBJECT;
public:
    PreferencesDialog(Preferences *preferences, QDialog *parent = nullptr);
    ~PreferencesDialog();
private slots:
    void choose_editor_font();

    void save();
private:
    Ui::PreferencesDialog *ui;

    QShortcut *save_shortcut;

    Preferences *preferences;
    Preferences buf_preferences;
};