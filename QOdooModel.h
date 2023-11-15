#ifndef  QODOO_MODEL_H
# define QODOO_MODEL_H

# include "QOdoo_global.h"
# include <QObject>
# include <QPair>
# include <QVector>
# include <QVariantList>
# include <QVariantMap>
# include <QDate>
# include <optional>
# define construct_qodoo_prop(type, name, odooName) \
  _##name(#odooName, [this]() { emit name##Changed(); })
# define construct_qodoo_prop_def(type, name, odooName, defaultValue) \
  _##name(#odooName, defaultValue, [this]() { emit name##Changed(); })

class OdooService;

class QTODOO_LIBRARY_EXPORT QOdooModel : public QObject
{
  friend class OdooService;
  Q_OBJECT
  Q_PROPERTY(int id READ id NOTIFY idChanged)
  Q_PROPERTY(bool markedForDeletion MEMBER markedForDeletion NOTIFY markedForDeletionChanged)
public:
  typedef unsigned long IdType;
  constexpr static const bool supportsPagination = true;

  explicit QOdooModel(QObject* parent = nullptr);
  virtual ~QOdooModel();

  IdType id() const { return _id; }
  void setId(IdType value);
  void markForDeletion() { markedForDeletion = true; emit markedForDeletionChanged(); }
  virtual void onSaved();

  virtual QVariantMap xmlrpcTransaction() const = 0;
  virtual const char* odooTypename() const = 0;

  virtual void save(OdooService& odoo, std::function<void()> callback);
  virtual bool hasChanged() const;
  virtual QStringList propertyNames() const;

  struct PropertyInterface
  {
    PropertyInterface(const QString& key, std::function<void()> signal) : key(key), second(false), emitChange(signal) {}
    bool hasChanged() const { return second; }
    void resetState() { second = false; }

    QString key;
    bool second;
    std::function<void()> emitChange;
  };

  template<typename TYPE>
  struct Property : public PropertyInterface
  {
    Property(const QString& key, std::function<void()> signal) : PropertyInterface(key, signal) { }
    Property(const QString& key, const TYPE& value, std::function<void()> signal) : PropertyInterface(key, signal) { first = value; this->second = false; }
    TYPE operator*() const { return first.value(); }
    TYPE valueOr(TYPE altValue) const { return first.value_or(altValue); }
    void set(const TYPE& value)
    {
      if (!first.has_value() || *first != value)
      {
        first = value;
        this->second = true;
        emitChange();
      }
    }
    Property& operator=(const TYPE& value) { set(value); return *this; }

    std::optional<TYPE> first;
  };

  struct StringProperty : Property<QString>
  {
    StringProperty(const QString& key, std::function<void()> signal) : Property<QString>(key, signal) {}
    StringProperty(const QString& key, const QString& value, std::function<void()> signal) : Property<QString>(key, value, signal) {}
    QString operator*() const { return first.value_or(QString()); }
    void loadFromVariant(QVariant value);
  };

  struct IdProperty : Property<IdType>
  {
    IdProperty(const QString& key, std::function<void()> signal) : Property<IdType>(key, 0, signal) {}
    IdProperty(const QString& key, IdType value, std::function<void()> signal) : Property<IdType>(key, value, signal) {}
    void loadFromVariant(QVariant value);
  };

signals:
  void idChanged();
  void markedForDeletionChanged();
  void requestPropertyRefresh();

private:
  void onRefreshProperties();

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

          // IGNORE
          if (item->markedForDeletion && item->id() == 0)
            continue ;
          // DELETE
          else if (item->markedForDeletion)
            entry << 2 << QVariant::fromValue(item->id()) << false;
          // CREATE
          else if (item->id() == 0)
            entry << 0 << QVariant::fromValue(item->id()) << transaction;
          // UPDATE
          else
            entry << 4 << QVariant::fromValue(item->id()) << transaction;
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
  bool markedForDeletion = false;
};

Q_DECLARE_METATYPE(QOdooModel::IdType)

# include "QOdooEnum.h"

#endif
