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

#include "qsourcehighliter.h"

#include "codeeditor.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->tab_widget->setTabsClosable(true);

    fs_model = new QFileSystemModel;
    fs_model->setRootPath(QDir::root().absolutePath());

    fs_context_menu = new QMenu;
    fs_open_file_action = new QAction("Open file");

    fs_context_menu->addAction(fs_open_file_action);

    connect(fs_open_file_action, &QAction::triggered, this, &MainWindow::open_fs_file);
    connect(ui->tree_view, &QTreeView::doubleClicked, this, &MainWindow::open_fs_file);

    connect(ui->tree_view, &QTreeView::customContextMenuRequested, this, &MainWindow::show_fs_context_menu);

    connect(ui->tab_widget,         &QTabWidget::tabCloseRequested, this, &MainWindow::close_tab    );

    connect(ui->new_tab_action,     &QAction::triggered,            this, &MainWindow::open_new_tab );
    connect(ui->close_tab_action,   &QAction::triggered,            this, &MainWindow::close_tab    );
    connect(ui->next_tab_action,    &QAction::triggered,            this, &MainWindow::next_tab     );
    connect(ui->prev_tab_action,    &QAction::triggered,            this, &MainWindow::prev_tab     );
    connect(ui->open_file_action,   &QAction::triggered,            this, &MainWindow::ask_open_file    );
    connect(ui->save_file_action,   &QAction::triggered,            this, &MainWindow::ask_save_file);
    connect(ui->open_folder_action, &QAction::triggered,            this, &MainWindow::ask_open_folder  );

    read_settings();
}

MainWindow::~MainWindow() {
    write_settings();

    delete ui;
}

void MainWindow::open_new_tab() {
    CodeEditor *editor = new CodeEditor(this);

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

    auto cur_editor = dynamic_cast<CodeEditor*>(ui->tab_widget->currentWidget());

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
        QMessageBox::warning(this, "Warning", "File was not selected", QMessageBox::Ok);
        return;
    } 

    open_file(file_name);
}

void MainWindow::ask_save_file() {
    QString file_name;

    auto cur_editor = dynamic_cast<CodeEditor*>(ui->tab_widget->currentWidget());
    if (cur_editor->get_file_name().has_value()) {
        file_name = cur_editor->get_file_name().value();
    } else {
        QString file_name_filter = "";
        file_name = QFileDialog::getSaveFileName(this, "Chose file save path", "", "", &file_name_filter);
        if (!file_name.length()) {
            QMessageBox::warning(this, "Warning", "File was not selected", QMessageBox::Ok);
            return;
        }
    }

    save_file(file_name);

    
}

void MainWindow::open_fs_file() {
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

void MainWindow::ask_open_folder() {
    const QString folder_path = QFileDialog::getExistingDirectory(this, "Open folder", "");
    if (folder_path.isEmpty()) {
        return;
    }

    open_folder(folder_path);

    
}

void MainWindow::show_fs_context_menu(const QPoint &point) {
    fs_context_menu->exec(ui->tree_view->viewport()->mapToGlobal(point));

}

void MainWindow::open_file(const QString &file_name) {
    const int tabs_amount = ui->tab_widget->count();
    for (int i = 0; i < tabs_amount; ++i) {
        auto tab_editor = dynamic_cast<CodeEditor*>(ui->tab_widget->widget(i));
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

    auto cur_editor = dynamic_cast<CodeEditor*>(ui->tab_widget->currentWidget());

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

    file.write(dynamic_cast<QPlainTextEdit*>(ui->tab_widget->currentWidget())->toPlainText().toUtf8());

    file.close();

    const int cur_tab_ind = ui->tab_widget->currentIndex();
    ui->tab_widget->setTabText(cur_tab_ind, QFileInfo(file.fileName()).fileName());

    dynamic_cast<CodeEditor*>(ui->tab_widget->currentWidget())->document()->setModified(false);
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
}

void MainWindow::write_settings() {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("tree_view_width", ui->tree_view->width());
    settings.setValue("tab_widget_width", ui->tab_widget->width());

    if (is_folder_opened) {
        settings.setValue("folder", fs_model->fileInfo(ui->tree_view->rootIndex()).absoluteFilePath());
    } else {
        settings.setValue("folder", "-");
    }
}
void MainWindow::read_settings() {
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    const int tree_view_width = settings.value("tree_view_width", "100").toInt();
    const int tab_widget_width = settings.value("tab_widget_width", "500").toInt();

    ui->splitter->setSizes({tree_view_width, tab_widget_width});

    const QString folder = settings.value("folder", "-").toString();
    if (folder != "-") {
        open_folder(folder);
        is_folder_opened = true;
    }

}