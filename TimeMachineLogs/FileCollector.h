#ifndef FILECOLLECTOR_H
#define FILECOLLECTOR_H

#include "FileEntry.h"

class FileCollector
{
public:
    explicit FileCollector(const QString &rootPath);

    const QList<FileEntry>        &getUniqueFiles() const;
    const QList<QList<FileEntry>> &getDuplicateFileGroups() const;

private:
    void scan();

    QString                 m_rootPath;
    QList<FileEntry>        m_uniqueFiles;
    QList<QList<FileEntry>> m_duplicateFileGroups;

    static constexpr qsizetype s_single{1};
};

#endif // FILECOLLECTOR_H
