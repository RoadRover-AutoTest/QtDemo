#include "recheckcombobox.h"
#include <QMouseEvent>
#include <QAbstractItemView>

reCheckComboBox::reCheckComboBox(QWidget *parent)
    : QComboBox(parent)
{
    //setStyleSheet("QComboBox QAbstractItemView {selection-background-color: transparent;}");
}

reCheckComboBox::~reCheckComboBox()
{

}

void reCheckComboBox::appendItem(const QString &text, bool bChecked)
{
    QIcon icon;

    if( bChecked )
    {
        icon.addFile(":/Title/check.png");
    }
    else
    {
        icon.addFile(":/Title/nocheck.png");
    }

    addItem(icon, text, bChecked);
    //setCurrentTextDeal(bChecked,text);
}

void reCheckComboBox::updateIndexStatus(int index)
{
    bool bChecked = this->itemData(index).toBool();

    if( bChecked )
    {
        this->setItemIcon(index, QIcon(":/Title/nocheck.png"));

    }
    else
    {
        this->setItemIcon(index, QIcon(":/Title/check.png"));
    }

    setItemData(index, !bChecked);

    checkedStateChange(index, !bChecked);

    //setCurrentTextDeal(bChecked,this->itemText(index));
}

void reCheckComboBox::clickedItem(int index,bool checked)
{
    if( checked )
    {
        this->setItemIcon(index, QIcon(":/Title/check.png"));

    }
    else
    {
        this->setItemIcon(index, QIcon(":/Title/nocheck.png"));
    }

    setItemData(index, checked);

    //checkedStateChange(index, !checked);//点击时已经确定了状态，无需再上传，避免进行处理又恢复
}

bool reCheckComboBox::isClickedItem(int index)
{
    return this->itemData(index).toBool();
}

void reCheckComboBox::setCurrentTextDeal(bool checked,const QString &itemtext)
{
    QString text = this->currentText();
    if (text.isEmpty() == false)
    {
        if (checked)
        {
            if (text.split("|").indexOf(itemtext) == -1)
            {
                text.append("|" + itemtext);
            }
        }
        else
        {
            text.remove("|" + itemtext);
            text.remove(itemtext);
            if (text.size() != 0 && text.at(0) == '|')
            {
                text.remove(0, 1);
            }
        }
    }
    else
    {
        if (checked)
        {
            text.append(itemtext);
        }
    }
    this->setCurrentText(text);
}

void reCheckComboBox::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x();

    int iconWidth = this->iconSize().width();

    if( x <= iconWidth )
    {
        int index = this->currentIndex();

        updateIndexStatus(index);
    }
    else
    {
        QComboBox::mousePressEvent(event);
    }
}

void reCheckComboBox::hidePopup()
{
    int iconWidth = this->iconSize().width();

    int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();

    int index = view()->selectionModel()->currentIndex().row();

    if( x >= 0 && x <= iconWidth )
    {
        updateIndexStatus(index);
    }
    else
    {
        QComboBox::hidePopup();
    }
}


