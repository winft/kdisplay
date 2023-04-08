/*
 * SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>
 * SPDX-FileCopyrightText: 2020 Roman Gilg <subdiff@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QStyle>

#include <KAboutData>
#include <KCMultiDialog>
#include <KLocalizedString>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    KAboutData about(QStringLiteral("kdisplay"),
                     i18n("KDisplay"),
                     QStringLiteral(KDISPLAY_VERSION),
                     i18n("KDisplay"),
                     KAboutLicense::GPL,
                     i18n("Copyright © 2020 Roman Gilg"));
    about.addAuthor(QStringLiteral("Roman Gilg"));
    about.setBugAddress("https://gitlab.com/kwinft/kdisplay/-/issues");
    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(
        QStringLiteral("args"), i18n("Arguments for the config module."), QStringLiteral("args")));

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    auto dialog = new KCMultiDialog;
    dialog->addModule(KPluginMetaData(QStringLiteral("plasma/kcms/systemsettings/kcm_kdisplay")),
                      {parser.value(QStringLiteral("args"))});

    auto style = dialog->style();
    dialog->setContentsMargins(style->pixelMetric(QStyle::PM_LayoutLeftMargin),
                               style->pixelMetric(QStyle::PM_LayoutTopMargin),
                               style->pixelMetric(QStyle::PM_LayoutRightMargin),
                               style->pixelMetric(QStyle::PM_LayoutBottomMargin));

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();

    app.setQuitOnLastWindowClosed(true);

    return app.exec();
}
