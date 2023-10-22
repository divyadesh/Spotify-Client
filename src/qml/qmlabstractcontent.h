#ifndef QMLABSTRACTCONTENT_H
#define QMLABSTRACTCONTENT_H
#pragma once
#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "util/icon.hpp"
#include "util/image.hpp"
#include <QObject>
#include "qmlnowplaying.h"
#include "../../main.h"
class QmlAbstractContent : public QObject
{
    Q_OBJECT
public:
    explicit QmlAbstractContent(QObject *parent = nullptr);

    void reset();
    void setCurrentlyPlaying(const lib::spt::track &track);
    virtual void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage);
    auto isCurrentlyPlaying() const -> bool;
public slots:
    void onSongMenu(const QPoint &pos);
signals:
protected:
    lib::spt::api &spotify;
    const lib::cache &cache;
    lib::settings &settings;
    QmlNowPlaying *nowPlaying = nullptr;
    bool isPlaying;
    virtual auto iconSize() const -> QSize = 0;
private:

};

#endif // QMLABSTRACTCONTENT_H
