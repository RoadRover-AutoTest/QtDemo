#ifndef RECHECKCOMBOBOX_H
#define RECHECKCOMBOBOX_H

#include <QObject>
#include <QComboBox>

class reCheckComboBox : public QComboBox
{
    Q_OBJECT
public:
    reCheckComboBox(QWidget *parent = 0);
    ~reCheckComboBox();

    //添加下拉框内容
    void appendItem(const QString &text, bool bChecked);

    //QComboBox的虚函数用来隐藏列表框,当单击是复选框时不让隐藏，用来改变状态
    void hidePopup();

    void clickedItem(int index,bool checked);

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    void updateIndexStatus(int index);//更新更改项的状态
    void setCurrentTextDeal(bool checked,const QString &itemtext);//重组显示数据

signals:
    void checkedStateChange(int index, bool bChecked);//状态改变后发送一个信号，告诉外界。

public slots:

};

#endif // RECHECKCOMBOBOX_H
