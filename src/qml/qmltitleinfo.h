#ifndef QMLTITLEINFO_H
#define QMLTITLEINFO_H
#pragma once
#include <QObject>
#include "lib/spotify/api.hpp"
#include "../../main.h"
class QmlTitleInfo : public QObject
{
    Q_OBJECT
public:
    explicit QmlTitleInfo(lib::spt::api &spotify,QObject *parent = nullptr);

    auto getIcon() const -> QIcon;
public slots:
    void onPlaybackRefreshed(const lib::spt::playback &refreshed,
                             const lib::spt::playback &previous);
signals:
private:
    lib::spt::api &spotify;
    lib::spt::playback playback;

//    void onContextMenu(const QPoint &pos);
//    void onContextMenuTriggered(bool checked);
};

#endif // QMLTITLEINFO_H
