#ifndef ARCHIVERMODEHELPER_H
#define ARCHIVERMODEHELPER_H

#include <QMetaEnum>
#include <QObject>

class ArchiverModeHelper : public QObject
{
    Q_OBJECT

public:
    enum class Mode
    {
        Pack,
        Unpack,
        Unknown
    };
    Q_ENUM(Mode)

    static inline Mode stringToMode(const QString &modeString)
    {
        bool ok;
        auto metaEnum{QMetaEnum::fromType<Mode>()};

        if (auto value{metaEnum.keyToValue(modeString.toUtf8().constData(), &ok)};
            ok)
            return static_cast<Mode>(value);

        for (auto i{0}; i < metaEnum.keyCount(); ++i)
        {
            if (QString::fromLatin1(metaEnum.key(i)).compare(modeString, Qt::CaseInsensitive) == 0)
                return static_cast<Mode>(metaEnum.value(i));
        }

        return Mode::Unknown;
    }

    static inline QString modeToString(Mode mode)
    {
        auto metaEnum{QMetaEnum::fromType<Mode>()};
        auto *key{metaEnum.valueToKey(static_cast<int>(mode))};

        return key ? QString::fromLatin1(key) : s_unknown;
    }

    static inline bool isValidMode(Mode mode)
    {
        return mode != Mode::Unknown;
    }

private:
    static inline const QString s_unknown{"Unknown"};
};

using ArchiverMode = ArchiverModeHelper::Mode;

#endif // ARCHIVERMODEHELPER_H
