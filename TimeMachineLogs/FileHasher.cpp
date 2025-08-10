#include <QFile>

#include "FileHasher.h"

QByteArray FileHasher::calculateHash(const QString &filePath,
                                     QCryptographicHash::Algorithm algorithm)
{
    QFile file{filePath};

    if (!file.open(QIODevice::ReadOnly))
        return {};

    QCryptographicHash hasher{algorithm};

    while (!file.atEnd())
        hasher.addData(file.read(s_bufferSize));

    return hasher.result();
}
