#ifndef  QODOO_MODEL_H
# define QODOO_MODEL_H

# include <QObject>
# include <QPair>
# include <QVector>
# include <QVariantList>
# include <QVariantMap>
# include <QDate>
# include <optional>

class OdooService;

class QOdooModel : public QObject
{
  friend class OdooService;
  Q_OBJECT
  Q_PROPERTY(int id READ id NOTIFY idChanged)
public:
  typedef unsigned long IdType;

  explicit QOdooModel(QObject* parent = nullptr) : QObject(parent) {}

  IdType id() const { return _id; }
  void setId(IdType value);
  virtual void onSaved();

  virtual QVariantMap xmlrpcTransaction() const = 0;
  virtual const char* odooTypename() const = 0;

  virtual void save(OdooService& odoo, std::function<void()> callback);
  virtual bool hasChanged() const;
  virtual QStringList propertyNames() const;

  struct PropertyInterface
  {
    PropertyInterface(const QString& key) : key(key), second(false) {}
    bool hasChanged() const { return second; }
    void resetState() { second = false; }

    QString key;
    bool second;
  };

  template<typename TYPE>
  struct Property : public PropertyInterface
  {
    Property(const QString& key) : PropertyInterface(key) { }
    Property(const QString& key, const TYPE& value) : PropertyInterface(key) { first = value; this->second = false; }
    TYPE operator*() const { return first.value(); }
    TYPE valueOr(TYPE altValue) const { return first.value_or(altValue); }
    void set(const TYPE& value) { if (!first.has_value() || *first != value) { first = value; this->second = true; } }
    Property& operator=(const TYPE& value) { set(value); return *this; }

    std::optional<TYPE> first;
  };

  struct StringProperty : Property<QString>
  {
    StringProperty(const QString& key) : Property<QString>(key) {}
    StringProperty(const QString& key, const QString& value) : Property<QString>(key, value) {}
    QString operator*() const { return first.value_or(QString()); }
  };

signals:
  void idChanged();

protected:
  struct XmlRpcTransaction : public QVariantMap
  {
    void addProperty(const Property<QDate>& property)
    {
      if (property.hasChanged())
        insert(property.key, property.first.value_or(QDate()).toString("yyyy-MM-dd"));
    }

    template<typename PROPERTY_TYPE>
    void addProperty(const Property<PROPERTY_TYPE>& property)
    {
      if (property.hasChanged())
        insert(property.key, QVariant::fromValue(*property));
    }

    template<typename PROPERTY_TYPE, typename RETURN_TYPE>
    void addProperty(const Property<PROPERTY_TYPE>& property, std::function<RETURN_TYPE(const Property<PROPERTY_TYPE>&)> converter)
    {
      if (property.hasChanged())
        insert(property.key, converter(property));
    }

    template<typename LIST_ITEM>
    void addRelationship(const QString& key, const QVector<LIST_ITEM*>& items)
    {
      QVariantList list;

      for (const LIST_ITEM* item : items)
      {
        const auto* model = reinterpret_cast<const QOdooModel*>(item);
        QVariantList entry;

        if (model->hasChanged())
        {
          QVariantMap transaction = model->xmlrpcTransaction();

          if (item->id() != 0)
            entry << 4 << QVariant::fromValue(item->id()) << transaction;
          else
            entry << 0 << 0 << transaction;
        }
        else
          entry << 4 << QVariant::fromValue(item->id()) << false;
        list.push_back(entry);
      }
      insert(key, list);
    }
  };

  friend struct XmlRpcTransaction;

  virtual void fetchRelationships(OdooService&, QVariantMap, std::function<void()> callback) { callback(); }

  QVector<PropertyInterface*> _properties;
  IdType _id = 0;
};

# include "QOdooEnum.h"

#endif
