#include "codeeditor.hpp"

#include <QPainter>
#include <QTextBlock>
#include <QFile>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

//![constructor]

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setTabStopDistance(QFontMetricsF(this->font()).horizontalAdvance(' ') * 4);

    lineNumberArea = new LineNumberArea(this);
    
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);



    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

//![constructor]

//![extraAreaWidth]

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

//![extraAreaWidth]

//![slotUpdateExtraAreaWidth]

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
//![slotUpdateExtraAreaWidth]

//![slotUpdateRequest]

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

//![slotUpdateRequest]

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent *event) {
    QPlainTextEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Return) {
        add_leading_offset();
    }
}


//![cursorPositionChanged]

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::black).lighter(5);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

//![cursorPositionChanged]

//![extraAreaPaintEvent_0]

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

//![extraAreaPaintEvent_0]

//![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
//![extraAreaPaintEvent_2]

void CodeEditor::add_leading_offset() {
    QString editor_text = this->toPlainText();

    const int start_cursor_pos = this->textCursor().position();
    const int line_end = start_cursor_pos - 2;
    int line_begin;

    for (line_begin = line_end; line_begin >= 0 && editor_text[line_begin] != '\n'; --line_begin);

    if (line_begin >= 0) { // if we found the end of the previous line
        ++line_begin;
    } else { // if we didn't found it, than we are on the first line;
        line_begin = 0;
    }

    const QString line = editor_text.mid(line_begin, start_cursor_pos - line_begin - 1);

    if (line.isEmpty()) {
        return;
    }



    QString offset;
    for (const auto &ch : line) {
        switch (ch.toLatin1()) {
            case ' ':
            case '\t':
                offset += ch;
            break;
            default:
                goto end;
            break;
        }
    }

    end:
    if (line.back() == '{') {
        offset += '\t';
    }

    if (!offset.isEmpty()) {
        insertPlainText(offset);
    }

    if (line.back() == '{') {
        if (offset.back() == '\t') {
            offset.chop(1);
        }

        insertPlainText('\n' + offset + '}');

        QTextCursor temp_cursor = this->textCursor();
        temp_cursor.setPosition(start_cursor_pos);
        setTextCursor(temp_cursor);

        this->moveCursor(QTextCursor::EndOfLine);
    }
} 