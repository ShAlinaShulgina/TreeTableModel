#include "FormattingProxyModel.h"

FormattingProxyModel::FormattingProxyModel(QObject *parent)
   : QIdentityProxyModel{ parent }
{}

QVariant FormattingProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
   QVariant rawData = QIdentityProxyModel::data(proxyIndex, role);
   if (role == Qt::DisplayRole && rawData.userType() == QMetaType::Double)
   {
      double value = rawData.toDouble();
      return QString::number(value, 'f', 6);
   }
   return rawData;
}
