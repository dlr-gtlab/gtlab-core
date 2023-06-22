// Copyright (C) 2006 J-P Nurmi. All rights reserved.


#ifndef GTXMLHIGHLIGHTER_H
#define GTXMLHIGHLIGHTER_H

#include "gt_gui_exports.h"

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QColor>
#include <QTextEdit>

class GT_GUI_EXPORT GtXmlHighlighter : public QSyntaxHighlighter
{
public:
    explicit GtXmlHighlighter(QObject* parent);
    explicit GtXmlHighlighter(QTextDocument* parent);
    explicit GtXmlHighlighter(QTextEdit* parent);

    enum HighlightType
    {
        SyntaxChar,
        ElementName,
        Comment,
        AttributeName,
        AttributeValue,
        Error,
        Other
    };

    void setHighlightColor(HighlightType type,
                           const QColor& color,
                           bool foreground = true);

    void setHighlightFormat(HighlightType type,
                            const QTextCharFormat& format);

protected:
    void highlightBlock(const QString& rstrText) override;

    int  processDefaultText(int i, const QString& rstrText);

public slots:
    /**
     * @brief onThemeChanged - handles reaction on change of the GUI theme
     * Changes the colors for the highlights and rehighlights the text
     */
    void onThemeChanged();

private:
    void init();

    QTextCharFormat fmtSyntaxChar;
    QTextCharFormat fmtElementName;
    QTextCharFormat fmtComment;
    QTextCharFormat fmtAttributeName;
    QTextCharFormat fmtAttributeValue;
    QTextCharFormat fmtError;
    QTextCharFormat fmtOther;

    enum ParsingState
    {
        NoState = 0,
        ExpectElementNameOrSlash,
        ExpectElementName,
        ExpectAttributeOrEndOfElement,
        ExpectEqual,
        ExpectAttributeValue
    };

    enum BlockState
    {
        NoBlock = -1,
        InComment,
        InElement
    };

    ParsingState state;

};

#endif // GTXMLHIGHLIGHTER_H
