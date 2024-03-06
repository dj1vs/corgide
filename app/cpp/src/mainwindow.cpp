#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <algorithm>

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QRegularExpression>
#include <QFileSystemModel>
#include <QMenu>
#include <QModelIndexList>
#include <QSettings>
#include <QInputDialog>
#include <QShortcut>

#include "qsourcehighliter.h"
#include "fs.hpp"

#include "codeeditor.hpp"
#include "preferenceswidget.hpp"

#include "font_settings.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    fs_model = new QFileSystemModel;
    fs_model->setRootPath(QDir::root().absolutePath());

    QShortcut *terminal_focus_shortcut = new QShortcut(QKeySequence("Ctrl+`"), this);
    QShortcut *editor_focus_shortcut = new QShortcut(QKeySequence("Ctrl+1"), this);
    QShortcut *folder_focus_shortcut = new QShortcut(QKeySequence("Ctrl+Shift+E"), this);
    connect(terminal_focus_shortcut, &QShortcut::activated, this, &MainWindow::terminal_focus);
    connect(editor_focus_shortcut, &QShortcut::activated, this, &MainWindow::editor_focus);
    connect(folder_focus_shortcut, &QShortcut::activated, this, &MainWindow::folder_focus);

    setup_folder_context_menu();

    connect(ui->tree_view, &QTreeView::doubleClicked, this, &MainWindow::open_folder_file);

    connect(ui->tab_widget, &QTabWidget::tabCloseRequested, this, &MainWindow::close_tab);

    read_settings();
}

MainWindow::~MainWindow() {
    write_settings();

    delete ui;
}

void MainWindow::open_new_tab() {
    CodeEditor *editor = new CodeEditor(this);
    editor->setFont(preferences.editor_font);

    auto *highlighter = new QSourceHighlite::QSourceHighliter(editor->document());
    highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);

    QRegularExpression untitled_regexp("Untitled (^\\d+$)");
    QVector<int> untitled_numbers;
    const int tabs_amount = ui->tab_widget->count();
    for (int i = 0; i < tabs_amount; ++i) {
        const QString tab_name = ui->tab_widget->tabText(i);
        const auto untitled_regexp_match = untitled_regexp.match(tab_name);
        if (!untitled_regexp_match.isValid()) {
            continue;
        }

        const qsizetype tab_name_begin = tab_name.indexOf('(') + 1;
        const qsizetype tab_name_end = tab_name.indexOf(')') - 1;
        const QString tab_number = tab_name.mid(tab_name_begin, tab_name_end - tab_name_begin + 1);
        untitled_numbers.push_back(tab_number.toInt());
    }

    std::sort(untitled_numbers.begin(), untitled_numbers.end());
    int new_tab_number = -1;
    for (qsizetype i = 1; i < untitled_numbers.size(); ++i) {
        if (untitled_numbers[i] != untitled_numbers[i-1] + 1) {
            new_tab_number = untitled_numbers[i-1] + 1;
            break;
        }
    }

    if (new_tab_number == -1 && untitled_numbers.size()) {
        new_tab_number = untitled_numbers.back() + 1;
    }

    QString new_tab_name = "Untitled";
    if (new_tab_number != -1) {
        new_tab_name += " (" + QString::number(new_tab_number) + ')';
    }

    ui->tab_widget->addTab(editor, new_tab_name);
    ui->tab_widget->setCurrentIndex(ui->tab_widget->count() - 1); // go to new tab
}

void MainWindow::close_tab(int index) {

    if (!ui->tab_widget->count()) {
        return;
    }

    auto cur_editor = get_cur_editor();

    bool file_opened = cur_editor->get_file_name().has_value();
    if (file_opened) {
        bool is_edited = cur_editor->document()->isModified();

        if (is_edited) {
             const auto ret = QMessageBox::question(this, "File opening", "Do you want to save changes?", QMessageBox::No, QMessageBox::Yes);
            if (ret == QMessageBox::Yes) {
                save_file(cur_editor->get_file_name().value());
            }
        }
    }
    
    ui->tab_widget->removeTab(index);
}

void MainWindow::close_current_tab() {
    close_tab(ui->tab_widget->currentIndex());
}

void MainWindow::next_tab() {
    const int cur_ind = ui->tab_widget->currentIndex();
    
    int next_ind = cur_ind + 1;
    if (next_ind == ui->tab_widget->count()) {
        next_ind = 0;
    }

    ui->tab_widget->setCurrentIndex(next_ind);
}

void MainWindow::prev_tab() {
    const int cur_ind = ui->tab_widget->currentIndex();
    
    int next_ind = cur_ind - 1;
    if (next_ind == -1) {
        next_ind = ui->tab_widget->count() - 1;
    }

    ui->tab_widget->setCurrentIndex(next_ind);
}

void MainWindow::ask_open_file() {
    QString file_name_filter = "";
    const QString file_name = QFileDialog::getOpenFileName(this, "Choose file to open", "", "", &file_name_filter);
    if (!file_name.length()) {
        return;
    } 

    open_file(file_name);
}

void MainWindow::ask_save_file() {
    QString file_name;

    auto cur_editor = get_cur_editor();
    if (cur_editor->get_file_name().has_value()) {
        file_name = cur_editor->get_file_name().value();
    } else {
        QString file_name_filter = "";
        file_name = QFileDialog::getSaveFileName(this, "Chose file save path", "", "", &file_name_filter);
        if (!file_name.length()) {
            return;
        }
    }

    save_file(file_name);

    
}

void MainWindow::ask_rename_file() {
    QModelIndexList fs_rows = ui->tree_view->selectionModel()->selectedRows();
    if (fs_rows.size() != 1) {
        return;
    }

    QModelIndex selected_row = fs_rows.front();

    const QString file_path = fs_model->filePath(selected_row);

    if (!QFileInfo(file_path).isFile()) {
        return;
    }

    const QString file_name = QFileInfo(file_path).fileName();

    bool ok;
    const QString new_name = QInputDialog::getText(this, "Pick a new file name", "New name", QLineEdit::Normal, file_name, &ok);

    if (ok && !new_name.isEmpty()) {
        rename_file(file_path, QFileInfo(file_path).absoluteDir().absolutePath() + '/' + new_name);
    }
}

void MainWindow::ask_create_file() {
    bool ok;
    const QString new_file_name = QInputDialog::getText(this, "New file", "New file name", QLineEdit::Normal, QString(), &ok);
    if (ok) {
        const QString opened_folder = get_opened_folder();

        if (opened_folder.isEmpty()) {
            return;
        }

        create_file(opened_folder + "/" + new_file_name);
    }
}

void MainWindow::ask_delete_file() {
    QModelIndexList fs_rows = ui->tree_view->selectionModel()->selectedRows();
    if (fs_rows.size() != 1) {
        return;
    }

    QModelIndex selected_row = fs_rows.front();

    const QString file_path = fs_model->filePath(selected_row);

    if (!QFileInfo(file_path).isFile()) {
        return;
    }

    const QString file_name = QFileInfo(file_path).fileName();

    const auto ret = QMessageBox::critical(this, "Delete file", "Do you really want to delete " + file_name + "?", QMessageBox::No, QMessageBox::Yes);
    if (ret == QMessageBox::Yes) {
        remove_file(file_path);
    }

    
}

void MainWindow::open_folder_file() {
    QModelIndexList fs_rows = ui->tree_view->selectionModel()->selectedRows();
    if (fs_rows.size() != 1) {
        return;
    }

    QModelIndex selected_row = fs_rows.front();

    const QString file_path = fs_model->filePath(selected_row);

    if (QFileInfo(file_path).isFile()) {
        open_file(file_path);
    }
}

void MainWindow::compile() {
    const auto cur_editor = get_cur_editor();

    std::optional<QString> file_name = cur_editor->get_file_name();
    if (!file_name.has_value()) {
        return; 
    }

    const QString compile_cmd = preferences.compiler_path + ' ' \
        + preferences.compiler_args + ' '\
        + file_name.value() + " -o " + QFileInfo(file_name.value()).baseName() + '\n';

    ui->terminal->runCommand(compile_cmd);
}

void MainWindow::execute() {
    const auto cur_editor = get_cur_editor();
    std::optional<QString> file_name = cur_editor->get_file_name();
    if (!file_name.has_value()) {
        return;
    }

    const QString exec_cmd = "./" + QFileInfo(file_name.value()).baseName() + '\n';

    ui->terminal->runCommand(exec_cmd);
}

void MainWindow::terminal_focus() {
    const bool has_focus = ui->terminal->hasFocus();
    const auto ind_in_splitter = ui->terminal_splitter->indexOf(ui->terminal);
    auto sizes = ui->terminal_splitter->sizes();

    if (has_focus) {
        prev_terminal_height = ui->terminal->height();

        sizes[ind_in_splitter] = 0;

        ui->terminal->clearFocus();

    } else {
        if (!sizes[ind_in_splitter] && ind_in_splitter && prev_terminal_height <= sizes[ind_in_splitter - 1]) {
            sizes[ind_in_splitter-1] -= prev_terminal_height; // possible undefined behaviour (ind_in_splitter < 0 or prev_terminal_height > sizes[in...])
            sizes[ind_in_splitter] = prev_terminal_height;\
        }
        ui->terminal->show();
        ui->terminal->setFocus(Qt::FocusReason::ShortcutFocusReason);
    }
    
    ui->terminal_splitter->setSizes(sizes);
}

void MainWindow::editor_focus() {
    get_cur_editor()->setFocus(Qt::FocusReason::MouseFocusReason);
}

void MainWindow::folder_focus() {
    ui->tree_view->setFocus(Qt::FocusReason::ShortcutFocusReason);
}

void MainWindow::open_preferences() {
    PreferencesWidget *preferences_widget = new PreferencesWidget(&preferences);
    preferences_widget->setAttribute(Qt::WA_DeleteOnClose);

    preferences_widget->show();
}

void MainWindow::ask_open_folder() {
    const QString folder_path = QFileDialog::getExistingDirectory(this, "Open folder", "");
    if (folder_path.isEmpty()) {
        return;
    }

    open_folder(folder_path);

    
}

void MainWindow::show_folder_context_menu(const QPoint &point) const {
    folder_context_menu->exec(ui->tree_view->viewport()->mapToGlobal(point));

}

void MainWindow::open_file(const QString &file_name) {
    const int tabs_amount = ui->tab_widget->count();

    for (int i = 0; i < tabs_amount; ++i) {
        auto tab_editor = get_tab_editor(i);
        std::optional<QString> editor_file_name = tab_editor->get_file_name();
        if (editor_file_name.has_value() && editor_file_name.value() == file_name) {
            ui->tab_widget->setCurrentIndex(i);
            return;
        }
    }

    QFile file(file_name);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Can't open file", QMessageBox::Ok);
        return;
    }

    QString file_contents = file.readAll();

    file.close();

    open_new_tab();

    auto cur_editor = get_cur_editor();

    cur_editor->setPlainText(file_contents);
    cur_editor->set_file_name(file.fileName());
    cur_editor->document()->setModified(false);

    const int cur_tab_ind = ui->tab_widget->currentIndex();
    ui->tab_widget->setTabText(cur_tab_ind, QFileInfo(file.fileName()).fileName());
}

void MainWindow::save_file(const QString &file_name) {
    QFile file(file_name);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Can't open file", QMessageBox::Ok);
        return;
    }

    file.write(get_cur_editor()->toPlainText().toUtf8());

    file.close();

    const int cur_tab_ind = ui->tab_widget->currentIndex();
    ui->tab_widget->setTabText(cur_tab_ind, QFileInfo(file.fileName()).fileName());

    get_cur_editor()->document()->setModified(false);
}

void MainWindow::open_folder(const QString &folder_name) {
    if (!is_folder_opened) {
        ui->tree_view->setModel(fs_model);
        is_folder_opened = true;

        ui->tree_view->setHeaderHidden(true);
        for (int i = 1; i < fs_model->columnCount(); ++i) {
            ui->tree_view->setColumnHidden(i, true);
        }
    }

    ui->tree_view->setRootIndex(fs_model->index(folder_name));
    ui->terminal->setDirectory(folder_name);

    is_folder_opened = true;
}

QString MainWindow::get_opened_folder() const {
    return fs_model->fileInfo(ui->tree_view->rootIndex()).absoluteFilePath();
}

void MainWindow::write_settings() {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("tree_view_width", ui->tree_view->width());
    settings.setValue("tab_widget_width", ui->tab_widget->width());

    settings.setValue("terminal_height", ui->terminal->height());
    settings.setValue("tab_widget_height", ui->tab_widget->height());

    if (is_folder_opened) {
        settings.setValue("folder", get_opened_folder());
    } else {
        settings.setValue("folder", "-");
    }

    // tab-to-file
    QString opened_tabs_str;
    const int tabs_amount = ui->tab_widget->count();
    for (int i = 0; i < tabs_amount; ++i) {
        const auto tab_editor = get_tab_editor(i);
        const auto tab_name = ui->tab_widget->tabText(i);
        const auto tab_file = tab_editor->get_file_name();

        opened_tabs_str += tab_name + ':';

        if (tab_file.has_value()) {
            opened_tabs_str += tab_file.value() + ';';
        } else {
            opened_tabs_str += "-;";
        }
    }

    settings.setValue("opened_tabs", opened_tabs_str);

    settings.beginGroup("compiler");
    settings.setValue("compiler_path", preferences.compiler_path);
    settings.setValue("compiler_args", preferences.compiler_args);
    settings.endGroup();

    settings.beginGroup("view");
    
    write_font_to_settings("editor_font", preferences.editor_font, settings);

    settings.endGroup();
}

void MainWindow::read_settings() {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    // setup widget sizes
    const int tree_view_width = settings.value("tree_view_width", "100").toInt();
    const int tab_widget_width = settings.value("tab_widget_width", "500").toInt();

    const int terminal_height = settings.value("terminal_height", "200").toInt();
    const int tab_widget_height = settings.value("tab_widget_height", "400").toInt();

    ui->folder_editor_splitter->setSizes({tree_view_width, tab_widget_width});
    ui->terminal_splitter->setSizes({tab_widget_height, terminal_height});

    // setup folder
    const QString folder = settings.value("folder", "-").toString();
    if (folder != "-") {
        open_folder(folder);
    }

    // setup opened tabs
    const QStringList opened_tabs = settings.value("opened_tabs", "").toString().split(';');
    for (const auto opened_tab: opened_tabs) {
        const auto opened_tab_split = opened_tab.split(':');

        if (opened_tab_split.size() != 2) {
            continue;
        }

        const auto tab_name = opened_tab_split[0];
        const auto file_name = opened_tab_split[1];

        if (file_name != "-") {
            open_file(file_name);
        } else {
            open_new_tab();
        }

        ui->tab_widget->setTabText(ui->tab_widget->count() - 1, tab_name); 
    }

    // setup compiler settings
    settings.beginGroup("compiler");
    preferences.compiler_path = settings.value("compiler_path", "/usr/bin/gcc").toString();
    preferences.compiler_args = settings.value("compiler_args", "-Wall -Wextra -O2").toString();
    settings.endGroup();

    settings.beginGroup("view");

    preferences.editor_font = read_font_from_settings("editor_font", settings, this->font());

    for (int tab_ind = 0; tab_ind < ui->tab_widget->count(); ++tab_ind) {
        ui->tab_widget->widget(tab_ind)->setFont(preferences.editor_font);
    }

    settings.endGroup();

}

CodeEditor* MainWindow::get_cur_editor() const {
    return dynamic_cast<CodeEditor*>(ui->tab_widget->currentWidget());
}

CodeEditor* MainWindow::get_tab_editor(uint8_t tab_ind) const {
    return dynamic_cast<CodeEditor*>(ui->tab_widget->widget(tab_ind));
}

void MainWindow::setup_folder_context_menu() {
    folder_context_menu = new QMenu;

    folder_open_file_action = new QAction("Open file");
    folder_create_file_action = new QAction("New file");
    folder_delete_file_action = new QAction("Delete file");
    folder_rename_file_action = new QAction("Rename file");

    folder_context_menu->addActions(
        {
            folder_open_file_action, folder_create_file_action,
            folder_delete_file_action, folder_rename_file_action
        }
    );

    connect(folder_open_file_action,   &QAction::triggered, this, &MainWindow::open_folder_file);
    connect(folder_create_file_action, &QAction::triggered, this, &MainWindow::ask_create_file);
    connect(folder_delete_file_action, &QAction::triggered, this, &MainWindow::ask_delete_file);
    connect(folder_rename_file_action, &QAction::triggered, this, &MainWindow::ask_rename_file);

    connect(ui->tree_view, &QTreeView::customContextMenuRequested, this, &MainWindow::show_folder_context_menu);
}