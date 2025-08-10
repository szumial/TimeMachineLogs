#include <QDir>

#include "Archiver.h"

bool Archiver::pack(const QString &archivePath,
                    const QList<FileEntry> &uniqueFiles,
                    const QList<QList<FileEntry> > &duplicateGroups,
                    qint64 chunkSize)
{
    QFile archiveFile{archivePath};

    if (!archiveFile.open(QIODevice::WriteOnly))
    {
        qWarning() << "Cannot open archive file for writing: " << archivePath;
        return false;
    }

    QDataStream out{&archiveFile};

    QList<FileMeta> metadataList;
    auto            totalFiles{uniqueFiles.size()};

    for (const auto &group: duplicateGroups)
        totalFiles += group.size();

    metadataList.reserve(totalFiles);

    // Write unique files to the archive
    if (!writeUniqueFiles(archiveFile, uniqueFiles, metadataList, chunkSize))
        return false;

    // Write duplicate files content once, record offsets by hash (calculated for duplicates)
    if (!writeDuplicateFiles(archiveFile, duplicateGroups, metadataList, chunkSize))
        return false;

    // Write metadata index at the end of the file
    qint64 metadataOffset;

    if (!writeMetadata(out, metadataList, metadataOffset))
        return false;

    // Write metadata offset as footer so we can find it during unpack
    writeMetadataOffset(out, metadataOffset);

    archiveFile.close();

    return true;
}

bool Archiver::unpack(const QString &archivePath, const QString &outputDir, qint64 chunkSize)
{
    QFile archiveFile{archivePath};

    if (!archiveFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open archive: " << archivePath;
        return false;
    }

    QDataStream in{&archiveFile};

    qint64 metadataOffset;

    // Read medatada offset from the archive file's footer
    if (!readMetadataOffset(archiveFile, in, metadataOffset))
        return false;

    archiveFile.seek(metadataOffset);

    QList<FileMeta> metadataList;

    // Get the metadata
    if (!readMetadata(in, metadataList))
        return false;

    // Extract all files from the archive
    for (qsizetype i{0}; i < metadataList.size(); ++i)
    {
        const auto &meta{metadataList.at(i)};

        if (!extractFile(archiveFile, meta, outputDir, chunkSize))
            return false;
    }

    return true;
}

bool Archiver::writeFileContentToArchive(QFile &archiveFile, const QString &sourceFilePath, qint64 chunkSize)
{
    QFile src{sourceFilePath};

    if (!src.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open file for reading: " << sourceFilePath;
        return false;
    }

    QByteArray buffer;

    buffer.resize(static_cast<int>(chunkSize));

    qint64 bytesRead;

    while ((bytesRead = src.read(buffer.data(), buffer.size())) > 0)
    {
        if (archiveFile.write(buffer.constData(), bytesRead) != bytesRead)
        {
            qWarning() << "Failed writing to archive for file: " << sourceFilePath;
            src.close();
            return false;
        }
    }

    src.close();

    return true;
}

bool Archiver::writeMetadata(QDataStream &out, const QList<FileMeta> &metadataList, qint64 &metadataOffset)
{
    metadataOffset = out.device()->pos();
    out << metadataList.size();

    for (const auto &meta : metadataList)
    {
        out << meta.relativePath;
        out << meta.size;
        out << meta.hash;
        out << meta.dataOffset;
    }

    return true;
}

bool Archiver::readMetadata(QDataStream &in, QList<FileMeta> &metadataList)
{
    qint64 fileCount;

    in >> fileCount;

    if (fileCount <= 0)
    {
        qWarning() << "No files stored in archive.";
        return false;
    }

    metadataList.reserve(static_cast<int>(fileCount));

    for (qsizetype i{0}; i < fileCount; ++i)
    {
        FileMeta meta;

        in >> meta.relativePath;
        in >> meta.size;
        in >> meta.hash;
        in >> meta.dataOffset;

        metadataList.append(meta);
    }

    return true;
}

bool Archiver::extractFile(QFile &archiveFile, const FileMeta &meta, const QString &outputDir, qint64 chunkSize)
{
    auto outputFilePath{QDir{outputDir}.filePath(meta.relativePath)};

    QDir().mkpath(QFileInfo{outputFilePath}.path());

    QFile outFile{outputFilePath};

    if (!outFile.open(QIODevice::WriteOnly))
    {
        qWarning() << "Cannot create file: " << outputFilePath;
        return false;
    }

    archiveFile.seek(meta.dataOffset);

    QByteArray buffer;

    buffer.resize(static_cast<int>(chunkSize));

    auto bytesRemaining{meta.size};

    while (bytesRemaining > 0)
    {
        auto toRead{qMin(bytesRemaining, chunkSize)};
        auto bytesRead{archiveFile.read(buffer.data(), toRead)};

        if (bytesRead <= 0)
        {
            qWarning() << "Unexpected end of archive while reading" << meta.relativePath;
            outFile.close();
            return false;
        }

        if (outFile.write(buffer.constData(), bytesRead) != bytesRead)
        {
            qWarning() << "Failed writing file:" << outputFilePath;
            outFile.close();
            return false;
        }

        bytesRemaining -= bytesRead;
    }

    outFile.close();

    return true;
}

bool Archiver::readMetadataOffset(QFile &archiveFile, QDataStream &in, qint64 &metadataOffset)
{
    if (archiveFile.size() < static_cast<qint64>(sizeof(qint64)))
    {
        qWarning() << "Archive file too small to contain metadata offset.";
        return false;
    }

    archiveFile.seek(archiveFile.size() - static_cast<qint64>(sizeof(qint64)));
    in >> metadataOffset;

    if (metadataOffset < 0 || metadataOffset >= archiveFile.size())
    {
        qWarning() << "Invalid metadata offset.";
        return false;
    }

    return true;
}

void Archiver::writeMetadataOffset(QDataStream &out, qint64 offset)
{
    out << offset;
}

bool Archiver::writeUniqueFiles(QFile &archiveFile,
                                const QList<FileEntry> &uniqueFiles,
                                QList<FileMeta> &metadataList,
                                qint64 chunkSize)
{
    for (const auto &file: uniqueFiles)
    {
        auto offset{archiveFile.pos()};

        // Write unique file's content to the archive
        if (!writeFileContentToArchive(archiveFile, file.path(), chunkSize))
            return false;

        // Store metadata for this file
        FileMeta meta;

        meta.relativePath = file.relativePath();
        meta.size = file.size();
        meta.hash = QByteArray{}; // No hash calculated for unique files
        meta.dataOffset = offset;

        metadataList.append(meta);
    }

    return true;
}

bool Archiver::writeDuplicateFiles(QFile &archiveFile,
                                   const QList<QList<FileEntry>> &duplicateGroups,
                                   QList<FileMeta> &metadataList,
                                   qint64 chunkSize)
{
    QHash<QByteArray, qint64> hashToOffset; // map file hash to offset in archive

    for (const auto &group : duplicateGroups)
    {
        if (group.isEmpty())
            continue;

        const auto &dataSource{group.first()}; // First file in group is the data source
        qint64     offset;

        if (hashToOffset.contains(dataSource.hash()))
        {
            offset = hashToOffset.value(dataSource.hash());
        }
        else
        {
            offset = archiveFile.pos();

            // Write data source file's content to the archive
            if (!writeFileContentToArchive(archiveFile, dataSource.path(), chunkSize))
                return false;

            hashToOffset.insert(dataSource.hash(), offset);
        }

        // Store all duplicate files metadata for unpacking
        for (const auto &duplicate : group)
        {
            FileMeta meta;

            meta.relativePath = duplicate.relativePath();
            meta.size = duplicate.size();
            meta.hash = duplicate.hash();
            meta.dataOffset = offset;

            metadataList.append(meta);
        }
    }

    return true;
}
