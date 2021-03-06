/*
  metaproperty.h

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

#ifndef GAMMARAY_METAPROPERTY_H
#define GAMMARAY_METAPROPERTY_H

#include "gammaray_core_export.h"

#include <QString>
#include <QVariant>

namespace GammaRay {

class MetaObject;

/** @brief Introspectable adaptor to non-QObject properties. */
class GAMMARAY_CORE_EXPORT MetaProperty
{
  public:
    explicit MetaProperty(const QString &name);
    virtual ~MetaProperty();

    /// User-readable name of that property
    QString name() const;

    /// Current value of the property for object @p object.
    virtual QVariant value(void *object) const = 0;

    /// Returns @c true if this property is read-only.
    virtual bool isReadOnly() const = 0;

    /// Allows changing the property value, assuming it's not read-only, for the instance @p object.
    virtual void setValue(void *object, const QVariant &value);

    /// Returns the name of the data type of this property.
    virtual QString typeName() const = 0;

    /// Returns the class this property belongs to.
    MetaObject *metaObject() const;

  private:
    friend class MetaObject;
    void setMetaObject(MetaObject *om);

    MetaObject *m_class;
    QString m_name;
};

///@cond internal
namespace detail {

template <typename T>
struct strip_const_ref { typedef T type; };

template <typename T>
struct strip_const_ref<const T&> { typedef T type; };

}
///@endcond

/** @brief Template-ed implementation of MetaProperty for member properties. */
template <typename Class, typename GetterReturnType, typename SetterArgType = GetterReturnType>
class MetaPropertyImpl : public MetaProperty
{
  private:
    typedef typename detail::strip_const_ref<GetterReturnType>::type ValueType;

  public:
    inline MetaPropertyImpl(
      const QString &name,
      GetterReturnType (Class::*getter)() const, void (Class::*setter)(SetterArgType) = 0)
      : MetaProperty(name), m_getter(getter), m_setter(setter)
    {
    }

    inline bool isReadOnly() const
    {
      return m_setter == 0 ;
    }

    inline QVariant value(void *object) const
    {
      Q_ASSERT(object);
      Q_ASSERT(m_getter);
      const ValueType v = (static_cast<Class*>(object)->*(m_getter))();
      return QVariant::fromValue(v);
    }

    inline void setValue(void *object, const QVariant &value)
    {
      if (isReadOnly())
        return;
      Q_ASSERT(object);
      Q_ASSERT(m_setter);
      (static_cast<Class*>(object)->*(m_setter))(value.value<ValueType>());
    }

    inline QString typeName() const
    {
      return QMetaType::typeName(qMetaTypeId<ValueType>()) ;
    }

  private:
    GetterReturnType (Class::*m_getter)() const;
    void (Class::*m_setter)(SetterArgType);
};


/** @brief Template-ed implementation of MetaProperty for static properties. */
template <typename Class, typename GetterReturnType>
class MetaStaticPropertyImpl : public MetaProperty
{
  private:
    typedef typename detail::strip_const_ref<GetterReturnType>::type ValueType;

  public:
    inline MetaStaticPropertyImpl(const QString &name, GetterReturnType (*getter)())
      : MetaProperty(name), m_getter(getter)
    {
    }

    inline bool isReadOnly() const
    {
      return true;
    }

    inline QVariant value(void *object) const
    {
      Q_UNUSED(object);
      Q_ASSERT(m_getter);
      const ValueType v = m_getter();
      return QVariant::fromValue(v);
    }

    inline QString typeName() const
    {
      return QMetaType::typeName(qMetaTypeId<ValueType>()) ;
    }

  private:
    GetterReturnType (*m_getter)();
};

}

#endif
