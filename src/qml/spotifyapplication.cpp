#include "spotifyapplication.h"
#include "util/shortcut.hpp"
#include "util/appconfig.hpp"

SpotifyApplication::SpotifyApplication(lib::spt::api &spotify, lib::settings &settings,
                                       lib::http_client &httpClient,lib::qt::http_client &httpClient2, lib::cache& cache,lib::paths &paths,QObject *parent)
    : QObject{parent},
    spotify(spotify),
    settings(settings),
    paths(paths),
    httpClient(httpClient),
    httpClient2(httpClient2)
{

}

void SpotifyApplication::playPause()
{
    static bool isPlaying = false;
    auto callback = [this](const std::string &status)
    {
        if (status.empty())
        {
            return;
        }
        qDebug()<<QString("Failed to %1 playback: %2").arg(isPlaying ? QStringLiteral("pause") : QStringLiteral("resume"), QString::fromStdString(status));
    };

    if (!isPlaying)
    {
        isPlaying  = true;
        spotify.resume(callback);
    }
    else
    {
        isPlaying  = false;
        spotify.pause(callback);
    }
}

void SpotifyApplication::nextSong()
{
    spotify.next([this](const std::string &status)
                 {
                     if (!status.empty())
                     {
                         qDebug()<<QString("Failed to go to next track: %1")
                                         .arg(QString::fromStdString(status));
                         return;
                     }
                 });
}

void SpotifyApplication::previousSong()
{
    this->spotify.previous([this](const std::string &status)
                           {
                               if (!status.empty())
                               {
                                   qDebug()<<QString("Failed to go to previous track: %1")
                                                   .arg(QString::fromStdString(status));
                                   return;
                               }
                           });
}

void SpotifyApplication::onShuffle(bool checked)
{
    Application* app = Application::getInstance();
    auto current = app->playback();

    current.shuffle = !current.shuffle;
    app->refreshed(current);

    spotify.set_shuffle(checked, [](const std::string &status)
                        {
                            if (status.empty())
                            {
                                return;
                            }

                            qDebug()<<QString("Failed to toggle shuffle: %1")
                                            .arg(QString::fromStdString(status));
                        });
}

void SpotifyApplication::onRepeat(bool checked)
{

    Application* app = Application::getInstance();
    auto current = app->playback();

    const auto repeatMode = toggleRepeat(current);
    current.repeat = repeatMode;
    app->refreshed(current);

    spotify.set_repeat(repeatMode, [](const std::string &status)
                       {
                           if (status.empty())
                           {
                               return;
                           }

                           qDebug()<<QString("Failed to toggle repeat: %1")
                                           .arg(QString::fromStdString(status));
                       });
}

void SpotifyApplication::onPlaybackRefreshed(const lib::spt::playback &current, const lib::spt::playback &previousPlayback)
{
    if (!current.is_valid())
    {
        setIsPlaying(false);
        return;
    }
    setProgress(current);
    setIsPlaying(current.is_playing);

    constexpr int volumeStep = 5;
    setVolume(current.volume() / volumeStep);

    setRepeat(current.repeat);
    setShuffle(current.shuffle);
}

QString SpotifyApplication::getRepeatIcon(lib::repeat_state repeatState)
{
    if (repeatState == lib::repeat_state::track)
    {
        return Icon::getQString(QStringLiteral("media-playlist-repeat-song"));
    }

    return Icon::getQString(QStringLiteral("media-playlist-repeat"));
}

void SpotifyApplication::setProgress(int current, int duration)
{
    setCurrent(current);
    setTotal(duration);

    setValue(current);
    setMaximum(duration);
}

auto SpotifyApplication::getNextRepeatState(const lib::spt::playback &playback) -> lib::repeat_state
{
    switch (m_repeat)
    {
    case lib::repeat_state::off:
        return playback.is_allowed(lib::player_action::toggling_repeat_context)
                   ? lib::repeat_state::context
                   : lib::repeat_state::track;

    case lib::repeat_state::context:
        return playback.is_allowed(lib::player_action::toggling_repeat_track)
                   ? lib::repeat_state::track
                   : lib::repeat_state::off;

    default:
        return lib::repeat_state::off;
    }
}

bool SpotifyApplication::isPlaying() const
{
    return m_isPlaying;
}

auto SpotifyApplication::toggleRepeat(const lib::spt::playback &playback) -> lib::repeat_state
{
    const auto newState = getNextRepeatState(playback);
    setRepeat(newState);
    return newState;
}

void SpotifyApplication::setIsPlaying(bool newIsPlaying)
{
    if (m_isPlaying == newIsPlaying)
        return;
    m_isPlaying = newIsPlaying;
    emit isPlayingChanged();
}

lib::repeat_state SpotifyApplication::repeat() const
{
    return m_repeat;
}

void SpotifyApplication::setRepeat(lib::repeat_state newRepeat)
{
    setRepeatIcon(getRepeatIcon(newRepeat));
    if (m_repeat == newRepeat)
        return;
    m_repeat = newRepeat;
    emit repeatChanged();
}

bool SpotifyApplication::shuffle() const
{
    return m_shuffle;
}

void SpotifyApplication::setShuffle(bool newShuffle)
{
    if (m_shuffle == newShuffle)
        return;
    m_shuffle = newShuffle;
    emit shuffleChanged();
}

int SpotifyApplication::volume() const
{
    return m_volume;
}

void SpotifyApplication::setProgress(const lib::spt::playback &playback)
{
    setProgress(playback.progress_ms, playback.item.duration);
}

void SpotifyApplication::setVolume(int newVolume)
{
    if (m_volume == newVolume)
        return;
    m_volume = newVolume;
    emit volumeChanged();
}

int SpotifyApplication::minimum() const
{
    return m_minimum;
}

void SpotifyApplication::setMinimum(int newMinimum)
{
    if (m_minimum == newMinimum)
        return;
    m_minimum = newMinimum;
    emit minimumChanged();
}

int SpotifyApplication::maximum() const
{
    return m_maximum;
}

void SpotifyApplication::setMaximum(int newMaximum)
{
    if (m_maximum == newMaximum)
        return;
    m_maximum = newMaximum;
    emit maximumChanged();
}

int SpotifyApplication::value() const
{
    return m_value;
}

void SpotifyApplication::setValue(int newValue)
{
    if (m_value == newValue)
        return;
    m_value = newValue;
    emit valueChanged();
}

int SpotifyApplication::total() const
{
    return m_total;
}

void SpotifyApplication::setTotal(int newTotal)
{
    if (m_total == newTotal)
        return;
    m_total = newTotal;
    emit totalChanged();
}

int SpotifyApplication::current() const
{
    return m_current;
}

void SpotifyApplication::setCurrent(int newCurrent)
{
    if (m_current == newCurrent)
        return;
    m_current = newCurrent;
    emit currentChanged();
}

QString SpotifyApplication::repeatIcon() const
{
    return m_repeatIcon;
}

void SpotifyApplication::setRepeatIcon(const QString &newRepeatIcon)
{
    if (m_repeatIcon == newRepeatIcon)
        return;
    m_repeatIcon = newRepeatIcon;
    emit repeatIconChanged();
}

QString SpotifyApplication::shuffleIcon() const
{
    return m_shuffleIcon;
}

void SpotifyApplication::setShuffleIcon(const QString &newShuffleIcon)
{
    if (m_shuffleIcon == newShuffleIcon)
        return;
    m_shuffleIcon = newShuffleIcon;
    emit shuffleIconChanged();
}
