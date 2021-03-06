/*
  translatorsmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRANSLATORSMODEL_H
#define TRANSLATORSMODEL_H

#include <QAbstractTableModel>

namespace GammaRay {
class TranslatorWrapper;

class TranslatorsModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    explicit TranslatorsModel(QObject *parent = 0);

    enum ExtraRoles
    {
      TranslatorRole = Qt::UserRole
    };

    int columnCount(
            const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    TranslatorWrapper *translator(const QModelIndex &index) const;

  public slots:
    void registerTranslator(TranslatorWrapper *translator);
    void unregisterTranslator(TranslatorWrapper *translator);

  private slots:
    void sourceDataChanged();

  private:
    QList<TranslatorWrapper *> m_translators;
};

}

#endif
