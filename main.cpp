#include "application.h"

#include <QMutex>
#include <QFile>
#include <QTextStream>

void fileLogger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString msgType;
    switch (type)
    {
    case QtDebugMsg:
        msgType = QStringLiteral("DEBUG");
        break;
    case QtInfoMsg:
        msgType = QStringLiteral("INFORMATION");
        break;
    case QtWarningMsg:
        msgType = QStringLiteral("WARNING");
        break;
    case QtCriticalMsg:
        msgType = QStringLiteral("CRITICAL");
        break;
    case QtFatalMsg:
        msgType = QStringLiteral("FATAL");
        break;
    /*case QtSystemMsg:
        msgType = QStringLiteral("SYSTEM");
        break;*/
    default:
        msgType = QStringLiteral("DEBUG");
        break;
    }
    QString messageStr = QStringLiteral("%0\t%1\t%2\t%3\t%4")
                .arg(msgType).arg(msg).arg(context.file).arg(context.line).arg(context.function);
    QString logPath = QCoreApplication::applicationDirPath();
    logPath += QStringLiteral("/debug.log");
    QFile file(logPath);
    if (file.open(QFile::WriteOnly | QFile::Append | QFile::Text))
    {
        QTextStream ts(&file);
        ts << messageStr << QLatin1Char('\n');
        file.close();
    }
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    qputenv("QT_ANGLE_PLATFORM", "d3d11");

#ifndef _DEBUG
    qInstallMessageHandler(fileLogger);
#endif

    Application app(argc, argv);

    return app.exec();
}
