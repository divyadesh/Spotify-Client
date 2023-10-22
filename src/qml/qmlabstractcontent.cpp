#include "qmlabstractcontent.h"
#include "application.h"
QmlAbstractContent::QmlAbstractContent(QObject *parent)
    : QObject{parent},
    spotify(spotify),
    cache(cache),
    settings(settings)
{

}

void QmlAbstractContent::setAlbum(const lib::spt::entity &albumEntity,
                                        const QPixmap &albumImage)
{
    qDebug()<<"Set pixel Map"<<QString::fromStdString(albumEntity.name);
//    if (album != nullptr)
//    {
//        const auto albumShape = settings.qt().album_shape;
//        album->setPixmap(Image::mask(albumImage, albumShape));
//        album->setToolTip(QString::fromStdString(albumEntity.name));
//    }
}

void QmlAbstractContent::onSongMenu(const QPoint &pos)
{
    Application *mainWindow = Application::getInstance();
    if (mainWindow == nullptr)
    {
        return;
    }

    auto track = mainWindow->playback().item;
    if (track.name.empty()
        && track.artists.empty())
    {
        return;
    }

//    auto *menu = new Menu::Track(track, spotify, cache, parentWidget());
//    menu->popup(mapToGlobal(pos));
}

void QmlAbstractContent::reset()
{
    isPlaying = false;

//    if (album != nullptr)
//    {
//        album->setPixmap(Icon::get("media-optical-audio").pixmap(iconSize()));
//    }

    if (nowPlaying != nullptr)
    {
        nowPlaying->setNoPlaying();
    }
}

void QmlAbstractContent::setCurrentlyPlaying(const lib::spt::track &track)
{
    isPlaying = true;

    if (nowPlaying != nullptr)
    {
        nowPlaying->setTrack(track);
    }
}

auto QmlAbstractContent::isCurrentlyPlaying() const -> bool
{
    return isPlaying;
}
