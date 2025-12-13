#ifndef USERDATA_H
#define USERDATA_H

#include <QString>
#include <QJsonObject>
#include <QDebug>

class UserData
{
public:
    static UserData& instance() {
        static UserData instance;
        return instance;
    }

    void setUserData(const QJsonObject &data) {
        userData = data;
        qDebug() << "UserData set:" << userData;
    }

    QJsonObject getUserData() const {
        return userData;
    }

    int getUserId() const {
        if (userData.contains("user_id")) {
            return userData["user_id"].toInt();
        }
        return -1;
    }

    QString getRole() const {
        if (userData.contains("role")) {
            return userData["role"].toString();
        }
        return "student"; // Default role
    }

    bool isTeacher() const {
        return getRole() == "teacher";
    }

    bool isStudent() const {
        return getRole() == "student";
    }

    void reset() {
        userData = QJsonObject();
    }

private:
    UserData() {}
    ~UserData() {}
    UserData(const UserData&) = delete;
    UserData& operator=(const UserData&) = delete;

    QJsonObject userData;
};

#endif // USERDATA_H
