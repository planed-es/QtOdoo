#ifndef  QODOO_ENUM_H
# define QODOO_ENUM_H

# include <QMap>
# include "QOdooModel.h"

template<typename ENUM, typename VALUE, ENUM NULL_VALUE>
struct OdooEnum : public QMap<ENUM, VALUE>
{
  typedef QMap<ENUM, VALUE> Map;

  OdooEnum(const QMap<ENUM, VALUE>& init) : Map(init) {}

  ENUM fromValue(VALUE value) const
  {
    for (auto it = Map::begin() ; it != Map::end() ; ++it)
    {
      if (it.value() == value)
        return it.key();
    }
    return NULL_VALUE;
  }

  VALUE fromEnum(ENUM value) const
  {
    auto it = Map::find(value);
    return it == Map::end() ? VALUE() : *it;
  }

  std::function<VALUE (const QOdooModel::Property<ENUM>&)> propertyUpdater() const
  {
    return [this](const QOdooModel::Property<ENUM>& property) -> VALUE
    {
      return fromEnum(property.first.value_or(NULL_VALUE));
    };
  }
};

# define ODOO_ENUM_BEGIN(varname, ENUM, VALUE, NULL_VALUE) \
  OdooEnum<ENUM, VALUE, NULL_VALUE> varname(QMap<ENUM, VALUE>{
# define ODOO_ENUM_END() });

#endif
