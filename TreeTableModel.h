#ifndef TREETABLEMODEL_H
#define TREETABLEMODEL_H

#include <QAbstractItemModel>
#include <QVector>

class TreeTableModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   explicit TreeTableModel(QObject *parent = nullptr);
   ~TreeTableModel() override;

   int rowCount(const QModelIndex &parent = QModelIndex()) const override;
   int columnCount(const QModelIndex &parent = QModelIndex()) const override;
   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
   Qt::ItemFlags flags(const QModelIndex &index) const override;

   QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
   QModelIndex parent(const QModelIndex &child) const override;
   bool hasChildren(const QModelIndex &parent) const override;

   void setTreeMode(bool enable);
   bool isTreeMode() const;

   void resetData();

   int findRowByName(const QString &name) const;
   int findRowByIndex(const QModelIndex &index) const;

   int getCountElements() const;
   QString getFullName(const QModelIndex &index) const;

   int addNewName(const QString &name);
   void updateValue(int row, const double &value);
   void removeValue(int row);

   enum Columns
   {
      NAME,
      VARIABLE,
      LAST
   };

private:
   struct Node
   {
      QString m_name;
      double m_value;
      bool m_hasValue;
      Node *m_parent;
      QVector<Node *> m_children;
      bool m_isDataNode;

      Node(const QString &name = QString(), double value = 0.0, Node *parent = nullptr);
      ~Node();
   };

   bool m_treeMode;
   Node *m_root;
   QVector<Node *> m_dataNodes;

   Node *findNodeByPath(const QString &path) const;
   QModelIndex indexFromNode(Node *node) const;
   
   QString fullPath(Node *node) const;
   
   void removeNodeAndChildren(Node *node);
};

#endif
