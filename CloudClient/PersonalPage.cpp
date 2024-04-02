#include "PersonalPage.h"
#include "ui_PersonalPage.h"

PersonalPage::PersonalPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PersonalPage)
{
    ui->setupUi(this);
}

PersonalPage::~PersonalPage()
{
    delete ui;
}
