#ifndef QMLTITLE_H
#define QMLTITLE_H
#pragma once
#include <QObject>
#include "qmltitleinfo.h"
#include "../../main.h"
class QmlTitle : public QObject
{
    Q_OBJECT
public:
    explicit QmlTitle(lib::spt::api &spotify, const lib::cache &cache,QObject *parent = nullptr);
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged FINAL)
    QString title() const;

public slots:
    void setTitle(const QString &newTitle);
    void onPlaybackRefreshed(const lib::spt::playback &playback,
                             const lib::spt::playback &previous);
signals:

    void titleChanged();

private:
    QmlTitleInfo *info = nullptr;
    lib::spt::api &spotify;
    const lib::cache &cache;
    static constexpr int spacing = 16;
    auto playlist(const std::string &id) -> lib::spt::playlist;
    auto playlistNameFromSaved(const std::string &id) -> std::string;
    void playlistName(const std::string &id, lib::callback<std::string> &callback);

    QString m_title;
};

#endif // QMLTITLE_H
