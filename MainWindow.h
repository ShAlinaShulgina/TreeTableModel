#pragma once

#include "ui_mainwindow.h"

#include <QMainWindow>

class ValuesModel;

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

protected:
   void resizeEvent(QResizeEvent *event) override;

private:
   Ui::MainWindow ui;
   ValuesModel *m_model;
   QAbstractItemView *m_view;

   void resizeHeaders();

private slots:
   void composeSelection();
   void onSetValue();
   void onDeleteItem();
};
