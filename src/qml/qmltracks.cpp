#include "qmltracks.h"
#include "application.h"
QmlTracks::QmlTracks(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
                     const lib::http_client &httpClient,QObject *parent)
    : QAbstractListModel{parent},
    settings(settings),
    cache(cache),
    spotify(spotify)
{
    auto *mainWindow = Application::getInstance();
    QObject::connect(mainWindow, &Application::playbackRefreshed,
                        this, &QmlTracks::onPlaybackRefreshed);
}

int QmlTracks::rowCount(const QModelIndex &parent) const
{
    return m_track.size();
}

QHash<int, QByteArray> QmlTracks::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[ArtistRole] = "artist";
    roles[AlbumRole] = "album";
    roles[LengthRole] = "length";
    roles[AddedRole] = "added";
    roles[LikedRole] = "liked";
    return roles;
}

QVariant QmlTracks::data(const QModelIndex &index, int role) const
{
    if(index.row()> m_track.size() || !index.isValid()){
        return QVariant();
    }
    qmltrack *data = m_track[index.row()];
    switch (role) {
    case TitleRole:
        return data->title();
        break;
    case ArtistRole:
        return data->artist();
        break;
    case AlbumRole:
        return data->album();
        break;
    case LengthRole:
        return data->length();
        break;
    case AddedRole:
        return data->added();
        break;
    case LikedRole:
        return data->liked();
        break;
    default:
        break;
    }
    return QVariant();
}

bool QmlTracks::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool isChanged = false;
    qmltrack *track = m_track[index.row()];
    switch (role) {
    case TitleRole:
        if(track->title() != value.toString()){
            isChanged = true;
            track->setTitle(value.toString());
        }
        break;
    case ArtistRole:
        if(track->artist() != value.toString()){
            isChanged = true;
            track->setArtist(value.toString());
        }
        break;
    case AlbumRole:
        if(track->album() != value.toString()){
            isChanged = true;
            track->setAlbum(value.toString());
        }
        break;
    case LengthRole:
        if(track->length() != value.toInt()){
            isChanged = true;
            track->setLength(value.toInt());
        }
        break;
    case AddedRole:
        if(track->added() != value.toBool()){
            isChanged = true;
            track->setAdded(value.toBool());
        }
        break;
    case LikedRole:
        if(track->liked() != value.toBool()){
            isChanged = true;
            track->setLiked(value.toBool());
        }
        break;
    default:
        break;
    }
    if(isChanged){
        emit dataChanged(index,index,QVector<int>()<<role);
    }
    return isChanged;
}

Qt::ItemFlags QmlTracks::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEditable;
}

void QmlTracks::addTrack(qmltrack *track)
{
    const int size = m_track.size();
    beginInsertRows(QModelIndex(),size,size);
    m_track.append(track);
    endInsertRows();
}

void QmlTracks::setPlayingTrackItem(const std::string &itemId)
{

}

void QmlTracks::load(const std::vector<lib::spt::track> &tracks, const std::string &selectedId)
{
    load(tracks, selectedId, std::string());
}

void QmlTracks::load(const std::vector<lib::spt::track> &tracks, const std::string &selectedId, const std::string &addedAt)
{
    lib::spt::page<lib::spt::track> page;
    page.items = tracks;
    page.total = static_cast<int>(tracks.size());

    load(page, selectedId, addedAt);
}

void QmlTracks::load(const std::vector<lib::spt::track> &tracks)
{
    load(tracks, std::string());
}

bool QmlTracks::load(const lib::spt::page<lib::spt::track> &page, const std::string &selectedId, const std::string &addedAt)
{
    return false;
}

void QmlTracks::load(const lib::spt::playlist &playlist)
{
    Application *mainWindow = Application::getInstance();

    const auto &tracks = playlist.tracks.empty()
                             ? cache.get_playlist(playlist.id).tracks
                             : playlist.tracks;

    if (!tracks.empty())
    {
        load(tracks);
    }
    else
    {
        //setEnabled(false);
    }

    const auto &snapshot = playlist.snapshot;
    spotify.playlist(playlist.id,
                     [this, snapshot, mainWindow](const lib::spt::playlist &loadedPlaylist)
                     {
                         const auto &currentUser = mainWindow != nullptr
                                                       ? lib::spt::user()/*mainWindow->getCurrentUser()*/
                                                       : lib::spt::user();

                         if (loadedPlaylist.is_up_to_date(snapshot, currentUser))
                         {
                             return;
                         }
                         this->refreshPlaylist(loadedPlaylist);
                     });

    if (mainWindow != nullptr)
    {
       // mainWindow->history()->push(playlist);
    }

    settings.general.last_playlist = playlist.id;
    settings.save();
}

void QmlTracks::refreshPlaylist(const lib::spt::playlist &playlist)
{

}

void QmlTracks::load(const lib::spt::album &album, const std::string &trackId)
{
    auto tracks = cache.get_tracks(album.id);
    if (!tracks.empty())
    {
        load(tracks, trackId, album.release_date);
    }
    else
    {
       // setEnabled(false);
    }

    auto *mainWindow = Application::getInstance();
    if (mainWindow != nullptr)
    {
       // mainWindow->history()->push(album);
    }

    spotify.album_tracks(album,
                         [this, album, trackId](const std::vector<lib::spt::track> &tracks)
                         {
                             this->load(tracks, trackId, album.release_date);
                             //this->setEnabled(true);

                             cache.set_tracks(album.id, tracks);
                             cache.set_album(album);
                         });
}

void QmlTracks::onPlaybackRefreshed(const lib::spt::playback &current,
                                    const lib::spt::playback &previous)
{
    if (current.item.id != previous.item.id && current.is_playing)
    {
        setPlayingTrackItem(current.item.id);
    }

//    Application *mainWindow = Application::getInstance();
//    const auto &type = current.context.type;
//    const auto uri = lib::strings::split(current.context.uri, ':').back();

//    qDebug()<<"This is type ::"<<QString::fromStdString(type);
//    if (type == "album")
//    {
//        //mainWindow->loadAlbum(uri);
//    }
//    else if (type == "artist")
//    {
//        //mainWindow->openArtist(uri);
//    }
//    else if (type == "playlist")
//    {
//        spotify.playlist(uri, [mainWindow](const lib::spt::playlist &playlist)
//                         {
//                             //mainWindow->resetLibraryPlaylist();
//                             Application::getTracks()->load(playlist);
//                         });
//    }
}
