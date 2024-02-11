#pragma once

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

static void create_file(const QString &file_path) {
    QFile file(file_path);
    bool success = file.open(QIODevice::WriteOnly);

    if (!success) {
        const QString err_msg = "Couldn't create file: " + QFileInfo(file_path).fileName() + '\n' + file.errorString();
        QMessageBox::critical(nullptr, "File creating error", err_msg, QMessageBox::Ok);
    }

    file.close();
}

static void remove_file(const QString &file_path) {
    QFile file(file_path);
    bool success = file.remove();

    if (!success) {
        const QString err_msg = "Couldn't remove file: " + QFileInfo(file_path).fileName() + '\n' + file.errorString();
        QMessageBox::critical(nullptr, "File removing error", err_msg, QMessageBox::Ok);
    }
}

static void rename_file(const QString &old_file_path, const QString &new_file_path) {
    QFile file(old_file_path);

    bool success { file.rename(new_file_path) };

    if (!success) {
        const QString err_msg = "Can't rename file: " + file.fileName() + '\n' + file.errorString();
        QMessageBox::critical(nullptr, "File renaming error", err_msg, QMessageBox::Ok);
    }
}