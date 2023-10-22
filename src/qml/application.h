#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QQmlApplicationEngine>
#include "spotifyapplication.h"
#include "qmltracks.h"
#include "qmlview.h"
#include <QQmlContext>
#pragma once

#include "../../main.h"
class SpotifyApplication;
class QmlView;
class QmlTracks;
class Application : public QObject
{
    Q_OBJECT
public:
    Application(QQmlApplicationEngine *engine,lib::settings &settings, lib::paths &paths,
                lib::qt::http_client &httpClient, lib::spt::api &spotify,QObject *parent = nullptr);

    static void createInstance(QQmlApplicationEngine *engine,lib::settings &settings, lib::paths &paths,
                               lib::qt::http_client &httpClient, lib::spt::api &spotify);

    static Application* getInstance() {
        return app;
    };

    static Application* app;

    Q_PROPERTY(QString appTitle READ appTitle WRITE setAppTitle NOTIFY appTitleChanged)

    Q_INVOKABLE void pauseMusic();
    QString appTitle() const;
    void initClient();
    auto playback() const -> const lib::spt::playback &;
    auto startClient() -> const SpotifyClient::Runner *;
    const spt::Current &getCurrent();
    auto getSpotifyRunner() -> const SpotifyClient::Runner *;
    void stopClient();
    void initMediaController();
    void initMediaHotkeys();
    void initDevice();

    lib::json_cache& getCache() {
        return cache;
    }

    lib::qt::http_client& getHttpClient2() {
        return httpClient2;
    }

    QQmlApplicationEngine* getEngine() {
        return m_engine;
    }

    static QmlTracks * getTracks(){
        return qmlTracks;
    }

    std::vector<lib::spt::track> loadTracksFromCache(const std::string &id);

    void saveTracksToCache(const std::string &id, const std::vector<lib::spt::track> &tracks);
#ifdef _WIN32
    void registerMediaHotkeys(bool enabled);
#endif
public slots:
    void setAppTitle(const QString &newAppTitle);
    void refresh();
    void refreshed(const lib::spt::playback &playback);

    void onSpotifyStatusChanged(const QString &status);
signals:
    void playbackRefreshed(const lib::spt::playback &current, const lib::spt::playback &previous);
    void appTitleChanged();

private:
    int refreshCount = -1;
    lib::spt::api &spotify;
    spt::Current current;
    lib::settings &settings;
    lib::paths &paths;
    lib::json_cache cache;
    lib::spt::user currentUser;
    lib::http_client &httpClient;
    lib::qt::http_client &httpClient2;
    QQmlApplicationEngine* m_engine;
    QString m_appTitle;
    SpotifyClient::Runner *spotifyRunner = nullptr;
    static SpotifyApplication *spotifyApp;
    static QmlView *qmlview;
    static QmlTracks * qmlTracks;
#ifdef USE_DBUS
    mp::Service *mediaPlayer = nullptr;
#endif

    // Prevent copy and assignment.
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};

#endif // APPLICATION_H
