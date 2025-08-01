#ifndef RAVENUTILS_H
#define RAVENUTILS_H

#include "ravenfile.h"

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

#include <algorithm>

class RavenUtils: public QObject
{
public:

    // FIXME: This code needs to be moved to a separate file.
    static std::optional<QStringConverter::Encoding> detectTextEncoding(QByteArray bytes) {
        // BOM detection is supported by Qt
        std::optional<QStringConverter::Encoding> encoding = QStringConverter::encodingForData(bytes);
        if (encoding.has_value()) return encoding.value();

        // We do not support binary files.
        if (bytes.size() >= 2) {
            bool isBinary = (bytes.slice(0, 2) == QByteArray(0x00));
            if (isBinary) return std::nullopt;
        }

        // Check with libmagic
        // FIXME: Need to merge multiple encoding detection logic
        RavenFile f;
        auto enc = f.detectEncodingByBuffer(bytes);
        if (enc.has_value()) return enc.value();

        // Default value
        return QStringConverter::Utf8;
    }

    static bool saveFile(QString absPath, QString text)
    {
        qDebug() << "RavenUtils::saveFile called";
        std::optional<QStringConverter::Encoding> encoding;

        // Read the file to detect encoding

        QFile fileForEncodingCheck(absPath);
        if (fileForEncodingCheck.open(QFile::ReadOnly)) {

            // Read first 4 bytes from `text` variable.
            // Note: We need to use the modified text to determine encoding
            //       in case existing file contents are empty.
            // FIXME: UTF-16 support?
            auto minSize = std::min(4, (int)text.size());
            QByteArray bytes(text.slice(0, minSize).toUtf8());
            fileForEncodingCheck.close();
            encoding = detectTextEncoding(bytes);
            if (!encoding.has_value()) {
                qDebug() << "Unable to determine file encoding, skipping modifying file.";
                return false;
            }
        }

        // Write new text content.
        QFile file(absPath);
        if (file.open(QFile::WriteOnly)) {
            QTextStream out(&file);
            // Set file encoding if known
            if (encoding.has_value()) {
                out.setEncoding(encoding.value());
            }
            out << text;
            out.flush();
            return true;
        }

        return false;
    }

    static std::optional<QStringConverter::Encoding> getQStringConvEncodingForLibMagicName(const char *name) {

        if (strcmp(name,"us-ascii") == 0) return QStringConverter::Encoding::Utf8;

        if (strcmp(name, "utf-16le") == 0) return QStringConverter::Encoding::Utf16LE;
        if (strcmp(name, "utf-16be") == 0) return QStringConverter::Encoding::Utf16BE;

        return std::nullopt;
    }

    static std::filesystem::path getEditorDirPath() {
        QDir editorDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                           .append(QDir::separator())
                           .append("editor");
        return editorDir.filesystemAbsolutePath();
    }
};

#endif // RAVENUTILS_H
