# QtOdoo

Qt library to communicate with an Odoo server using the External API.
Primarily designed for backend work, it also provides tools useful for
integrating Odoo with QML frontends.

### Building

You will first need to build and install the [QtXMLRpc](https://github.com/planed-es/QtXMLRpc)
on your system, before building this library.

Build this library with the following command:

```
mkdir build
cd build
cmake ..
make
```

Use the following command to install the library on your system:

```
sudo make install
```

### Using with CMake 

QtOdoo and QtXMLRpc use pkgconfig: once installed on your system, you can easily add it to
your projects using CMake's pkgconfig extension:

```cmake
# import Qt
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
# ...

# add the pkgconfig extension
find_package(PkgConfig)

# import QtXMLRpc and QtOdoo
pkg_check_modules(QTXMLRPC REQUIRED Qt${QT_VERSION_MAJOR}XMLRpc>=1.0)
pkg_check_modules(QTODOO REQUIRED Qt${QT_VERSION_MAJOR}Odoo>=1.0)

# register the libraries include directories
include_directories(${QTXMLRPC_INCLUDE_DIRS} ${QTODOO_INCLUDE_DIRS})

# link the libraries to your compile target
target_link_libraries(target PRIVATE
  # ...
  ${QTXMLRPC_LIBRARIES} ${QTODOO_LIBRARIES})

# register QtOdoo's compile flags:
target_compile_options(target PUBLIC
  ${QTODOO_CFLAGS_OTHER})
```

## Connecting to your server

```c++
void odoo_test()
{
  QUrl odooUrl("https://odoo.domain.com");
  QSharedPointer<OdooService> service(new OdooService(odooUrl));

  service->authenticate("database", "username", "password", [service]()
  {
    qDebug() << "Connected to Odoo";
  });
}
```

N.B.: Asynchronous procedures always take a callback as their last parameter. Note how we captured the `service` shared pointer within our callback: it is very important to make sure that the objects used to call such asycnhronous method are not wiped from memory during the operation, which is why using smart pointers and capturing them in the callbacks can be useful, even when the captured object isn't going to be used within the callback.

## Interacting with models

To fetch models from Odoo, we use `OdooService`'s `fetch` method, alongside
a `QOdooSearchQuery` object. The latter lets you handle pagination, filters,
and pick the fields you wish to fetch from a model:

```c++
void odoo_test()
{
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    QOdooSearchQuery query;

    query.fields(QOdooPartner().propertyNames());
    query.limit(100);
    query.offset(0);
    query.where("name") != QString("Excluded name");
    service->fetch<QOdooPartner>(query, [service](QVector<QOdooPartner*> results)
    {
      for (QOdooPartner* partner : results)
        qDebug() << "Found partner" << partner->id() << partner->name();
    });
  });
}
```

### Ownership of fetched models

Fetched models implement `QObject` and are parented to the `OdooService` object.
To better handle their lifetime, you may use `setParent` to parent them to another
`QObject`. You may also safely delete them at anytime.

### Creating models

You may also perform change and update models, or create them from scratch using `QOdoo::save`. The following examples create a new incoming invoice from scratch using `QOdooInvoice` and `QOdooInvoiceLine`:

```c++
void odoo_test()
{
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    QSharedPointer<QOdooInvoice> invoice(new QOdooInvoice);
    QOdooInvoiceLine* line1 = invoice->lineAt(0);
    QOdooInvoiceLine* line2 = invoice->lineAt(1);

    invoice->setMoveType(QOdooInvoice::IncomingInvoice);
    invoice->setState(QOdooInvoice::PostedState);
    invoice->setRef("Invoice Reference");
    invoice->setName("INC_INV_001");
    invoice->setInvoiceDate(QDate::currentDate());
    invoice->setNarraiton("RGPD of sorts");
    line1->setName("product A");
    line1->setProductId(1);
    line1->setPriceUnit(50);
    line2->setName("product B");
    line2->setProductId(2);
    line2->setPriceUnit(12);
    service->save(*invoice, [service, invoice]()
    {
      qDebug() << "Invoice saved successfully.";
      qDebug() << "The invoice id in the odoo database is" << invoice->id();
    });
  });
}
```

Note that calling for `QOdooInvoice::lineAt` will either return an existing line at the given index, or create as many lines as necessary before returning a newly created line at the given index.

The `QOdoo::save` method can also be called on models that were fetched using `QOdoo::fetch`. In which case, the models will be updated instead of created.

### Destroying models

You may also remove models from the odoo database using `QOdoo::destroy`:

```c++
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    QOdooSearchQuery query;

    query.fields(QOdooPartner().propertyNames());
    query.where("name") == QString("Partner to remove");
    service->fetch<QOdooPartner>(query, [service](QVector<QOdooPartner*> results)
    {
      for (QOdooPartner* partner : results)
      {
        service->destroy(*partner, [service, partner]()
        {
          qDebug() << "Successfully removed partner" << partner->id() << partner->name();
        });
      }
    });
  });
```

## Search Queries

The `QOdooSearchQuery` objects allows you to construct complex queries to filter the objects you want to query.

You *must* provide a list of the fields you want to fetch when using `QOdooSearchQuery`. For instance, to fetch the names of partners:

```c++
void queryNames(QSharedPointer<OdooService> service)
{
  QOdooSearchQuery query;

  query.fields({"name", "country_id"});
  query.limit(1);
  service->fetch<QOdooPartner>(query, [service](QVector<QOdooPartner*> results)
  {
    if (results.size())
    {
      QOdooPartner* partner = results.first();

      qDebug() << "Fetched partner" << partner->name() << "with country id" << partner->countryId();
    }
  });
}
```

When in doubt, here's a simple way to fetch all the supported fields of a model by using the list of property names from any odoo model, such as:

```c++
query.fields(QOdooPartner().propertyNames());
```

### Comparaison operators

You can build your queries criterias by using the `where` method to pick a column,
and your typical operators (`==, !=, >=, <=, >, <`) to define a condition: 

```c++
void queryCountries(QSharedPointer<OdooService> service)
{
  QOdooSearchQuery query;

  query.fields({"name", "code"});
  query.where("phone_code") >= 30;
  query.where("state_required") == true;
  query.where("zip_required") != false;
  service->fetch<QOdooCountry>(query, [service](QVector<QOdooCountry*> results)
  {
  // ...
  });
}
```

##### Like operator

QOdooSearchQuery also supports the `LIKE` operator:

```c++
void queryCountries(QSharedPointer<OdooService> service)
{
  // ...
  query.where("name").like("pain");
  // ...
}
```

This will ask for models with a name containing "pain". It is case insensitive by default. You can
also build more precise queries using options:

```c++
void queryCuntries(QSharedPointer<OdooService> service)
{
  // ...
  query.where("name").like("%stan", QOdooSearchQuery::CaseSensitive | QOdooSearchQuery::ExactMatch);
  // ...
}
```

The previous example's name filter is now case sensitive. We've also used the `QOdooSearchQuery::ExactMatch`
option to search for an entire pattern: in this case, the `%stan` pattern will look for names that end in "stan".

##### In operator

Sometimes, rather than look for a single value in a given column, you'll want to fetch any model that matches
a list of values. This can be achieved using the `in` method:

```c++
void queryCountries(QSharedPointer<OdooService> service)
{
  // ...
  query.where("country_code").in({"ES", "FR", "IT"});
  // ...
}
```

## Collections

Collections are helper classes designed to help you navigate through pages of models.
In the following example, we use a C++ controller to fetch and iterate through the
first page of `resources.partners`:

```c++
class MyController : public QObject
{
  QOdooCollection<QOdooPartner> collection;
public:
  MyController(OdooService* service) :
    QObject(service),
    collection(*service, this)
  {
    connect(&collection, &QOdooCollectionInterface::modelsChanged, this, &MyController::onModelsChanged);
  }

  void findByType(QOdooPartner::CompanyType type)
  {
    QOdooSearchQuery query;

    query.fields(QOdooPartner().propertyNames());
    switch (type)
    {
    case QOdooPartner::Company:
      query.where("company_type") == "company";
      break ;
    case QOdooPartner::Individual:
      query.where("company_type") != "company";
      break ;
    }
    collection.setQuery(query);
  }

  void onModelsChanged()
  {
    qDebug() << "Received results, page" << collection.page();
    for (const QOdooPartner& model : collection) // collections are iterables
      qDebug() << "- found partner:" << model.id() << model.name();
  }
};

void odoo_test()
{
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    MyController* controller = new MyController(service.get());

    controller.findByType(QOdooPartner::Company);
  });
}
```

The `setQuery` method is used to store the parameters of the query the
collection is currently running. Using `setQuery` itself will trigger
a fetching request, which will in turn trigger the `onModelsChanged`
slot as soon as a response is received.

Once at least a first fetching request has been sent, the collection will
store and update the `count` and `pageCount` attributes to let you know
how much more resources are available:

```
  void onModelsChanged()
  {
    qDebug() << "Total items: " << collection.count() << ", total pages: " << collection.pageCount();
  }
```

To change the range of items currently stored in the collection, without
changing the filtering query that's being used, you may use the `setPage`
or `setLimit` methods, which will respectively change the current page or
the amount of items stored per page.

```
  void onModelsChanged()
  {
    qDebug() << "Received results, page" << collection.page();
    for (const QOdooPartner& model : collection)
      qDebug() << "- found partner:" << model.id() << model.name();
    
    // the following lines will have the Controller fetch each
    // page one by one until there is no further pages:
    if (collection.page() < collection.pageCount())
        collection.setPage(collection.page() + 1);
  }
```

### Collections and QML

Collections are also a great starting point to expose your backend to a QML
frontend. For instance, we could very quickly setup a UI to display lists
of products with a simple C++ class an QML file.

First, we define a QML component from C++, by implementing QOdooCollection
with a specific type of resource, and exporting it to our QML module as
`ProductCollection`:

```c++
#include <odoo-qt/QOdooCollection.h>
#include <odoo-qt/QOdooProduct.h>

class ProductCollection : public QOdooCollection<QOdooProduct>
{
  Q_OBJECT
  QML_NAMED_ELEMENT(ProductCollection)
public:
  explicit ProductCollection(QObject* parent = nullptr) : QOdooCollection<QOdooProduct>(parent)
  {
  }
};
```

Then, we implement a QML view which will make use of QOdooCollection's
pagintion capabilities:

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.name.odoo // your custom module providing ProductCollection

ColumnLayout {
  ProductCollection {
    id: collection
    page: 0
    limit: 15
  }

  ListView {
    model: collection.models
    delegate: Label {
      text: collection.models[index]
    }
  }

  Text {
    text: `Page ${collection.page} / ${collection.pageCount}`
  }

  RowLayout {
    Button {
      text: "Previous page"
      onClicked: if (collection.page > 0) { collection.page-- }
    }
    Button {
      text: "Next page"
      onClicked: if (collection.page < collection.pageCount) { collection.page++ }
    }
  }
}
```
