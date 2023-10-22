#include "qmltitleinfo.h"
#include "lib/developermode.hpp"
#include "util/icon.hpp"
#include "application.h"
QmlTitleInfo::QmlTitleInfo(lib::spt::api &spotify,QObject *parent)
    : QObject{parent},
    spotify(spotify)
{


    Application *mainWindow = Application::getInstance();
    QObject::connect(mainWindow, &Application::playbackRefreshed,
                        this, &QmlTitleInfo::onPlaybackRefreshed);
}
auto QmlTitleInfo::getIcon() const -> QIcon
{
    return Icon::getByType(playback.context.type);
}

void QmlTitleInfo::onPlaybackRefreshed(const lib::spt::playback &refreshed,
                                             const lib::spt::playback &/*previous*/)
{
    playback = refreshed;
}
