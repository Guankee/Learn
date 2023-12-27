#pragma once

#include <QtWidgets/QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>


class GLWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    GLWidget(QMainWindow *parent = nullptr);
    ~GLWidget();

private:
    // Ui::GL3D *ui;
};

