#ifndef SPOTIFYAPPLICATION_H
#define SPOTIFYAPPLICATION_H

#pragma once
#include "lib/settings.hpp"
#include <QObject>
#include "application.h"
#include "util/icon.hpp"
#include "../../main.h"
class SpotifyApplication : public QObject
{
    Q_OBJECT
public:

    explicit SpotifyApplication(lib::spt::api &spotify, lib::settings &settings,
                                lib::http_client &httpClient,lib::qt::http_client &httpClient2, lib::cache& cache,lib::paths &paths,QObject *parent = nullptr);
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(lib::repeat_state repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged FINAL)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged FINAL)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged FINAL)
    Q_PROPERTY(int total READ total WRITE setTotal NOTIFY totalChanged FINAL)
    Q_PROPERTY(int current READ current WRITE setCurrent NOTIFY currentChanged FINAL)


    Q_PROPERTY(QString repeatIcon READ repeatIcon WRITE setRepeatIcon NOTIFY repeatIconChanged FINAL)
    Q_PROPERTY(QString shuffleIcon READ shuffleIcon WRITE setShuffleIcon NOTIFY shuffleIconChanged FINAL)

    Q_INVOKABLE void playPause();
    Q_INVOKABLE void nextSong();
    Q_INVOKABLE void previousSong();
    Q_INVOKABLE void onShuffle(bool checked);
    Q_INVOKABLE void onRepeat(bool checked);

    bool isPlaying() const;
    auto toggleRepeat(const lib::spt::playback &playback) -> lib::repeat_state;
    lib::repeat_state repeat() const;
    bool shuffle() const;
    int volume() const;

    int minimum() const;

    int maximum() const;

    int value() const;

    int total() const;

    int current() const;

    QString repeatIcon() const;

    QString shuffleIcon() const;

public slots:
    void setRepeatIcon(const QString &newRepeatIcon);
    void setShuffleIcon(const QString &newShuffleIcon);
    void setMinimum(int newMinimum);
    void setMaximum(int newMaximum);
    void setValue(int newValue);
    void setTotal(int newTotal);
    void setCurrent(int newCurrent);
    void setProgress(const lib::spt::playback &playback);
    void setShuffle(bool newShuffle);
    void setVolume(int newVolume);
    void setRepeat(lib::repeat_state newRepeat);
    void setIsPlaying(bool newIsPlaying);
    void onPlaybackRefreshed(const lib::spt::playback &current,
                             const lib::spt::playback &previousPlayback);
signals:
    void isPlayingChanged();
    void repeatChanged();
    void shuffleChanged();
    void volumeChanged();

    void minimumChanged();

    void maximumChanged();

    void valueChanged();

    void totalChanged();

    void currentChanged();

    void repeatIconChanged();

    void shuffleIconChanged();

private:
    QString getRepeatIcon(lib::repeat_state repeatState);
    void setProgress(int current, int duration);
    auto getNextRepeatState(const lib::spt::playback &playback) -> lib::repeat_state;
    lib::spt::api &spotify;
    lib::settings &settings;
    lib::paths &paths;
    lib::http_client &httpClient;
    lib::qt::http_client &httpClient2;

    bool m_isPlaying{false};
    lib::repeat_state m_repeat{lib::repeat_state::off};
    bool m_shuffle{false};
    int m_volume{0};
    int m_minimum{0};
    int m_maximum{0};
    int m_value{0};
    int m_total{0};
    int m_current{0};
    QString m_repeatIcon;
    QString m_shuffleIcon;
};

#endif // SPOTIFYAPPLICATION_H
