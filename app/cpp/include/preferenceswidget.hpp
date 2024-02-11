#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class Ui_preferences_widget;
QT_END_NAMESPACE

class PreferencesWidget : public QWidget {
    Q_OBJECT;
public:
    PreferencesWidget(QWidget *parent = nullptr);
    ~PreferencesWidget();
private:
    Ui_preferences_widget *ui;
};