#ifndef QMLTRACKS_H
#define QMLTRACKS_H

#include <QAbstractListModel>
#include <QList>
#pragma once

#include "lib/cache.hpp"
#include "lib/set.hpp"
#include "spotify/current.hpp"
#include "qmltrack.h"
#include "../../main.h"
class QmlTracks : public QAbstractListModel
{
    enum TrackRole{
        TitleRole = Qt::UserRole+1,
        ArtistRole,
        AlbumRole,
        LengthRole,
        AddedRole,
        LikedRole
    };

public:
    explicit QmlTracks(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
                       const lib::http_client &httpClient,QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int,QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void addTrack(qmltrack* track);
    void setPlayingTrackItem(const std::string &itemId);

    /**
         * Load tracks directly, without cache, but select an item
         * @deprecated Use with page instead
         */
    void load(const std::vector<lib::spt::track> &tracks, const std::string &selectedId);

    /**
         * Load tracks directly, without cache,
         * but select an item and provide a fallback added date
         * @deprecated Use with page instead
         */
    void load(const std::vector<lib::spt::track> &tracks, const std::string &selectedId,
              const std::string &addedAt);

    /**
         * Load tracks directly, without cache
         * @deprecated Use with page instead
         */
    void load(const std::vector<lib::spt::track> &tracks);

    /**
         * Load tracks from page into UI
         * @param page Current page
         * @param selectedId ID of item to select
         * @param addedAt Fallback added date
         * @return If next page is needed
         */
    auto load(const lib::spt::page<lib::spt::track> &page,
              const std::string &selectedId = {}, const std::string &addedAt = {}) -> bool;

    /**
         * Load playlist first from cache, then refresh it
         */
    void load(const lib::spt::playlist &playlist);

    /** Force refresh tracks in playlist */
    void refreshPlaylist(const lib::spt::playlist &playlist);

    /**
         * Load album first from cache, then refresh it
         * @param album Album to load
         * @param trackId Track to highlight
         */
    void load(const lib::spt::album &album, const std::string &trackId = std::string());

signals:
public slots:
    void onPlaybackRefreshed(const lib::spt::playback &current,
                                        const lib::spt::playback &previous);
private:
    bool refreshing = false;
    lib::settings &settings;
    lib::cache &cache;
    lib::spt::api &spotify;
    QList<qmltrack*> m_track;
};

#endif // QMLTRACKS_H
