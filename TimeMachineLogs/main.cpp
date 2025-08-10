#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

#include "FileCollector.h"
#include "ApplicationConstants.h"
#include "ArchiverModeHelper.h"
#include "Archiver.h"

static const QCommandLineOption modeOption{
    QStringList() << ApplicationConstants::MODE_SHORT << ApplicationConstants::MODE_LONG,
    ApplicationConstants::MODE_DESCRIPTION,
    ApplicationConstants::MODE_LONG
};

static const QCommandLineOption inputOption{
    QStringList() << ApplicationConstants::INPUT_SHORT << ApplicationConstants::INPUT_LONG,
    ApplicationConstants::INPUT_DESCRIPTION,
    ApplicationConstants::INPUT_LONG
};

static const QCommandLineOption outputOption{
    QStringList() << ApplicationConstants::OUTPUT_SHORT << ApplicationConstants::OUTPUT_LONG,
    ApplicationConstants::OUTPUT_DESCRIPTION,
    ApplicationConstants::OUTPUT_LONG
};

struct CommandLineArguments
{
    ArchiverMode mode;
    QString      input;
    QString      output;
};

CommandLineArguments parseArguments(const QCommandLineParser &parser)
{
    CommandLineArguments args;

    const auto &modeString{parser.value(modeOption)};

    args.mode = ArchiverModeHelper::stringToMode(modeString);
    args.input = parser.value(inputOption);
    args.output = parser.value(outputOption);

    return args;
}

void setupApplication(QCoreApplication &app)
{
    app.setApplicationName(ApplicationConstants::APPLICATION_NAME);
    app.setApplicationVersion(ApplicationConstants::APPLICATION_VERSION);
}

QList<QCommandLineOption> getCommandLineOptions()
{
    return QList<QCommandLineOption>{modeOption, inputOption, outputOption};
}

void setupCommandLineParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(ApplicationConstants::APPLICATION_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(getCommandLineOptions());
}

bool validateArguments(const QCommandLineParser &parser, char *argv[])
{
    if (!parser.isSet(modeOption) || !parser.isSet(inputOption) || !parser.isSet(outputOption))
    {
        qCritical() << "Error: Missing required arguments";
        qCritical() << "Usage examples:";
        qCritical() << " " << argv[0] << " --" << ApplicationConstants::MODE_LONG
                    << " " << ApplicationConstants::MODE_PACK
                    << " --" << ApplicationConstants::INPUT_LONG << " /path/to/directory"
                    << " --" << ApplicationConstants::OUTPUT_LONG << " archive.zip";
        qCritical() << " " << argv[0] << " --" << ApplicationConstants::MODE_LONG
                    << " " << ApplicationConstants::MODE_UNPACK
                    << " --" << ApplicationConstants::INPUT_LONG << " archive.zip"
                    << " --" << ApplicationConstants::OUTPUT_LONG << " /path/to/directory";
        qCritical() << "";
        qCritical() << "Use --help for more information";

        return false;
    }

    return true;
}

bool validateMode(const ArchiverMode mode)
{
    if (!ArchiverModeHelper::isValidMode(mode))
    {
        qCritical() << "Error: Invalid mode. Use '"
                    << ApplicationConstants::MODE_PACK << "' or '"
                    << ApplicationConstants::MODE_UNPACK << "'";

        return false;
    }

    return true;
}

// Use this method for testing purposes - automatically runs code logic with provided paths
void testWithoutCommandLineArgs()
{
    qDebug() << "TESTING APPLICATION LOGIC";

    FileCollector fileCollector{"<path_to_scan_for_files>"};
    const auto    &uniqueFiles{fileCollector.getUniqueFiles()};
    const auto    &duplicateFiles{fileCollector.getDuplicateFileGroups()};

    qDebug() << "WRITING ARCHIVE";
    Archiver::pack("<path_to_output_archive_file>", uniqueFiles, duplicateFiles);

    qDebug() << "UNPACKING THE ARCHIVE";
    Archiver::unpack("<path_to_input_archive_file>", "<output_path>");
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    setupApplication(app);
    setupCommandLineParser(parser);
    parser.process(app);

    // Uncomment this method call to run test without providing command line args
    // testWithoutCommandLineArgs();

    if (!validateArguments(parser, argv))
        return 1;

    auto args{parseArguments(parser)};

    if (!validateMode(args.mode))
        return 1;

    if (args.mode == ArchiverModeHelper::Mode::Pack)
    {
        FileCollector fileCollector{args.input};
        const auto    &uniqueFiles{fileCollector.getUniqueFiles()};
        const auto    &duplicateFileGroups{fileCollector.getDuplicateFileGroups()};

        if (!Archiver::pack(args.output, uniqueFiles, duplicateFileGroups))
        {
            qCritical() << "Failed to pack the archive:" << args.output;
            return 1;
        }
    }
    else if (args.mode == ArchiverModeHelper::Mode::Unpack)
    {
        if (!Archiver::unpack(args.input, args.output))
        {
            qCritical() << "Failed to unpack the archive:" << args.input;
            return 1;
        }
    }

    return 0;
}
