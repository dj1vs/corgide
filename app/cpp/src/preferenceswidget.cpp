#include "preferenceswidget.hpp"
#include "ui_preferenceswidget.h"

#include <QShortcut>
#include <QFontDialog>

PreferencesWidget::PreferencesWidget(Preferences *preferences, QWidget *parent)
    :QWidget(parent), ui(new Ui::PreferencesWidget), preferences(preferences)
{
    ui->setupUi(this);

    save_shortcut = new QShortcut(QKeySequence::Save, this);
    connect(save_shortcut, &QShortcut::activated, this, &PreferencesWidget::save);

    ui->compiler_path_input->setText(preferences->compiler_path);
    ui->compiler_args_input->setText(preferences->compiler_args);

    buf_preferences = *preferences;
}

PreferencesWidget::~PreferencesWidget() {
    delete ui;
}

void PreferencesWidget::choose_editor_font() {
    bool ok;
    QFont new_editor_font = QFontDialog::getFont(
        &ok, preferences->editor_font, this
    );

    if (ok) {
        buf_preferences.editor_font = new_editor_font;
    }
}

void PreferencesWidget::save() {
    preferences->compiler_path = ui->compiler_path_input->text();
    preferences->compiler_args = ui->compiler_args_input->text();
    
    preferences->editor_font = buf_preferences.editor_font;
}