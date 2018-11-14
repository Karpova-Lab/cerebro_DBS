/****************************************************************************
** Meta object code from reading C++ file 'settingsDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "settingsDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settingsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_settingsDialog_t {
    QByteArrayData data[12];
    char stringdata0[123];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_settingsDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_settingsDialog_t qt_meta_stringdata_settingsDialog = {
    {
QT_MOC_LITERAL(0, 0, 14), // "settingsDialog"
QT_MOC_LITERAL(1, 15, 12), // "dialogClosed"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 6), // "myexec"
QT_MOC_LITERAL(4, 36, 7), // "setPath"
QT_MOC_LITERAL(5, 44, 10), // "removeItem"
QT_MOC_LITERAL(6, 55, 11), // "addListItem"
QT_MOC_LITERAL(7, 67, 12), // "openSettings"
QT_MOC_LITERAL(8, 80, 8), // "addAlias"
QT_MOC_LITERAL(9, 89, 11), // "removeAlias"
QT_MOC_LITERAL(10, 101, 9), // "editLabel"
QT_MOC_LITERAL(11, 111, 11) // "saveChanges"

    },
    "settingsDialog\0dialogClosed\0\0myexec\0"
    "setPath\0removeItem\0addListItem\0"
    "openSettings\0addAlias\0removeAlias\0"
    "editLabel\0saveChanges"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_settingsDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   65,    2, 0x0a /* Public */,
       4,    0,   66,    2, 0x0a /* Public */,
       5,    0,   67,    2, 0x0a /* Public */,
       6,    0,   68,    2, 0x0a /* Public */,
       7,    0,   69,    2, 0x0a /* Public */,
       8,    0,   70,    2, 0x0a /* Public */,
       9,    0,   71,    2, 0x0a /* Public */,
      10,    0,   72,    2, 0x0a /* Public */,
      11,    0,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void settingsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        settingsDialog *_t = static_cast<settingsDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dialogClosed(); break;
        case 1: _t->myexec(); break;
        case 2: _t->setPath(); break;
        case 3: _t->removeItem(); break;
        case 4: _t->addListItem(); break;
        case 5: _t->openSettings(); break;
        case 6: _t->addAlias(); break;
        case 7: _t->removeAlias(); break;
        case 8: _t->editLabel(); break;
        case 9: _t->saveChanges(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (settingsDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&settingsDialog::dialogClosed)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject settingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_settingsDialog.data,
      qt_meta_data_settingsDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *settingsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *settingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_settingsDialog.stringdata0))
        return static_cast<void*>(const_cast< settingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int settingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void settingsDialog::dialogClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
