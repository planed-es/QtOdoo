#ifndef  QODOO_FUTURE_H
# define QODOO_FUTURE_H

# include <QFuture>
# include <QPromise>
# include <QVector>
# include <QException>
# include <memory>
# include <atomic>

// Small set of QFuture helpers. Written by hand against QPromise rather than
// relying on QtFuture::makeReadyFuture()/whenAll(), whose exact names moved
// around between Qt 6 minor versions - these are stable across all of them.
namespace QOdooFuture
{
  inline QFuture<void> ready()
  {
    QPromise<void> promise;

    promise.start();
    promise.finish();
    return promise.future();
  }

  template<typename T>
  QFuture<T> ready(T value)
  {
    QPromise<T> promise;

    promise.start();
    promise.addResult(std::move(value));
    promise.finish();
    return promise.future();
  }

  // Flattens a QFuture<QFuture<T>> into a QFuture<T>, propagating whichever
  // exception (outer or inner) fails first.
  template<typename T>
  QFuture<T> unwrap(QFuture<QFuture<T>> nested)
  {
    auto promise = std::make_shared<QPromise<T>>();

    promise->start();
    nested.then([promise](QFuture<T> inner)
    {
      inner.then([promise](T value)
      {
        promise->addResult(std::move(value));
        promise->finish();
      }).onFailed([promise](const QException& error)
      {
        promise->setException(error);
        promise->finish();
      });
    }).onFailed([promise](const QException& error)
    {
      promise->setException(error);
      promise->finish();
    });
    return promise->future();
  }

  // Waits for every future in `futures` to finish and collects the results
  // in the same order. If any of them fails, the combined future fails with
  // the first exception encountered (the others are still awaited but their
  // results are discarded).
  template<typename T>
  QFuture<QVector<T>> combine(const QVector<QFuture<T>>& futures)
  {
    auto promise = std::make_shared<QPromise<QVector<T>>>();

    promise->start();
    if (futures.isEmpty())
    {
      promise->addResult(QVector<T>());
      promise->finish();
      return promise->future();
    }

    auto results = std::make_shared<QVector<T>>(futures.size());
    auto pending = std::make_shared<std::atomic_int>(futures.size());
    auto failed  = std::make_shared<std::atomic_bool>(false);

    for (int i = 0 ; i < futures.size() ; ++i)
    {
      futures[i].then([promise, results, pending, failed, i](T value)
      {
        (*results)[i] = std::move(value);
        if (pending->fetch_sub(1) == 1 && !failed->load())
        {
          promise->addResult(*results);
          promise->finish();
        }
      }).onFailed([promise, pending, failed](const QException& error)
      {
        if (!failed->exchange(true))
        {
          promise->setException(error);
          promise->finish();
        }
      });
    }
    return promise->future();
  }
}

#endif
