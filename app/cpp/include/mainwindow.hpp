#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class CodeEditor;
class QFileSystemModel;
class QAction;
class QMenu;
class QShowEvent;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open_new_tab();
    void close_tab(int index);
    void next_tab();
    void prev_tab();

    void ask_open_file();
    void ask_save_file();

    void ask_create_file();
    void ask_delete_file();

    void open_fs_file();

    void ask_open_folder();

    void show_fs_context_menu(const QPoint &point);

private:
    void open_file(const QString &file_name);
    void save_file(const QString &file_name);

    void create_file(const QString &file_name);
    void delete_file(const QString &file_path);

    void open_folder(const QString &folder_name);

    QString get_opened_folder() const;

    void write_settings();
    void read_settings();

private:
    Ui::MainWindow *ui;

    QFileSystemModel *fs_model;

    QMenu *fs_context_menu;
    QAction *fs_open_file_action;
    QAction *fs_create_file_action;
    QAction *fs_delete_file_action;

    bool is_folder_opened = false;
};
#endif // MAINWINDOW_H
