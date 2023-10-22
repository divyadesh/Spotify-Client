#include "application.h"
#include "lib/time.hpp"
#include "lib/system.hpp"
#include "../util/font.hpp"
#include "../util/appconfig.hpp"
#include "qml/qmltracks.h"
#include <QTimer>
#ifdef _WIN32
#include "windows.h"
#endif

Application* Application::app;
SpotifyApplication *Application::spotifyApp;
QmlView *Application::qmlview;
QmlTracks *Application::qmlTracks;
Application::Application(QQmlApplicationEngine *engine,lib::settings &settings, lib::paths &paths,
                         lib::qt::http_client &httpClient, lib::spt::api &spotify,QObject *parent)
    : QObject{parent},
    spotify(spotify),
    settings(settings),
    paths(paths),
    cache(paths),
    httpClient(httpClient),
    httpClient2(httpClient),
    m_engine(engine)
{

    lib::log::warn("Contructor Created :: is another instance running?");
    lib::crash_handler::set_cache(cache);
    // Update player status
    auto *timer = new QTimer(this);
    QTimer::connect(timer, &QTimer::timeout, this, &Application::refresh);
    refresh();
    constexpr int tickMs = 1000;
    timer->start(tickMs);

    initClient();
    initMediaController();
    initMediaHotkeys();
    initDevice();

    setAppTitle(APP_NAME);
    QCoreApplication::instance()->installEventFilter(this);

}

void Application::createInstance(QQmlApplicationEngine *engine,lib::settings &settings, lib::paths &paths,
                                 lib::qt::http_client &httpClient, lib::spt::api &spotify) {
    Application::app = new Application(engine, settings, paths, httpClient, spotify);

    Application::spotifyApp = new SpotifyApplication(spotify,settings,httpClient,Application::app->getHttpClient2(),Application::app->getCache(),paths, Application::app);
    Application::qmlview = new QmlView(spotify,settings,Application::app->getCache(),httpClient);
    Application::qmlTracks = new QmlTracks(spotify,settings,Application::app->getCache(),httpClient,Application::getInstance());

    Application::app->getEngine()->rootContext()->setContextProperty("gSpotifyApp",Application::spotifyApp);
    Application::app->getEngine()->rootContext()->setContextProperty("gQmlView",Application::qmlview);
    Application::app->getEngine()->rootContext()->setContextProperty("gQmlTracks",Application::qmlTracks);


    QObject::connect(Application::app , &Application::playbackRefreshed,
                     Application::qmlview, &QmlView::onPlaybackRefreshed);
    QObject::connect(Application::app ,&Application::playbackRefreshed,Application::spotifyApp,&SpotifyApplication::onPlaybackRefreshed);
}

void Application::pauseMusic()
{
    static bool isPlaying = false;
    auto callback = [this](const std::string &status)
    {
        if (status.empty())
        {
            return;
        }
    };

    if (isPlaying)
    {
        isPlaying = false;
        spotify.resume(callback);
    }
    else
    {
        isPlaying = true;
        spotify.pause(callback);
    }
}

void Application::refresh()
{
    if (refreshCount < 0
        || ++refreshCount >= settings.general.refresh_interval
        || current.playback.progress_ms + lib::time::ms_in_sec > current.playback.item.duration)
    {
        spotify.current_playback([this](const lib::result<lib::spt::playback> &result)
                                 {
                                     if (result.success())
                                     {
                                         refreshed(result.value());
                                         refreshCount = 0;
                                         return;
                                     }

                                     if (!result.message().empty())
                                     {
                                         lib::log::error("Refresh failed: {}", result.message());
                                     }

                                     if (current.playback.is_playing)
                                     {
                                         current.playback.progress_ms += lib::time::ms_in_sec;
                                         refreshed(current.playback);
                                     }
                                 });
        return;
    }

    // Assume last refresh was 1 sec ago
    if (!current.playback.is_playing)
    {
        return;
    }
    current.playback.progress_ms += lib::time::ms_in_sec;
    refreshed(current.playback);
}

void Application::refreshed(const lib::spt::playback &playback)
{
    emit playbackRefreshed(playback, current.playback);

    const auto trackChange = current.playback.item.id != playback.item.id;
    current.playback = playback;

    if (!current.playback.item.is_valid())
    {
        setAppTitle(APP_NAME);
        return;
    }

    if (current.playback.device.is_valid())
    {
        settings.general.last_device = current.playback.device.id;
    }

    if (trackChange || appTitle() == APP_NAME)
    {
        setAppTitle(QString::fromStdString(playback.item.title()));
    }
}
void Application::initClient()
{
    if (!settings.spotify.start_client)
    {
        return;
    }

    if (settings.spotify.always_start)
    {
        startClient();
    }
    else
    {
        spotify.devices([this](const std::vector<lib::spt::device> &devices)
                        {
                            if (devices.empty())
                            {
                                this->startClient();
                            }
                        });
    }
}

auto Application::playback() const -> const lib::spt::playback &
{
    return current.playback;
}

auto Application::startClient()  -> const SpotifyClient::Runner *
{
    stopClient();
    spotifyRunner = new SpotifyClient::Runner(settings, paths, this);
    SpotifyClient::Runner::connect(spotifyRunner, &SpotifyClient::Runner::statusChanged,
                                   this, &Application::onSpotifyStatusChanged);

    spotifyRunner->start();
    return spotifyRunner;
}

auto Application::getCurrent() -> const spt::Current &
{
    return current;
}

auto Application::getSpotifyRunner() -> const SpotifyClient::Runner *
{
    return spotifyRunner;
}
auto Application::loadTracksFromCache(const std::string &id) -> std::vector<lib::spt::track>
{
    return cache.get_tracks(id);
}

void Application::saveTracksToCache(const std::string &id, const std::vector<lib::spt::track> &tracks)
{
    cache.set_tracks(id, tracks);
}
void Application::initMediaController()
{
#ifdef USE_DBUS
    if (!settings.general.media_controller)
    {
        return;
    }

    mediaPlayer = new mp::Service(spotify, this);
    // Check if something went wrong during init
    if (!mediaPlayer->isValid())
    {
        delete mediaPlayer;
        mediaPlayer = nullptr;
    }
#endif
}

void Application::initMediaHotkeys()
{
#ifdef _WIN32
    if (settings.general.media_hotkeys)
    {
        registerMediaHotkeys(true);
    }
#endif
}
#ifdef _WIN32
void Application::registerMediaHotkeys(bool enabled)
{

}
#endif

void Application::stopClient()
{
    spotifyRunner->deleteLater();
    spotifyRunner = nullptr;
}

void Application::initDevice()
{
    spotify.devices([this](const std::vector<lib::spt::device> &devices)
                    {
                        // Don't select a new device if one is currently active
                        for (const auto &device: devices)
                        {
                            if (device.is_active)
                            {
                                return;
                            }
                        }

                        if (devices.size() == 1
                            && lib::strings::starts_with(devices.front().name, APP_NAME))
                        {
                            spotify.set_device(devices.front(), {});
                        }
                        else
                        {
                            for (const auto &device: devices)
                            {
                                if (device.id == this->settings.general.last_device)
                                {
                                    spotify.set_device(device, {});
                                    break;
                                }
                            }
                        }
                    });
}

QString Application::appTitle() const
{
    return m_appTitle;
}

void Application::setAppTitle(const QString &newAppTitle)
{
    if (m_appTitle == newAppTitle)
        return;
    m_appTitle = newAppTitle;
    emit appTitleChanged();
}

void Application::onSpotifyStatusChanged(const QString &status)
{
    if ((Qt::WindowActive) > 0)
    {
        qDebug()<<"The Spotify Client Start Failed"<<status;
    }else{
        qDebug()<<"The Spotify Client Start Success"<<status;
    }
}
