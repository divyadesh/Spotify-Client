#include "qmltitle.h"
#include "application.h"
QmlTitle::QmlTitle(lib::spt::api &spotify, const lib::cache &cache,QObject *parent)
    : QObject{parent},
    spotify(spotify),
    cache(cache)
{
    info = new QmlTitleInfo(spotify, this);
    Application *mainWindow = Application::getInstance();
    QObject::connect(mainWindow, &Application::playbackRefreshed,
                     this, &QmlTitle::onPlaybackRefreshed);
}
void QmlTitle::onPlaybackRefreshed(const lib::spt::playback &playback,
                                   const lib::spt::playback &/*previous*/)
{
    auto callback = [this](const std::string &currentName)
    {
        auto size = 0;
        if (this->info != nullptr)
        {
            this->setTitle(QString::fromStdString(currentName));
        }

        auto show = currentName != "No context";

    };

    if (playback.context.type.empty()
        || playback.context.uri.empty())
    {
        callback("No context");
    }
    else if (playback.context.type == "album")
    {
        callback(playback.item.album.name);
    }
    else if (playback.context.type == "artist")
    {
        auto name = playback.item.artists.empty()
                        ? std::string()
                        : playback.item.artists.front().name;

        auto id = lib::spt::uri_to_id(playback.context.uri);
        for (const auto &artist: playback.item.artists)
        {
            if (artist.id == id)
            {
                name = artist.name;
            }
        }

        callback(name);
    }
    else
    {
        //playlistName(playback.context.uri, callback);
    }
}

auto QmlTitle::playlist(const std::string &id) -> lib::spt::playlist
{
    for (const auto &playlist: cache.get_playlists())
    {
        if (lib::strings::ends_with(id, playlist.id))
        {
            return playlist;
        }
    }

    return {};
}

auto QmlTitle::playlistNameFromSaved(const std::string &id) -> std::string
{
    return playlist(id).name;
}

void QmlTitle::playlistName(const std::string &id, lib::callback<std::string> &callback)
{
    const auto &name = playlistNameFromSaved(id);
    if (!name.empty())
    {
        callback(name);
    }
    else
    {
        spotify.playlist(lib::spt::uri_to_id(id),
                         [callback](const lib::spt::playlist &playlist)
                         {
                             callback(playlist.name);
                         });
    }
}

QString QmlTitle::title() const
{
    return m_title;
}

void QmlTitle::setTitle(const QString &newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged();
}
