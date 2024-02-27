#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include <QMainWindow>
#include<iostream>
#include <QtWidgets/QMainWindow>
#include "ui_GL3D.h"
// #include "GLWidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QOpenGLWidget>
#include <QString>
#include <QUrl>
QT_BEGIN_NAMESPACE
namespace Ui { class GL3D; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QMainWindow *parent = 0);
    ~MainWindow();
	// void dragEnterEvent(QDragEnterEvent* event) override;
	// void dropEvent(QDropEvent* event) override;
private:
    Ui::GL3D *ui;
    // GLWidget*widget;
};

#endif // MAINWINDOW_H
