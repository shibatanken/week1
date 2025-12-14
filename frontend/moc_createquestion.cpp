/****************************************************************************
** Meta object code from reading C++ file 'createquestion.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "createquestion.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'createquestion.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CreateQuestion_t {
    QByteArrayData data[11];
    char stringdata0[196];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CreateQuestion_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CreateQuestion_t qt_meta_stringdata_CreateQuestion = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CreateQuestion"
QT_MOC_LITERAL(1, 15, 15), // "questionCreated"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 9), // "setRoomId"
QT_MOC_LITERAL(4, 42, 6), // "roomId"
QT_MOC_LITERAL(5, 49, 24), // "onAddAnswerButtonClicked"
QT_MOC_LITERAL(6, 74, 27), // "onDeleteAnswerButtonClicked"
QT_MOC_LITERAL(7, 102, 21), // "onSaveQuestionClicked"
QT_MOC_LITERAL(8, 124, 21), // "updateSaveButtonState"
QT_MOC_LITERAL(9, 146, 20), // "handleCreateQuestion"
QT_MOC_LITERAL(10, 167, 28) // "handleCreateQuestionResponse"

    },
    "CreateQuestion\0questionCreated\0\0"
    "setRoomId\0roomId\0onAddAnswerButtonClicked\0"
    "onDeleteAnswerButtonClicked\0"
    "onSaveQuestionClicked\0updateSaveButtonState\0"
    "handleCreateQuestion\0handleCreateQuestionResponse"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CreateQuestion[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   55,    2, 0x0a /* Public */,
       5,    0,   58,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    0,   60,    2, 0x08 /* Private */,
       8,    0,   61,    2, 0x08 /* Private */,
       9,    0,   62,    2, 0x08 /* Private */,
      10,    0,   63,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CreateQuestion::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CreateQuestion *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->questionCreated(); break;
        case 1: _t->setRoomId((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onAddAnswerButtonClicked(); break;
        case 3: _t->onDeleteAnswerButtonClicked(); break;
        case 4: _t->onSaveQuestionClicked(); break;
        case 5: _t->updateSaveButtonState(); break;
        case 6: _t->handleCreateQuestion(); break;
        case 7: _t->handleCreateQuestionResponse(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CreateQuestion::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CreateQuestion::questionCreated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CreateQuestion::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CreateQuestion.data,
    qt_meta_data_CreateQuestion,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CreateQuestion::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CreateQuestion::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CreateQuestion.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CreateQuestion::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void CreateQuestion::questionCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
