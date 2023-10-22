#include "qmlview.h"
#include"application.h"
#include <QBuffer>
#include "application.h"
QmlView::QmlView(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
                 const lib::http_client &httpClient,QObject *parent)
    : QObject{parent},
    spotify(spotify),
    settings(settings),
    cache(cache),
    httpClient(httpClient)
{
    m_isCurrentPlaying = false;
    setAlbumSize(settings.qt().album_size);
    title = new QmlTitle(spotify, cache, this);
    Application *mainWindow = Application::getInstance();

    QObject::connect(mainWindow, &Application::playbackRefreshed,
                     this, &QmlView::onPlaybackRefreshed);
}

void QmlView::setAlbumSize(lib::album_size albumSize)
{
    //    if (albumContent != nullptr)
    //    {
    //        albumContent->deleteLater();
    //    }

    //    if (albumSize == lib::album_size::expanded)
    //    {
    //        albumContent = new Context::ExpandedContent(spotify, cache, settings, this);
    //    }
    //    else
    //    {
    //        albumContent = new Context::SmallContent(spotify, cache, settings, this);
    //    }

    //    setWidget(albumContent);
}
const QString picture(QImage myImage)
{
    // Some init code to setup the image (e.g. loading a PGN/JPEG, etc.)
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    myImage.save(&buffer, "JPEG");

    QString image("data:image/jpg;base64,");
    image.append(QString::fromLatin1(bArray.toBase64().data()));

    return image;
}
void QmlView::setAlbumImage(const lib::spt::entity &albumEntity,
                            const std::string &albumImageUrl)
{
    Http::getAlbumImage(albumImageUrl, httpClient, cache, [this, albumEntity](const QPixmap &image)
                        {
                            const auto albumShape = settings.qt().album_shape;
                            QPixmap qmlPixmap = Image::mask(image, albumShape);
                            QString setToolTip = QString::fromStdString(albumEntity.name);
                            QImage imageIcon = qmlPixmap.toImage();
                            setCurrentImage(picture(imageIcon));
                            setSongDetails(QString::fromStdString(albumEntity.name));
                        });
}
void QmlView::reset()
{
    //albumContent->reset();
}

void QmlView::onPlaybackRefreshed(const lib::spt::playback &current, const lib::spt::playback &previous)
{
    if (!current.is_valid())
    {
        reset();
        return;
    }

    if (current.item.id != previous.item.id
        || !m_isCurrentPlaying
        || Application::getInstance()->appTitle() == QStringLiteral(APP_NAME))
    {
        const auto &albumImageUrl = settings.qt().album_size == lib::album_size::expanded
                                        ? current.item.image_large()
                                        : current.item.image_small();

        setAlbumImage(current.item.album, albumImageUrl);
    }
    setIsCurrentPlaying(current.is_playing);
    artists = current.item.artists;
    QString allArtists;
    for(auto &artist : artists){
        allArtists.append(QString::fromStdString(artist.name)).append(", ");
    }
    setArtistsName(allArtists);
}

void QmlView::loadArtistById(std::string artistId)
{
    spotify.artist(artistId, [this](const lib::spt::artist &loadedArtist)
                   {

                       artistLoaded(loadedArtist);

                   });
}

void QmlView::artistLoaded(const lib::spt::artist &loadedArtist)
{
    artist = loadedArtist;

    // Get cover image
    if (artist.image.empty())
    {
        //coverLabel->setJpeg({});
    }
    else
    {
        httpClient.get(artist.image, lib::headers(),
                       [this](const std::string &data)
                       {
                           //coverLabel->setJpeg(QByteArray::fromStdString(data));
                       });
    }

    // Artist name title
   //name->setText(QString::fromStdString(artist.name));

    // Artist is loaded now at least
    //context->setArtist(artist);

    // Genres
    //genres->setText(QString::fromStdString(lib::strings::join(artist.genres, ", ")));

    // Top tracks
    spotify.top_tracks(artist, [this](const std::vector<lib::spt::track> &tracks)
                       {
                           //topTracksLoaded(tracks);
                       });

    // Albums
    spotify.albums(artist, [this](const std::vector<lib::spt::album> &albums)
                   {
                       //albumList->setAlbums(albums);
                   });

    // Related artists
    spotify.related_artists(artist, [this](const std::vector<lib::spt::artist> &artists)
                            {
                               // relatedArtistsLoaded(artists);
    });
}

void QmlView::topLoad()
{
    lib::spt::entity &entity = artists[0];
    auto callback = [this, entity](const std::vector<lib::spt::track> &tracks)
    {
        //tracksLoaded(entity, tracks);
        for(auto &track:tracks){
            qDebug()<<"The Top Track ::"<<QString::fromStdString(track.title());
        }
    };

//    if (item->text(0) == recentlyPlayed)
//    {
//        spotify.recently_played(callback);
//    }
//    else if (item->text(0) == savedTracks)
//    {
//        spotify.saved_tracks(callback);
//    }
//    else if (item->text(0) == topTracks)
//    {
//        spotify.top_tracks(callback);
//    }

    spotify.top_tracks(callback);
}


QString QmlView::currentImage() const
{
    return m_currentImage;
}

void QmlView::setCurrentImage(const QString &newCurrentImage)
{
    if (m_currentImage == newCurrentImage)
        return;
    m_currentImage = newCurrentImage;
    emit currentImageChanged();
}

bool QmlView::isCurrentPlaying() const
{
    return m_isCurrentPlaying;
}

void QmlView::setIsCurrentPlaying(bool newIsCurrentPlaying)
{
    if (m_isCurrentPlaying == newIsCurrentPlaying)
        return;
    m_isCurrentPlaying = newIsCurrentPlaying;
    emit isCurrentPlayingChanged();
}

QString QmlView::songDetails() const
{
    return m_songDetails;
}

void QmlView::setSongDetails(const QString &newSongDetails)
{
    if (m_songDetails == newSongDetails)
        return;
    m_songDetails = newSongDetails;
    emit songDetailsChanged();
}

QString QmlView::artistsName() const
{
    return m_artistsName;
}

void QmlView::setArtistsName(const QString &newArtistsName)
{
    if (m_artistsName == newArtistsName)
        return;
    m_artistsName = newArtistsName;
    emit artistsNameChanged();
}
