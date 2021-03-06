/*
  localserverdevice.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "localserverdevice.h"

#include <QLocalServer>
#include <QLocalSocket>

using namespace GammaRay;

LocalServerDevice::LocalServerDevice(QObject* parent):
    ServerDeviceImpl<QLocalServer>(parent)
{
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SIGNAL(newConnection()));
}

bool LocalServerDevice::listen()
{
    QLocalServer::removeServer(m_address.path());
    return m_server->listen(m_address.path());
}

QUrl LocalServerDevice::externalAddress() const
{
    return m_address;
}
