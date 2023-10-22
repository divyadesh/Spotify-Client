#include "qmltrack.h"

qmltrack::qmltrack(QString title,QString artist,QString album,int length,bool added,bool liked,QObject *parent)
    : QObject{parent},
    m_title{title},
    m_artist{artist},
    m_album{album},
    m_length{length},
    m_added{added},
    m_liked{liked}
{

}

QString qmltrack::title() const
{
    return m_title;
}

void qmltrack::setTitle(const QString &newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged();
}

QString qmltrack::artist() const
{
    return m_artist;
}

void qmltrack::setArtist(const QString &newArtist)
{
    if (m_artist == newArtist)
        return;
    m_artist = newArtist;
    emit artistChanged();
}

QString qmltrack::album() const
{
    return m_album;
}

void qmltrack::setAlbum(const QString &newAlbum)
{
    if (m_album == newAlbum)
        return;
    m_album = newAlbum;
    emit albumChanged();
}

int qmltrack::length() const
{
    return m_length;
}

void qmltrack::setLength(int newLength)
{
    if (m_length == newLength)
        return;
    m_length = newLength;
    emit lengthChanged();
}

bool qmltrack::added() const
{
    return m_added;
}

void qmltrack::setAdded(bool newAdded)
{
    if (m_added == newAdded)
        return;
    m_added = newAdded;
    emit addedChanged();
}

bool qmltrack::liked() const
{
    return m_liked;
}

void qmltrack::setLiked(bool newLiked)
{
    if (m_liked == newLiked)
        return;
    m_liked = newLiked;
    emit likedChanged();
}
