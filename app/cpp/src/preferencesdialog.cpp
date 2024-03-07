#include "preferencesdialog.hpp"
#include "ui_preferencesdialog.h"

#include <QShortcut>
#include <QFontDialog>
#include <QMessageBox>

#include "ds.hpp"

PreferencesDialog::PreferencesDialog(Preferences *preferences, QDialog *parent)
    :QDialog(parent), ui(new Ui::PreferencesDialog), preferences(preferences)
{
    ui->setupUi(this);

    save_shortcut = new QShortcut(QKeySequence::Save, this);
    connect(save_shortcut, &QShortcut::activated, this, &PreferencesDialog::save);

    ui->compiler_path_input->setText(preferences->compiler_path);
    ui->compiler_args_input->setText(preferences->compiler_args);

    ui->file_open_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::FILE_OPEN]);
    ui->file_save_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::FILE_SAVE]);

    ui->folder_open_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::FOLER_OPEN]);
    
    ui->run_compile_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::RUN_COMPILE]);
    ui->run_exec_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::RUN_EXEC]);

    ui->tab_close_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::TAB_CLOSE]);
    ui->tab_new_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::TAB_NEW]);
    ui->tab_next_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::TAB_NEXT]);
    ui->tab_prev_seq_edit->setKeySequence(preferences->shortcuts[ShortcutType::TAB_PREV]);

    buf_preferences = *preferences;
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
}

void PreferencesDialog::choose_editor_font() {
    bool ok;
    QFont new_editor_font = QFontDialog::getFont(
        &ok, preferences->editor_font, this
    );

    if (ok) {
        buf_preferences.editor_font = new_editor_font;
    }
}

void PreferencesDialog::save() {
    preferences->compiler_path = ui->compiler_path_input->text();
    preferences->compiler_args = ui->compiler_args_input->text();
    
    preferences->editor_font = buf_preferences.editor_font;

    qDebug() << ui->tab_new_seq_edit->keySequence().toString();

    preferences->shortcuts[ShortcutType::FILE_OPEN] = ui->file_open_seq_edit->keySequence();
    preferences->shortcuts[ShortcutType::FILE_SAVE] = ui->file_save_seq_edit->keySequence();

    preferences->shortcuts[ShortcutType::FOLER_OPEN] = ui->folder_open_seq_edit->keySequence();
    
    preferences->shortcuts[ShortcutType::RUN_COMPILE] = ui->run_compile_seq_edit->keySequence();
    preferences->shortcuts[ShortcutType::RUN_EXEC]    = ui->run_exec_seq_edit->keySequence();

    preferences->shortcuts[ShortcutType::TAB_CLOSE] = ui->tab_close_seq_edit->keySequence();
    preferences->shortcuts[ShortcutType::TAB_NEW] =   ui->tab_new_seq_edit->keySequence();
    preferences->shortcuts[ShortcutType::TAB_NEXT] =  ui->tab_next_seq_edit->keySequence();
    preferences->shortcuts[ShortcutType::TAB_PREV] =  ui->tab_prev_seq_edit->keySequence();

    QMessageBox::information(this, "Preferences saving", "Your changes were saved\nFor changes to take effect, you have to restart application", QMessageBox::StandardButton::Ok);
}