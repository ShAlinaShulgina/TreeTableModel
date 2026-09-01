#ifndef FORMATTINGPROXYMODEL_H
#define FORMATTINGPROXYMODEL_H

#include <QIdentityProxyModel>

class FormattingProxyModel : public QIdentityProxyModel
{
   Q_OBJECT

public:
   explicit FormattingProxyModel(QObject *parent = nullptr);
   QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
};

#endif // FORMATTINGPROXYMODEL_H
