#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <QFileInfo>

class FileEntry
{
public:
    explicit FileEntry(const QFileInfo &fileInfo, const QString &rootPath);

    const QString &name() const;
    const QString &path() const;
    const QString &relativePath() const;
    qint64        size() const;

    void             setHash(const QByteArray &hash);
    const QByteArray &hash() const;

private:
    QString    m_name;
    QString    m_path;
    QString    m_relativePath;
    qint64     m_size;
    QByteArray m_hash;
};

#endif // FILEENTRY_H
