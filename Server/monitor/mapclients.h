#ifndef MAPCLIENTS_H
#define MAPCLIENTS_H

#include <QAbstractTableModel>
#include <QMap>
#include <QString>

class MapClients : public QAbstractTableModel{
    Q_OBJECT
   public:

       enum MapRoles {
           KeyRole = Qt::UserRole + 1,
           ValueRole
       };

       explicit MapClients(QObject *parent = 0);
       int rowCount(const QModelIndex& parent = QModelIndex()) const;
       int columnCount(const QModelIndex& parent = QModelIndex()) const;
       QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
       inline void setMap(QMap<QString, int>* map) { _map = map; }

   private:
       QMap<QString,int>* _map;
};

#endif // MAPCLIENTS_H
