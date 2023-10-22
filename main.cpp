#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include <QApplication>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "util/icon.hpp"
#include "util/appinstalltype.hpp"
#include "util/refresher.hpp"

#include "spotify/deviceselect.hpp"

#include "lib/qtpaths.hpp"
#include "lib/spotify/request.hpp"
#include "lib/crash/crashhandler.hpp"

#include "dialog/setup.hpp"

#include "commandline/parser.hpp"
#include "commandline/args.hpp"
#include "commandline/processor.hpp"
#include "main.h"

#include "qml/application.h"
int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(ORG_NAME);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif


    // Custom crash handler if not using KCrash
    lib::crash_handler::init();



    QApplication app(argc, argv);


    // Settings
    QtPaths paths(nullptr);
    lib::settings settings(paths);

    // Check fallback icons
    //Icon::useFallbackIcons = settings.general.fallback_icons;

    // Command line arguments
    CommandLine::Parser parser(app);

    if (CommandLine::Processor::process(parser))
    {
        return 0;
    }

    if (parser.isSet(ARG_LIST_PATHS))
    {
        lib::log::info("Config: {}", paths.config_file().string());
        lib::log::info("Cache:  {}", paths.cache().string());
        return 0;
    }

    // First setup window
    if (settings.account.refresh_token.empty() || parser.isSet(ARG_FORCE_SETUP))
    {
        Dialog::Setup dialog(settings, nullptr);
        if (dialog.exec() == QDialog::Rejected)
        {
            return 0;
        }
    }

    lib::qt::http_client httpClient(nullptr);
    const spt::DeviceSelect deviceSelect(nullptr);
    lib::spt::request request(settings, httpClient, deviceSelect);
    lib::spt::api spotify(settings, httpClient, request);

    Refresher refresher(settings, request);
    if (!refresher.refresh())
    {
        return 1;
    }


    QQmlApplicationEngine engine;

    Application::createInstance(&engine, settings, paths, httpClient, spotify);

    engine.rootContext()->setContextProperty("gApplication", Application::getInstance());


    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return QApplication::exec();
}
