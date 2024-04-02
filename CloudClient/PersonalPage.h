#ifndef PERSONALPAGE_H
#define PERSONALPAGE_H

#include <QWidget>

namespace Ui {
class PersonalPage;
}

class PersonalPage : public QWidget
{
    Q_OBJECT

public:
    explicit PersonalPage(QWidget *parent = nullptr);//构造函数

    ~PersonalPage();//析构函数

private:
    Ui::PersonalPage *ui;
};

#endif // PERSONALPAGE_H
