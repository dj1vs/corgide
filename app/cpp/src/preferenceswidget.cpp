#include "preferenceswidget.hpp"
#include "ui_preferenceswidget.h"

PreferencesWidget::PreferencesWidget(QWidget *parent)
    :QWidget(parent), ui(new Ui_preferences_widget)
{
    ui->setupUi(this);
}

PreferencesWidget::~PreferencesWidget() {
    delete ui;
}