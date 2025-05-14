#pragma once

#include <QString>
#include <QList>
#include <QMetaType>

namespace QT_UI {

/**
 * @brief Data structure for enhancements
 */
class EnhancementData
{
public:
    struct CodeEntry {
        uint32_t command;
        QString value;
    };
    
    typedef QList<CodeEntry> CodeEntries;
    typedef QList<QString> PluginList;
    
    EnhancementData() :
        enabledByDefault(false),
        overclock(false),
        overclockModifier(1),
        isEnabled(false)
    {
    }
    
    QString name;
    QString author;
    QString notes;
    CodeEntries codeEntries;
    PluginList pluginList;
    bool enabledByDefault;
    bool overclock;
    int overclockModifier;
    bool isEnabled;
};

} // namespace QT_UI

Q_DECLARE_METATYPE(QT_UI::EnhancementData*)
