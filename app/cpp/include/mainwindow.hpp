#pragma once

#include <QMainWindow>

#include "ds/preferences.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QFileSystemModel;
class QAction;
class QMenu;
class QShowEvent;
class CodeEditor;
class QVariant;
QT_END_NAMESPACE

class CodeforcesWrapper;
struct CodeforcesProblem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void get_problem(QString url);
private slots:
    // tab
    void open_new_tab();
    void close_tab(int index);
    void close_current_tab();
    void next_tab();
    void prev_tab();
    void set_current_tab_saved(bool status);

    // file
    void ask_open_file();
    void ask_save_file();
    void ask_rename_file();
    void ask_create_file();
    void ask_delete_file();

    // folder
    void ask_open_folder();
    void open_folder_file();

    // run
    void compile();
    void execute();

    // focus
    void terminal_focus();
    void editor_focus();
    void folder_focus();

    //problem
    void load_problem();
    //comp
    void start_comp();

    // open
    void open_preferences();

    // custom menu override
    void show_folder_context_menu(const QPoint &point) const;
private:

    void open_file(const QString &file_name);
    void save_file(const QString &file_name);

    void open_folder(const QString &folder_name);

    QString get_opened_folder() const;

    void write_settings();
    void read_settings();

    CodeEditor* get_cur_editor() const;
    CodeEditor *get_tab_editor(uint8_t tab_ind) const;

    void setup_folder_context_menu();
private:
    Ui::MainWindow *ui;

    QFileSystemModel *fs_model;

    QMenu *folder_context_menu;
    QAction *folder_open_file_action;
    QAction *folder_create_file_action;
    QAction *folder_delete_file_action;
    QAction *folder_rename_file_action;

    CodeforcesWrapper *wrapper;

    Preferences preferences;

    int prev_terminal_height = 0; // need this to re-open terminal on shortcut with the same height

    bool is_folder_opened = false;
};