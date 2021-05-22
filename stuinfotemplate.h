#ifndef STUINFOTEMPLATE_H
#define STUINFOTEMPLATE_H
#include <QString>
#include <QDebug>

class StuInfoTemplate
{
public:
    StuInfoTemplate();
    StuInfoTemplate(QString ID,QString name,QString sex,QString birth,QString home,QString add);

    void showStuInfo();
    void setID(const QString &value);
    void setName(const QString &value);
    void setSex(const QString &value);
    void setBirthday(const QString &value);
    void setHometown(const QString &value);
    void setAddress(const QString &value);

    QString getID() const;
    QString getName() const;
    QString getSex() const;
    QString getBirthday() const;
    QString getHometown() const;
    QString getAddress() const;

private:
    QString ID;
    QString name;
    QString sex;
    QString birthday;
    QString hometown;
    QString address;
};

#endif // STUINFOTEMPLATE_H
