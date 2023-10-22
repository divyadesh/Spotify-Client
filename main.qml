import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4 as CO
ApplicationWindow {
    width: 1280
    height: 280
    visible: true
    title: gApplication.appTitle

    header: RowLayout{
        Image {
            id: imageIcon
            source: gQmlView.currentImage
        }
        ColumnLayout{
            Label{
                Layout.fillWidth: true
                text: gQmlView.songDetails
            }
            Label{
                text: gQmlView.artistsName
            }
        }

        Button{
            text: "Prev"
            onClicked: {
                gSpotifyApp.previousSong()
            }
        }

        Button{
            text: gSpotifyApp.isPlaying ? "Pause" : "Play"
            onClicked: {
                gSpotifyApp.playPause()
            }
        }

        Button{
            text: "Next"
            onClicked: {
                gSpotifyApp.nextSong()
            }
        }
        Label{
            text:millisecondsToTime(gSpotifyApp.current)
            function millisecondsToTime(milliseconds) {
                var seconds = Math.floor(milliseconds / 1000);
                milliseconds %= 1000;
                var minutes = Math.floor(seconds / 60);
                seconds %= 60;
                var hours = Math.floor(minutes / 60);
                minutes %= 60;
                return Qt.formatDateTime(new Date(0, 0, 0, hours, minutes, seconds, milliseconds), "hh:mm:ss");
            }
        }
        Slider{
            width: 100
            height: 10
            from: gSpotifyApp.minimum
            to:gSpotifyApp.maximum
            value:gSpotifyApp.value
        }

        Label{
            text:millisecondsToTime(gSpotifyApp.total)
            function millisecondsToTime(milliseconds) {
                var seconds = Math.floor(milliseconds / 1000);
                milliseconds %= 1000;
                var minutes = Math.floor(seconds / 60);
                seconds %= 60;
                var hours = Math.floor(minutes / 60);
                minutes %= 60;
                return Qt.formatDateTime(new Date(0, 0, 0, hours, minutes, seconds, milliseconds), "hh:mm:ss");
            }
        }
        Button{
            text: "Shuffle"
            onClicked: {
                gSpotifyApp.onShuffle(!gSpotifyApp.shuffle)
            }
        }

        Button{
            text: "Top Track"
            onClicked: gQmlView.topLoad()
        }
    }
}
