#ifndef DOWNLOADMANAGEMENT_H
#define DOWNLOADMANAGEMENT_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QVariant>

#include <memory>

#include <kiwix/downloader.h>

#include "library.h"

typedef QMap<QString, QVariant> DownloadInfo;

class DownloadState
{
public:
    double progress = 0;
    QString completedLength;
    QString downloadSpeed;
    bool paused = false;

public:
    void update(const DownloadInfo& info);
};

class DownloadManager : public QObject
{
    Q_OBJECT

public: // types

    // BookId -> DownloadState map
    class Downloads
    {
    private:
        typedef std::shared_ptr<DownloadState> DownloadStatePtr;
        typedef QMap<QString, DownloadStatePtr> ImplType;

    public:
        void set(const QString& id, DownloadStatePtr d) {
            const QMutexLocker threadSafetyGuarantee(&mutex);
            impl[id] = d;
        }

        DownloadStatePtr value(const QString& id) const {
            const QMutexLocker threadSafetyGuarantee(&mutex);
            return impl.value(id);
        }

        QList<QString> keys() const {
            const QMutexLocker threadSafetyGuarantee(&mutex);
            return impl.keys();
        }

        void remove(const QString& id) {
            const QMutexLocker threadSafetyGuarantee(&mutex);
            impl.remove(id);
        }

    private:
        ImplType impl;
        mutable QMutex mutex;
    };

public: // functions
    DownloadManager(const Library* lib, kiwix::Downloader *downloader);

    DownloadInfo getDownloadInfo(QString bookId) const;
    void restoreDownloads();
    void updateDownloads();

    // returns the download id
    std::string startDownload(const std::string& url, const std::string& downloadDirPath);
    void pauseDownload(const QString& bookId);
    void resumeDownload(const QString& bookId);

signals:
    void downloadUpdated(QString bookId, const DownloadInfo& );
    void downloadDisappeared(QString bookId);

protected: // data
    const Library* const     mp_library;
    kiwix::Downloader* const mp_downloader;
    Downloads                m_downloads;
};

#endif // DOWNLOADMANAGEMENT_H
