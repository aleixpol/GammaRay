/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "quickinspector.h"
#include "quickitemmodel.h"

#include <common/objectbroker.h>

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/probeinterface.h>
#include <core/propertycontroller.h>
#include <core/singlecolumnobjectproxymodel.h>

#include <QQuickItem>
#include <QQuickView>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlError>

#include <QDebug>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QOpenGLContext>

Q_DECLARE_METATYPE(QQmlError)

using namespace GammaRay;

QuickInspector::QuickInspector(ProbeInterface* probe, QObject* parent) :
  QuickInspectorInterface(parent),
  m_probe(probe),
  m_itemModel(new QuickItemModel(this)),
  m_propertyController(new PropertyController("com.kdab.GammaRay.QuickItem", this))
{
  registerMetaTypes();
  probe->installGlobalEventFilter(this);

  QAbstractProxyModel* windowModel = new ObjectTypeFilterProxyModel<QQuickWindow>(this);
  windowModel->setSourceModel(probe->objectListModel());
  QAbstractProxyModel* proxy = new SingleColumnObjectProxyModel(this);
  proxy->setSourceModel(windowModel);
  probe->registerModel("com.kdab.GammaRay.QuickWindowModel", proxy);
  probe->registerModel("com.kdab.GammaRay.QuickItemModel", m_itemModel);

  QItemSelectionModel* selection = ObjectBroker::selectionModel(m_itemModel);
  connect(selection, &QItemSelectionModel::selectionChanged, this, &QuickInspector::itemSelectionChanged);

  // ### just for testing
  selectWindow(qobject_cast<QQuickWindow*>(windowModel->index(0,0).data(ObjectModel::ObjectRole).value<QObject*>()));
}

QuickInspector::~QuickInspector()
{
}

void QuickInspector::selectWindow(QQuickWindow* window)
{
  // TODO disconnect previous window
  m_window = window;
  connect(window, &QQuickWindow::frameSwapped, this, &QuickInspector::frameSwapped);
  m_itemModel->setWindow(window);
}

void QuickInspector::frameSwapped()
{
  // ### just for testing, we need to rate-limit that and only update if the client actually wants that
  emit sceneRendered(m_window->grabWindow());
}

void QuickInspector::itemSelectionChanged(const QItemSelection& selection)
{
  if (selection.isEmpty())
    return;
  const QModelIndex index = selection.first().topLeft();
  QQuickItem* item = index.data(ObjectModel::ObjectRole).value<QQuickItem*>();
  m_propertyController->setObject(item);
}

QQuickItem* QuickInspector::recursiveChiltAt(QQuickItem* parent, const QPointF& pos) const
{
  Q_ASSERT(parent);
  QQuickItem *child = parent->childAt(pos.x(), pos.y());
  if (child)
    return recursiveChiltAt(child, parent->mapToItem(child, pos));
  return parent;
}

bool QuickInspector::eventFilter(QObject *receiver, QEvent *event)
{
  if (event->type() == QEvent::MouseButtonRelease) {
    QMouseEvent *mouseEv = static_cast<QMouseEvent*>(event);
    if (mouseEv->button() == Qt::LeftButton &&
        mouseEv->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
      QQuickWindow *window = qobject_cast<QQuickWindow*>(receiver);
      if (window && window->contentItem()) {
        QQuickItem *item = recursiveChiltAt(window->contentItem(), mouseEv->pos());
        m_probe->selectObject(item);
      }
    }
  }

  return QObject::eventFilter(receiver, event);
}

void QuickInspector::registerMetaTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT1(QQuickWindow, QWindow);
  MO_ADD_PROPERTY   (QQuickWindow, bool, clearBeforeRendering, setClearBeforeRendering);
  MO_ADD_PROPERTY   (QQuickWindow, bool, isPersistentOpenGLContext, setPersistentOpenGLContext);
  MO_ADD_PROPERTY   (QQuickWindow, bool, isPersistentSceneGraph, setPersistentSceneGraph);
  MO_ADD_PROPERTY_RO(QQuickWindow, QQuickItem*, mouseGrabberItem);
  MO_ADD_PROPERTY_RO(QQuickWindow, QOpenGLContext*, openglContext);
  MO_ADD_PROPERTY_RO(QQuickWindow, uint, renderTargetId);

  MO_ADD_METAOBJECT1(QQuickView, QQuickWindow);
  MO_ADD_PROPERTY_RO(QQuickView, QQmlEngine*, engine);
  MO_ADD_PROPERTY_RO(QQuickView, QList<QQmlError>, errors);
  MO_ADD_PROPERTY_RO(QQuickView, QSize, initialSize);
  MO_ADD_PROPERTY_RO(QQuickView, QQmlContext*, rootContext);
  MO_ADD_PROPERTY_RO(QQuickView, QQuickItem*, rootObject);
}

QString QuickInspectorFactory::name() const
{
  return tr("Quick Scenes");
}