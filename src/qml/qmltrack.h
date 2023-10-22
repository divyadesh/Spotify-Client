#ifndef QMLTRACK_H
#define QMLTRACK_H

#include <QObject>

class qmltrack : public QObject
{
    Q_OBJECT
public:
    explicit qmltrack(QString title,QString artist,QString album,int length,bool added,bool liked,QObject *parent = nullptr);

    /*
    roles[TitleRole] = "title";
    roles[ArtistRole] = "artist";
    roles[AlbumRole] = "album";
    roles[LengthRole] = "length";
    roles[AddedRole] = "added";
    roles[LikedRole] = "liked";

    */
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ artist WRITE setArtist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ album WRITE setAlbum NOTIFY albumChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(bool added READ added WRITE setAdded NOTIFY addedChanged)
    Q_PROPERTY(bool liked READ liked WRITE setLiked NOTIFY likedChanged)

    QString title() const;

    QString artist() const;

    QString album() const;

    int length() const;

    bool added() const;

    bool liked() const;

public slots:
    void setTitle(const QString &newTitle);
    void setArtist(const QString &newArtist);
    void setAlbum(const QString &newAlbum);
    void setLength(int newLength);
    void setLiked(bool newLiked);
    void setAdded(bool newAdded);
signals:
    void titleChanged();

    void artistChanged();

    void albumChanged();

    void lengthChanged();

    void addedChanged();

    void likedChanged();

private:
    QString m_title;
    QString m_artist;
    QString m_album;
    int m_length;
    bool m_added;
    bool m_liked;
};

#endif // QMLTRACK_H
