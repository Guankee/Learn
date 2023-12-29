#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_GL3D.h"
#include "GLWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GL3D; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QMainWindow *parent = 0);
    ~MainWindow();

private:
    Ui::GL3D *ui;
    GLWidget*widget;
};

#endif // MAINWINDOW_H
