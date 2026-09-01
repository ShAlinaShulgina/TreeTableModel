#include "TreeTableModel.h"

#include <functional>

TreeTableModel::Node::Node(const QString &name, double value, Node *parent)
   : m_name(name)
   , m_value(value)
   , m_hasValue(false)
   , m_parent(parent)
   , m_isDataNode(false)
{}

TreeTableModel::Node::~Node()
{
   for (auto item : m_children)
   {
      delete item;
   }
   m_children.clear();
}

TreeTableModel::TreeTableModel(QObject *parent)
   : QAbstractItemModel{ parent }
   , m_treeMode(false)
   , m_root(new Node)
{}

TreeTableModel::~TreeTableModel()
{
   delete m_root;
}

int TreeTableModel::rowCount(const QModelIndex &parent) const
{
   if (parent.isValid() && parent.column() > 0)
   {
      return 0;
   }
   Node *parentNode = parent.isValid() ? static_cast<Node *>(parent.internalPointer()) : m_root;
   if (m_treeMode)
   {
      return parentNode->m_children.size();
   }
   else
   {
      return parent.isValid() ? 0 : m_dataNodes.size();
   }
}

int TreeTableModel::columnCount(const QModelIndex &parent) const
{
   Q_UNUSED(parent);
   return Columns::LAST;
}

QVariant TreeTableModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   Node *node = static_cast<Node *>(index.internalPointer());
   if (!node || node == m_root)
   {
      return QVariant();
   }

   if (role == Qt::DisplayRole)
   {
      switch (index.column())
      {
      case Columns::NAME:
         return m_treeMode ? node->m_name : fullPath(node);
      case Columns::VARIABLE:
         return node->m_hasValue ? QString::number(node->m_value, 'f', 6) : QVariant();
      default:
         return QVariant();
      }
   }
   return QVariant();
}

QVariant TreeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (orientation == Qt::Horizontal)
   {
      switch (role)
      {
      case Qt::DisplayRole:
         switch (section)
         {
         case Columns::NAME:
            return tr("Variable");
         case Columns::VARIABLE:
            return tr("Value");
         default:
            return QVariant();
         }
         break;
      case Qt::TextAlignmentRole:
         return Qt::AlignCenter;
      default:
         return QVariant();
      }
   }
   return QVariant();
}

Qt::ItemFlags TreeTableModel::flags(const QModelIndex &index) const
{
   if (!index.isValid())
   {
      return Qt::NoItemFlags;
   }
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex TreeTableModel::index(int row, int column, const QModelIndex &parent) const
{
   if (!hasIndex(row, column, parent))
   {
      return QModelIndex();
   }
   Node *parentNode = parent.isValid() ? static_cast<Node *>(parent.internalPointer()) : m_root;
   if (m_treeMode)
   {
      if (row < parentNode->m_children.size())
      {
         Node *child = parentNode->m_children.at(row);
         return createIndex(row, column, child);
      }
   }
   else
   {
      if (!parent.isValid())
      {
         if (row < m_dataNodes.size())
         {
            Node *node = m_dataNodes.at(row);
            return createIndex(row, column, node);
         }
      }
   }
   return QModelIndex();
}

QModelIndex TreeTableModel::parent(const QModelIndex &child) const
{
   if (!child.isValid() /* || child.column() > 0*/)
   {
      return QModelIndex();
   }
   Node *node = static_cast<Node *>(child.internalPointer());
   if (!node || node == m_root)
   {
      return QModelIndex();
   }
   Node *parentNode = node->m_parent;
   if (m_treeMode)
   {
      if (!parentNode || parentNode == m_root)
      {
         return QModelIndex();
      }
      Node *grandParent = parentNode->m_parent;
      if (!grandParent)
      {
         return QModelIndex();
      }
      int row = grandParent->m_children.indexOf(parentNode);
      if (row >= 0)
      {
         return createIndex(row, 0, parentNode);
      }
   }
   return QModelIndex();
}

bool TreeTableModel::hasChildren(const QModelIndex &parent) const
{
   if (parent.column() > 0)
   {
      return false;
   }
   Node *node = parent.isValid() ? static_cast<Node *>(parent.internalPointer()) : m_root;
   if (m_treeMode)
   {
      return !node->m_children.isEmpty();
   }
   else
   {
      return !parent.isValid() && !m_dataNodes.isEmpty();
   }
}

void TreeTableModel::setTreeMode(bool enable)
{
   if (m_treeMode == enable)
   {
      return;
   }
   m_treeMode = enable;
   beginResetModel();
   endResetModel();
}

bool TreeTableModel::isTreeMode() const
{
   return m_treeMode;
}

void TreeTableModel::resetData()
{
   beginResetModel();
   for (auto item : m_root->m_children)
   {
      delete item;
   }
   m_root->m_children.clear();
   m_dataNodes.clear();
   endResetModel();
}

int TreeTableModel::findRowByName(const QString &name) const
{
   Node *node = findNodeByPath(name);
   if (!node || !node->m_isDataNode)
   {
      return -1;
   }
   return m_dataNodes.indexOf(node);
}

int TreeTableModel::findRowByIndex(const QModelIndex &index) const
{
   return findRowByName(getFullName(index));
}

int TreeTableModel::getCountElements() const
{
   return m_dataNodes.size();
}

QString TreeTableModel::getFullName(const QModelIndex &index) const
{
   if (!index.isValid())
   {
      return QString();
   }
   Node *node = static_cast<Node *>(index.internalPointer());
   if (!node || node == m_root)
   {
      return QString();
   }
   return fullPath(node);
}

int TreeTableModel::addNewName(const QString &name)
{
   if (name.isEmpty())
   {
      return -1;
   }

   Node *existing = findNodeByPath(name);
   if (existing)
   {
      if (!existing->m_isDataNode)
      {
         existing->m_isDataNode = true;
         if (!m_treeMode)
         {
            int newRow = m_dataNodes.size();
            beginInsertRows(QModelIndex(), newRow, newRow);
            m_dataNodes.append(existing);
            endInsertRows();
            return newRow;
         }
         else
         {
            int pos = m_dataNodes.size();
            m_dataNodes.append(existing);
            return pos;
         }
      }
      return m_dataNodes.indexOf(existing);
   }

   QStringList parts = name.split("::", Qt::SkipEmptyParts);
   Node *current = m_root;

   for (const QString &part : std::as_const(parts))
   {
      Node *next = nullptr;
      for (Node *child : std::as_const(current->m_children))
      {
         if (child->m_name == part)
         {
            next = child;
            break;
         }
      }

      if (!next)
      {
         if (m_treeMode)
         {
            QModelIndex parentIndex = (current == m_root) ? QModelIndex() : indexFromNode(current);
            int insertRow = current->m_children.size();
            beginInsertRows(parentIndex, insertRow, insertRow);
         }

         next = new Node(part, 0.0, current);
         current->m_children.append(next);

         if (m_treeMode)
         {
            endInsertRows();
         }
      }

      current = next;
   }

   current->m_isDataNode = true;

   int dataRow = -1;
   if (!m_treeMode)
   {
      dataRow = m_dataNodes.size();
      beginInsertRows(QModelIndex(), dataRow, dataRow);
      m_dataNodes.append(current);
      endInsertRows();
   }
   else
   {
      dataRow = m_dataNodes.size();
      m_dataNodes.append(current);
   }

   return dataRow;
}

void TreeTableModel::updateValue(int row, const double &value)
{
   if (row < 0 || row >= m_dataNodes.size())
   {
      return;
   }

   Node *node = m_dataNodes.at(row);
   if (!node)
   {
      return;
   }

   node->m_value = value;
   node->m_hasValue = true;

   QModelIndex idx = indexFromNode(node);
   if (!idx.isValid())
   {
      return;
   }

   QModelIndex left = idx.sibling(idx.row(), NAME);
   QModelIndex right = idx.sibling(idx.row(), VARIABLE);

   if (left.isValid() && right.isValid())
   {
      emit dataChanged(left, right);
   }
}

void TreeTableModel::removeValue(int row)
{
   if (row < 0 || row >= m_dataNodes.size())
   {
      return;
   }
   Node *node = m_dataNodes.at(row);
   if (!node || node == m_root)
   {
      return;
   }
   QVector<Node *> nodesToRemove;
   std::function<void(Node *)> collectDataNodes = [&](Node *n)
   {
      if (n->m_isDataNode)
      {
         nodesToRemove.append(n);
      }
      for (Node *child : n->m_children)
      {
         collectDataNodes(child);
      }
   };
   collectDataNodes(node);

   if (m_treeMode)
   {
      for (Node *dn : nodesToRemove)
      {
         int idx = m_dataNodes.indexOf(dn);
         if (idx >= 0)
         {
            m_dataNodes.removeAt(idx);
         }
      }
   }
   else
   {
      for (int i = nodesToRemove.size() - 1; i >= 0; --i)
      {
         Node *dn = nodesToRemove.at(i);
         int idx = m_dataNodes.indexOf(dn);
         if (idx >= 0)
         {
            beginRemoveRows(QModelIndex(), idx, idx);
            m_dataNodes.removeAt(idx);
            endRemoveRows();
         }
      }
   }

   Node *parentNode = node->m_parent;
   if (parentNode)
   {
      int childIndex = parentNode->m_children.indexOf(node);
      if (childIndex >= 0)
      {
         if (m_treeMode)
         {
            QModelIndex parentIndex = (parentNode == m_root) ? QModelIndex() : indexFromNode(parentNode);
            beginRemoveRows(parentIndex, childIndex, childIndex);
         }
         parentNode->m_children.removeAt(childIndex);
         if (m_treeMode)
         {
            endRemoveRows();
         }
      }
   }
   delete node;
}

TreeTableModel::Node *TreeTableModel::findNodeByPath(const QString &path) const
{
   if (path.isEmpty())
   {
      return nullptr;
   }
   QStringList parts = path.split("::", Qt::SkipEmptyParts);
   Node *current = m_root;
   for (const QString &part : std::as_const(parts))
   {
      Node *found = nullptr;
      for (Node *child : std::as_const(current->m_children))
      {
         if (child->m_name == part)
         {
            found = child;
            break;
         }
      }
      if (!found)
      {
         return nullptr;
      }
      current = found;
   }
   return current;
}

QString TreeTableModel::fullPath(Node *node) const
{
   if (!node || node == m_root)
   {
      return QString();
   }
   QString path;
   Node *current = node;
   while (current != m_root && current != nullptr)
   {
      if (!path.isEmpty())
      {
         path.prepend("::");
      }
      path.prepend(current->m_name);
      current = current->m_parent;
   }
   return path;
}

QModelIndex TreeTableModel::indexFromNode(Node *node) const
{
   if (!node || node == m_root)
   {
      return QModelIndex();
   }
   if (m_treeMode)
   {
      QList<Node *> path;
      Node *cur = node;
      while (cur != m_root && cur != nullptr)
      {
         path.prepend(cur);
         cur = cur->m_parent;
      }
      QModelIndex parentIdx;
      for (Node *n : path)
      {
         Node *parentNode = n->m_parent;
         if (!parentNode)
         {
            return QModelIndex();
         }
         int row = parentNode->m_children.indexOf(n);
         if (row < 0)
         {
            return QModelIndex();
         }
         parentIdx = createIndex(row, 0, n);
      }
      return parentIdx;
   }
   else
   {
      int row = m_dataNodes.indexOf(node);
      if (row >= 0)
      {
         return createIndex(row, 0, node);
      }
   }
   return QModelIndex();
}

void TreeTableModel::removeNodeAndChildren(Node *node)
{
   if (!node)
   {
      return;
   }
   if (node->m_parent)
   {
      int idx = node->m_parent->m_children.indexOf(node);
      if (idx >= 0)
      {
         node->m_parent->m_children.removeAt(idx);
      }
   }
   delete node;
}
