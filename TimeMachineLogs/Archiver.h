#ifndef ARCHIVER_H
#define ARCHIVER_H

#include "FileEntry.h"

class Archiver
{
public:
    static bool pack(const QString &archivePath,
                     const QList<FileEntry> &uniqueFiles,
                     const QList<QList<FileEntry>> &duplicateGroups,
                     qint64 chunkSize = s_chunkSize);

    static bool unpack(const QString &archivePath,
                       const QString &outputDir,
                       qint64 chunkSize = s_chunkSize);

private:
    struct FileMeta
    {
        QString    relativePath;
        qint64     size;
        QByteArray hash;
        qint64     dataOffset;
    };

    static bool writeFileContentToArchive(QFile &archiveFile, const QString &sourceFilePath, qint64 chunkSize);
    static bool writeMetadata(QDataStream &out, const QList<FileMeta> &metadataList, qint64 &metadataOffset);
    static bool readMetadata(QDataStream &in, QList<FileMeta> &metadataList);
    static bool extractFile(QFile &archiveFile, const FileMeta &meta, const QString &outputDir, qint64 chunkSize);
    static bool readMetadataOffset(QFile &archiveFile, QDataStream &in, qint64 &metadataOffset);
    static void writeMetadataOffset(QDataStream &out, qint64 offset);

    static bool writeUniqueFiles(QFile &archiveFile,
                                 const QList<FileEntry> &uniqueFiles,
                                 QList<FileMeta> &metadataList,
                                 qint64 chunkSize);
    static bool writeDuplicateFiles(QFile &archiveFile,
                                    const QList<QList<FileEntry>> &duplicateGroups,
                                    QList<FileMeta> &metadataList,
                                    qint64 chunkSize);

    static bool validateArchivePathForPack(const QString &path);
    static bool validateArchivePathForUnpack(const QString &path);
    static bool validateOutputDirForUnpack(const QString &dirPath);

    static constexpr qint64 s_chunkSize{4 * 1024 * 1024};
};

#endif // ARCHIVER_H
