#include "preferenceswidget.hpp"
#include "ui_preferenceswidget.h"

#include <QShortcut>

PreferencesWidget::PreferencesWidget(Preferences *preferences, QWidget *parent)
    :QWidget(parent), ui(new Ui::PreferencesWidget), preferences(preferences)
{
    ui->setupUi(this);

    save_shortcut = new QShortcut(QKeySequence::Save, this);
    connect(save_shortcut, &QShortcut::activated, this, &PreferencesWidget::save);

    ui->compiler_path_input->setText(preferences->compiler_path);
}

PreferencesWidget::~PreferencesWidget() {
    delete ui;
}

void PreferencesWidget::save() {
    preferences->compiler_path = ui->compiler_path_input->text();
}