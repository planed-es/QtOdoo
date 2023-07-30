#ifndef  QODOO_MODEL_H
# define QODOO_MODEL_H

# include <QObject>
# include <QPair>
# include <QVector>
# include <QVariantList>
# include <QVariantMap>
# include "QOdoo.h"

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

  virtual QVariantMap xmlrpcTransaction() const = 0;
  virtual const char* odooTypename() const = 0;

  virtual void save(OdooService& odoo, std::function<void()> callback);
  virtual bool hasChanged() const;
  virtual QStringList propertyNames() const;

  struct PropertyInterface
  {
    PropertyInterface(const QString& key) : key(key) {}
    QString key;
    virtual bool hasChanged() const = 0;
  };

  template<typename TYPE>
  struct Property : public QPair<TYPE, bool>, public PropertyInterface
  {
    Property(const QString& key) : PropertyInterface(key) { this->second = false; }
    Property(const QString& key, const TYPE& value) : PropertyInterface(key) { this->first = value; this->second = false; }
    const TYPE& operator*() const { return this->first; }
    void set(const TYPE& value) { if (this->first != value) { this->first = value; this->second = true; } }
    Property& operator=(const TYPE& value) { set(value); return *this; }
    bool hasChanged() const override { return this->second; }
  };

signals:
  void idChanged();

protected:
  struct XmlRpcTransaction : public QVariantMap
  {
    void addProperty(const Property<QDate>& property)
    {
      if (property.hasChanged())
        insert(property.key, property.first.toString("yyyy-MM-dd"));
    }

    template<typename PROPERTY_TYPE>
    void addProperty(const Property<PROPERTY_TYPE>& property)
    {
      if (property.hasChanged())
        insert(property.key, QVariant::fromValue(property.first));
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
