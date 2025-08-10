#include <QDir>

#include "FileEntry.h"

FileEntry::FileEntry(const QFileInfo &fileInfo, const QString &rootPath)
    : m_name{fileInfo.fileName()}
    , m_path{fileInfo.absoluteFilePath()}
    , m_relativePath{QDir{rootPath}.relativeFilePath(m_path)}
    , m_size{fileInfo.size()}
    , m_hash{QByteArray{}}
{
}

const QString &FileEntry::name() const
{
    return m_name;
}

const QString &FileEntry::path() const
{
    return m_path;
}

const QString &FileEntry::relativePath() const
{
    return m_relativePath;
}

qint64 FileEntry::size() const
{
    return m_size;
}

void FileEntry::setHash(const QByteArray &hash)
{
    m_hash = hash;
}

const QByteArray &FileEntry::hash() const
{
    return m_hash;
}
