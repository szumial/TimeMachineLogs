#include <QDirIterator>

#include "FileCollector.h"
#include "FileHasher.h"

FileCollector::FileCollector(const QString &rootPath)
    : m_rootPath{rootPath}
{
    scan();
}

const QList<FileEntry> &FileCollector::getUniqueFiles() const
{
    return m_uniqueFiles;
}

const QList<QList<FileEntry> > &FileCollector::getDuplicateFileGroups() const
{
    return m_duplicateFileGroups;
}

void FileCollector::scan()
{
    QHash<qint64, QList<FileEntry>> sizeGroups;
    QDirIterator                    dirIterator{m_rootPath,
                                                QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden,
                                                QDirIterator::Subdirectories};

    // Group by file size first - prefiltering to find possible duplicates
    while (dirIterator.hasNext())
    {
        QFileInfo fileInfo{dirIterator.next()};
        FileEntry fileEntry{fileInfo, m_rootPath};

        sizeGroups[fileEntry.size()].append(std::move(fileEntry));
    }

    // Iterate over each size group
    for (auto it{sizeGroups.constBegin()}; it != sizeGroups.constEnd(); ++it)
    {
        const QList<FileEntry> &filesOfSameSize{it.value()};

        // Groups of size of 1 contain unique files only
        if (filesOfSameSize.size() == s_single)
        {
            m_uniqueFiles.append(filesOfSameSize.first());
            continue;
        }

        // Hash files that share the same size - compare their contents by calculated hashes
        QHash<QByteArray, QList<FileEntry>> hashGroups;

        for(auto file: filesOfSameSize)
        {
            file.setHash(FileHasher::calculateHash(file.path()));
            hashGroups[file.hash()].append(std::move(file));
        }

        // Group by hash and classify if files are unique or duplicates
        for (auto hashIt{hashGroups.constBegin()}; hashIt != hashGroups.constEnd(); ++hashIt)
        {
            const QList<FileEntry> &sameHashFiles{hashIt.value()};

            if (sameHashFiles.size() == s_single)
                m_uniqueFiles.append(sameHashFiles.first());
            else
                m_duplicateFileGroups.append(sameHashFiles);
        }
    }
}
