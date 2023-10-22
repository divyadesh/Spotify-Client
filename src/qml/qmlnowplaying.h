#ifndef QMLNOWPLAYING_H
#define QMLNOWPLAYING_H
#pragma once

#include "lib/spotify/track.hpp"
#include <QObject>
#include "../../main.h"
class QmlNowPlaying : public QObject
{
    Q_OBJECT
public:
    explicit QmlNowPlaying(QObject *parent = nullptr);

    Q_PROPERTY(QString artist READ artist WRITE setArtist NOTIFY artistChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    void setTrack(const lib::spt::track &track);
    void setNoPlaying();

    QString artist() const;
    QString name() const;

public slots:
    void setArtist(const QString &newArtist);
    void setName(const QString &newName);
signals:

    void artistChanged();
    void nameChanged();

private:
    static constexpr float nameScale = 1.1F;
    static constexpr float artistScale = 0.9F;

    QString m_artist;
    QString m_name;
};

#endif // QMLNOWPLAYING_H
