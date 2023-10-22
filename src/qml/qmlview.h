#ifndef QMLVIEW_H
#define QMLVIEW_H
#pragma once
#include <QObject>
#include "qmltitle.h"
#include "qmlabstractcontent.h"
#include "lib/settings.hpp"
#include "lib/enum/albumsize.hpp"
#include "../../main.h"
#include <QJsonObject>
class QmlView : public QObject
{
    Q_OBJECT
public:
    explicit QmlView(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
                     const lib::http_client &httpClient,QObject *parent = nullptr);
    Q_PROPERTY(QString currentImage READ currentImage WRITE setCurrentImage NOTIFY currentImageChanged FINAL)
    Q_PROPERTY(bool isCurrentPlaying READ isCurrentPlaying WRITE setIsCurrentPlaying NOTIFY isCurrentPlayingChanged FINAL)
    Q_PROPERTY(QString songDetails READ songDetails WRITE setSongDetails NOTIFY songDetailsChanged FINAL)
    Q_PROPERTY(QString artistsName READ artistsName WRITE setArtistsName NOTIFY artistsNameChanged FINAL)

    void setAlbumSize(lib::album_size albumSize);
    void reset();
    Q_INVOKABLE void artistLoaded(const lib::spt::artist &loadedArtist);
    Q_INVOKABLE void topLoad();

    QString currentImage() const;

    bool isCurrentPlaying() const;

    QString songDetails() const;

    QString artistsName() const;

    static constexpr const int dataRole = 0x100;
    static constexpr const char *followedArtists = "Followed Artists";
    static constexpr const char *newReleases = "New Releases";
    static constexpr const char *recentlyPlayed = "History";
    static constexpr const char *savedAlbums = "Liked Albums";
    static constexpr const char *savedTracks = "Liked Tracks";
    static constexpr const char *topArtists = "Top Artists";
    static constexpr const char *topTracks = "Top Tracks";

public slots:
    void setArtistsName(const QString &newArtistsName);
    void setSongDetails(const QString &newSongDetails);
    void setIsCurrentPlaying(bool newIsCurrentPlaying);
    void setCurrentImage(const QString &newCurrentImage);
    void onPlaybackRefreshed(const lib::spt::playback &current,
                             const lib::spt::playback &previous);
signals:

    void currentImageChanged();

    void isCurrentPlayingChanged();

    void songDetailsChanged();

    void artistsNameChanged();

private:
    std::vector<lib::spt::entity> artists;
    lib::spt::artist artist;
    std::string artistId;
    void loadArtistById(std::string artistId);
    QmlTitle *title = nullptr;
    QmlAbstractContent *albumContent = nullptr;
    lib::spt::api &spotify;
    lib::settings &settings;
    lib::cache &cache;
    const lib::http_client &httpClient;
    void setAlbumImage(const lib::spt::entity &albumEntity, const std::string &albumImageUrl);
    QString m_currentImage;
    bool m_isCurrentPlaying;
    QString m_songDetails{QString()};
    QString m_artistsName;
};

#endif // QMLVIEW_H
