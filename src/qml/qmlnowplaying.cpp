#include "qmlnowplaying.h"

QmlNowPlaying::QmlNowPlaying(QObject *parent)
    : QObject{parent}
{
     m_artist = "";
     m_name = "";
}

void QmlNowPlaying::setTrack(const lib::spt::track &track)
{
//     setName(track.name);
//     const auto names = lib::spt::entity::combine_names(track.artists);

     //setArtist(names);
}

void QmlNowPlaying::setNoPlaying()
{

     QString art = "No music playing";
    // setArtist(art);

}
QString QmlNowPlaying::artist() const
{
    return m_artist;
}

void QmlNowPlaying::setArtist(const QString &newArtist)
{
    if (m_artist == newArtist)
        return;
    m_artist = newArtist;
    emit artistChanged();
}

QString QmlNowPlaying::name() const
{
    return m_name;
}

void QmlNowPlaying::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}
