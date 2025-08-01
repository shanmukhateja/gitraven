#include "ravenfile.h"

#include "ravenutils.h"

RavenFile::RavenFile()
{
    m_magic_cookie = magic_open(MAGIC_MIME_ENCODING);

    if (magic_load(m_magic_cookie, NULL) != 0)
    {
        throw std::runtime_error("Failed to load libmagic");
    }
}

RavenFile::~RavenFile()
{
    magic_close(m_magic_cookie);
}

// https://gist.github.com/vivithemage/9489378
bool RavenFile::checkFileIsBinary(QString filePath)
{
    // 1. We do not consider empty files as binary
    //    Note: This check fixes a bug where empty file in FS
    //          cannot be modified.
    QFile f(filePath);
    if (f.size() == 0) return false;

    // 2. Check with libmagic
    assert(m_magic_cookie != NULL);
    auto encoding = magic_file(m_magic_cookie, filePath.toStdString().c_str());
    std::string encodingStr(encoding);
    return encodingStr == "binary";
}

std::optional<QStringConverter::Encoding> RavenFile::detectEncoding(QString filePath)
{
    // qDebug() << "RavenFile::detectEncoding called" << filePath;
    const char *encoding = magic_file(m_magic_cookie, filePath.toStdString().c_str());
    auto encodingOpt = RavenUtils::getQStringConvEncodingForLibMagicName(encoding);
    // qDebug() << "encoding="    << encoding;
    // qDebug() << "encodingOpt=" << encodingOpt;

    return encodingOpt;
}

std::optional<QStringConverter::Encoding> RavenFile::detectEncodingByBuffer(QByteArray bytes) {
    // qDebug() << "RavenFile::detectEncodingByBuffer called";
    const char *encoding = magic_buffer(m_magic_cookie, bytes.data(), bytes.size());
    auto encodingOpt = RavenUtils::getQStringConvEncodingForLibMagicName(encoding);
    // qDebug() << "encoding="    << encoding;
    // qDebug() << "encodingOpt=" << encodingOpt;

    return encodingOpt;
}
