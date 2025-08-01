#ifndef RAVENFILE_H
#define RAVENFILE_H

#include <QObject>
#include <QDebug>

#include <magic.h>

class RavenFile : public QObject
{
    Q_OBJECT
public:
    explicit RavenFile();
    ~RavenFile() override;

    bool checkFileIsBinary(QString filePath);

    std::optional<QStringConverter::Encoding> detectEncoding(QString filePath);
    std::optional<QStringConverter::Encoding> detectEncodingByBuffer(QByteArray bytes);

private:
    magic_t m_magic_cookie = NULL;
};

#endif // RAVENFILE_H
