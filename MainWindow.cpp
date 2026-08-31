#include "mainwindow.h"

#include "ValuesModel.h"

#include <QInputDialog>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
   , m_model(nullptr)
   , m_view(nullptr)
{
   ui.setupUi(this);

   m_model = new ValuesModel();
   ui.treeView->setModel(m_model);
   ui.tableView->setModel(m_model);

   m_model->setTreeMode(false);
   m_view = ui.tableView;

   // test data
   m_model->addNewName("app1::this::test");
   m_model->addNewName("app1::test");
   m_model->addNewName("app2::this::test1");
   m_model->addNewName("app2::this::test2");
   m_model->addNewName("app3::this::test1");
   m_model->addNewName("app3::this::test2");
   m_model->addNewName("app2::test::lol");
   // end set data

   ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui.tableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
   ui.tableView->verticalHeader()->setVisible(false);
   ui.tableView->horizontalHeader()->setStretchLastSection(true);
   ui.tableView->horizontalHeader()->setHighlightSections(false);
   ui.tableView->horizontalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
   ui.tableView->horizontalHeader()->setSectionsClickable(false);

   connect(ui.treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::composeSelection);
   connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
           &MainWindow::composeSelection);

   connect(ui.aTable, &QAction::triggered, this, [this]()
   {
      m_model->setTreeMode(false);
      m_view = ui.tableView;
      ui.stWdg->setCurrentWidget(ui.pTable);
   });
   connect(ui.aTree, &QAction::triggered, this, [this]()
   {
      m_model->setTreeMode(true);
      m_view = ui.treeView;
      ui.treeView->expandAll();
      ui.stWdg->setCurrentWidget(ui.pTree);
   });

   connect(ui.pbSetValue, &QPushButton::clicked, this, &MainWindow::onSetValue);
   connect(ui.pbDeleteItem, &QPushButton::clicked, this, &MainWindow::onDeleteItem);
}

MainWindow::~MainWindow()
{
   delete m_model;
}

void MainWindow::resizeHeaders()
{
   // table
   int allWidth = ui.tableView->width() - 6 - ui.tableView->verticalScrollBar()->width();
   const int nameWidget = 600;
   ui.tableView->setColumnWidth(ValuesModel::Columns::NAME, nameWidget);
   ui.tableView->setColumnWidth(ValuesModel::Columns::VARIABLE, allWidth - nameWidget);
   // view
   allWidth = ui.treeView->width() - 6 - ui.treeView->verticalScrollBar()->width();
   ui.treeView->setColumnWidth(ValuesModel::Columns::NAME, nameWidget);
   ui.treeView->setColumnWidth(ValuesModel::Columns::VARIABLE, allWidth - nameWidget);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
   resizeHeaders();
   QMainWindow::resizeEvent(event);
}

void MainWindow::composeSelection()
{
   if (!m_view)
   {
      return;
   }
   QModelIndexList indexes = m_view->selectionModel()->selectedRows();

   for (int i = 0; i < indexes.size(); ++i)
   {
      if (indexes[i].isValid())
      {
         QModelIndex currentIndex = indexes[i];
         qDebug()
             << currentIndex.row()
             << currentIndex.column()
             << m_model->getFullName(currentIndex)
             << m_model->findRowByIndex(currentIndex);
      }
   }
}

void MainWindow::onSetValue()
{
   if (!m_view)
   {
      return;
   }
   QModelIndexList indexes = m_view->selectionModel()->selectedRows();

   bool ok = false;
   double value = QInputDialog::getDouble(this, tr("Enter Number"), tr("Please enter a decimal value:"), 0.0, -1000.0,
                                          1000.0, 2, &ok);

   for (int i = 0; i < indexes.size(); ++i)
   {
      if (indexes[i].isValid())
      {
         QModelIndex currentIndex = indexes[i];
         if (ok)
         {
            int row = m_model->findRowByIndex(currentIndex);
            m_model->updateValue(row, value);
         }
      }
   }
}

void MainWindow::onDeleteItem()
{
   if (!m_view)
   {
      return;
   }
   QModelIndexList indexes = m_view->selectionModel()->selectedRows();
   for (int i = 0; i < indexes.size(); ++i)
   {
      if (indexes[i].isValid())
      {
         int row = m_model->findRowByIndex(indexes[i]);
         m_model->removeValue(row);
      }
   }
}
