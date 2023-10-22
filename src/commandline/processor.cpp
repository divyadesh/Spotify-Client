#include "commandline/processor.hpp"
#include "commandline/args.hpp"
#include "lib/log.hpp"
//#include "util/systeminfo.hpp"

#include <QtDebug>

auto CommandLine::Processor::process(const QCommandLineParser &parser) -> bool
{
	if (parser.isSet(ARG_ENABLE_DEV))
	{
		lib::developer_mode::enabled = true;

		if (parser.isSet(ARG_NEW_PAGING))
		{
			lib::developer_mode::add_experiment(lib::experiment::new_paging);
		}

		return false;
	}

	if (parser.isSet(ARG_SYSTEM_INFORMATION))
	{
//		SystemInfo systemInfo;
//		qInfo().noquote() << systemInfo.to_text();
		return true;
	}

#ifdef USE_DBUS
	MediaPlayer::Client client;

	if (parser.isSet(ARG_PLAY_PAUSE))
	{
		client.playPause();
	}
	else if (parser.isSet(ARG_PREVIOUS_TRACK))
	{
		client.previousTrack();
	}
	else if (parser.isSet(ARG_NEXT_TRACK))
	{
		client.nextTrack();
	}
	else if (parser.isSet(ARG_METADATA))
	{
		qInfo().noquote() << client.metadata();
	}
	else
	{
		for (const auto &argument: parser.positionalArguments())
		{
			if (argument.startsWith(QStringLiteral("https://open.spotify.com/track/"))
				|| argument.startsWith(QStringLiteral("spotify:track:")))
			{
				client.openUri(argument);
				return true;
			}
		}
		return false;
	}
	return true;
#else
	return false;
#endif
}
