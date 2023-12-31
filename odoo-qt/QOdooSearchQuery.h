#ifndef  QODOO_SEARCH_QUERY_H
# define QODOO_SEARCH_QUERY_H

# include <QString>
# include <QVariantList>

class OdooService;

class QOdooSearchQuery
{
  friend class OdooService;
public:
  enum LikeOption
  {
    CaseSensitive   = 1,
    CaseInsensitive = 2,
    ExactMatch      = 4
  };

private:
  class QOdooColumn
  {
    QOdooSearchQuery& query;
    QString name;
  public:
    QOdooColumn(QOdooSearchQuery& query, const QString& name) : query(query), name(name)
    {}

    QOdooSearchQuery& compare(QString op, const char* comparator)
    {
      return compare(op, QString(comparator));
    }

    template<typename TYPE>
    QOdooSearchQuery& compare(QString op, TYPE comparator)
    {
      QVariantList comp;

      comp << name << op << QVariant::fromValue(comparator);
      query.params.push_back(comp);
      return query;
    }

    template<typename TYPE>
    QOdooSearchQuery& operator==(TYPE comparator)
    {
      return compare("=", comparator);
    }

    template<typename TYPE>
    QOdooSearchQuery& operator!=(TYPE comparator)
    {
      return compare("!=", comparator);
    }

    template<typename TYPE>
    QOdooSearchQuery& operator>(TYPE comparator)
    {
      return compare(">", comparator);
    }

    template<typename TYPE>
    QOdooSearchQuery& operator<(TYPE comparator)
    {
      return compare("<", comparator);
    }

    template<typename TYPE>
    QOdooSearchQuery& operator<=(TYPE comparator)
    {
      return compare("<=", comparator);
    }

    template<typename TYPE>
    QOdooSearchQuery& operator>=(TYPE comparator)
    {
      return compare(">=", comparator);
    }

    QOdooSearchQuery& in(QVariantList values)
    {
      QVariantList comp;

      comp << name << "in";
      comp.push_back(values);
      query.params.push_back(comp);
      return query;
    }

    QOdooSearchQuery& like(const QVariant& value, char options = CaseInsensitive)
    {
      return like(value.toString(), options);
    }

    QOdooSearchQuery& like(const QString& value, char options = CaseInsensitive)
    {
      QVariantList comp;
      QString operation("like");

      if ((options & CaseInsensitive) > 0)
        operation = 'i' + operation;
      if ((options & ExactMatch) > 0)
        operation = '=' + operation;
      comp << name << operation << value;
      query.params.push_back(comp);
      return query;
    }
  };

public:
  QOdooSearchQuery() {}

  QOdooColumn where(const QString& column)
  {
    return QOdooColumn(*this, column);
  }

  QOdooSearchQuery& fields(const QStringList& value) { _fields = value; return *this; }
  QOdooSearchQuery& offset(unsigned long value) { _offset = value; return *this; }
  QOdooSearchQuery& limit(unsigned long value) { _limit = value; return *this; }

  unsigned long getOffset() const { return _offset; }
  unsigned long getLimit() const { return _limit; }

private:
  QVariantList  params;
  QStringList   _fields;
  unsigned long _offset = 0;
  unsigned long _limit = 0;
};

#endif
