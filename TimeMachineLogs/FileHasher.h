#ifndef FILEHASHER_H
#define FILEHASHER_H

#include <QCryptographicHash>

class FileHasher
{
public:
    static QByteArray calculateHash(const QString &filePath,
                                    QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha256);

private:
    static constexpr int s_bufferSize{8192};
};

#endif // FILEHASHER_H
