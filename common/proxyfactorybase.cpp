/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "proxyfactorybase.h"

#include <QPluginLoader>

#include <iostream>

using namespace GammaRay;

ProxyFactoryBase::ProxyFactoryBase(const PluginInfo& pluginInfo, QObject* parent):
  QObject(parent),
  m_factory(0),
  m_pluginInfo(pluginInfo)
{
}

ProxyFactoryBase::~ProxyFactoryBase()
{
}

PluginInfo ProxyFactoryBase::pluginInfo() const
{
    return m_pluginInfo;
}

QString ProxyFactoryBase::errorString() const
{
  return m_errorString;
}

void ProxyFactoryBase::loadPlugin()
{
  if (m_factory)
    return;
  QPluginLoader loader(pluginInfo().path(), this);
  m_factory = loader.instance();
  if (m_factory) {
    m_factory->setParent(this);
  } else {
    m_errorString = loader.errorString();
    std::cerr << "error loading plugin " << qPrintable(pluginInfo().path())
              << ": " << qPrintable(loader.errorString()) << std::endl;
  }
}
