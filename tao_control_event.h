#ifndef TAOCONTROLEVENT_H
#define TAOCONTROLEVENT_H
// ****************************************************************************
//  taocontrolevent.h						    Tao project
// ****************************************************************************
//
//   File Description:
//
//     Events used to control tao presentaion.
//     Used to save and replay a test or
//     to send over the network and control a remote session.
//
//
//
//
//
//
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2011 Catherine Burvelle <cathy@taodyne.com>
//  (C) 2011 Taodyne SAS
// ****************************************************************************

#include <QWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>


// To not interfer with QEvent::Type, we can use the registerEventType()
// function, but we are not sure we will always have the same value, so as,
// those types won't be used in a QEvent inherited object, we decide to use
// values outside of the QEvent::Type range which is [0..QEvent::MaxUser]
#define ACTION_EVENT_TYPE  65536 // QEvent::MaxUser + 1
#define COLOR_EVENT_TYPE   65537 // QEvent::MaxUser + 2
#define FONT_EVENT_TYPE    65538 // QEvent::MaxUser + 3
#define FILE_EVENT_TYPE    65539 // QEvent::MaxUser + 4
#define DIALOG_EVENT_TYPE  65540 // QEvent::MaxUser + 5
#define CHECK_EVENT_TYPE   65541 // QEvent::MaxUser + 6

class TaoControlEvent
// ----------------------------------------------------------------------------
//   Ancestor class of all tao control event type.
// ----------------------------------------------------------------------------
// This class and descendent use sized type (quint32,...) to get read of
// type length diffence between different architecture.
{
public:
    TaoControlEvent(uint delay) : delay(delay){}
    virtual ~TaoControlEvent(){}

    // The output string is the xl command that represent the event.
    // It contains no indentation.
    virtual QString toTaoCmd() = 0;

    // Serialize and unserialize deal with delay and type serialization,
    // then call serializeData and unserializeData.
    // unserialize finaly return the object represented in the data.
    virtual QDataStream & serialize(QDataStream &out);
    virtual QDataStream & serializeData(QDataStream &out) = 0;
    static TaoControlEvent *unserialize(QDataStream &in);
    virtual QDataStream &unserializeData(QDataStream &in,
                                         quint32 e_type) = 0;

    // simulate this event with no delay
    virtual void simulateNow(QWidget*) = 0;

    // Run the current event on the widget
    static void simulateQEvent(QEvent *e, QWidget* w);

    // returns the event type
    virtual quint32 getType() = 0;

    quint32 getDelay() {return delay;}

protected:
    // The delay before running this event
    quint32 delay;

};


class TaoKeyEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store QKeyEvents
// ----------------------------------------------------------------------------
// The QKeyEvent contained in this class is allocated on the heap either by
// the constructor, or by the unserializeData method to
// let the postEvent method deals with it, and delete it.
{
public:
    TaoKeyEvent(QKeyEvent &event, uint delay)
        : TaoControlEvent(delay), event(new QKeyEvent(event)) {}

    virtual ~TaoKeyEvent()
    {
        delete event;
    }

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in,
                                          quint32 e_type);
    virtual void simulateNow(QWidget* w)
    {
        simulateQEvent(event, w);
    }
    virtual quint32 getType();

protected:
    QKeyEvent *event;
public:
    TaoKeyEvent(uint delay)
        : TaoControlEvent(delay), event(NULL) {}

};


class TaoMouseEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store QMouseEvents
// ----------------------------------------------------------------------------
// The QKeyEvent contained in this class is allocated on the heap either by
// the constructor, or by the unserializeData method to
// let the postEvent method deals with it, and delete it.
{
public:
    TaoMouseEvent(QMouseEvent &event, uint delay)
        : TaoControlEvent(delay), event(new QMouseEvent(event)) {}

    virtual ~TaoMouseEvent()
    {
        delete event;
    }

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in, quint32 e_type);
    virtual void simulateNow(QWidget* w)
    {
        simulateQEvent(event, w);
    }
    virtual quint32 getType();

protected:
    QMouseEvent *event;
public:
    TaoMouseEvent(uint delay)
        : TaoControlEvent(delay), event(NULL) {}

};



class TaoActionEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store QAction events with other mouse/key events
// ----------------------------------------------------------------------------
{
public:
    TaoActionEvent(QString name, int delay)
        : TaoControlEvent(delay), action_name(name) {}

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in, quint32 e_type);
    virtual void simulateNow(QWidget *w);

    virtual quint32 getType() { return ACTION_EVENT_TYPE;}

protected:
    QString action_name;
public:
    TaoActionEvent(uint delay)
        : TaoControlEvent(delay) {}

};


class TaoColorActionEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store color change events
// ----------------------------------------------------------------------------
{
public:
    TaoColorActionEvent(QString objName, QString name, int delay)
        : TaoControlEvent(delay), objName(objName), colorName(name) {}

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in, quint32 e_type);
    virtual void simulateNow(QWidget *w);

    virtual quint32 getType() { return COLOR_EVENT_TYPE;}

protected:
    QString objName;
    QString colorName;
public:
    TaoColorActionEvent(uint delay)
        : TaoControlEvent(delay) {}

};


class TaoFontActionEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store font change events.
// ----------------------------------------------------------------------------
{
public:
    TaoFontActionEvent(QString objName, QString name, int delay)
        : TaoControlEvent(delay), objName(objName), fontName(name) {}

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in, quint32 e_type);
    virtual void simulateNow(QWidget *w);

    virtual quint32 getType() { return FONT_EVENT_TYPE;}

protected:
    QString objName;
    QString fontName;

public:
    TaoFontActionEvent(uint delay)
        : TaoControlEvent(delay) {}
};

class TaoFileActionEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store file selection events.
// ----------------------------------------------------------------------------
{
public:
    TaoFileActionEvent(QString objName, QString name, int delay)
        : TaoControlEvent(delay), objName(objName), fileName(name) {}

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in, quint32 e_type);
    virtual void simulateNow(QWidget *w);

    virtual quint32 getType() { return FILE_EVENT_TYPE;}

protected:
    QString objName;
    QString fileName;
public:
    TaoFileActionEvent(uint delay)
        : TaoControlEvent(delay) {}

};


class TaoDialogActionEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store QAction events with other mouse/key events
// ----------------------------------------------------------------------------
{
public:
    TaoDialogActionEvent(QString objName, int result, int delay)
        : TaoControlEvent(delay), objName(objName), result(result) {}

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in, quint32 e_type);
    virtual void simulateNow(QWidget *w);

    virtual quint32 getType() { return DIALOG_EVENT_TYPE;}

protected:
    QString objName;
    qint32  result;
public:
    TaoDialogActionEvent(uint delay)
        : TaoControlEvent(delay) {}

};


class TaoCheckEvent: public TaoControlEvent
// ----------------------------------------------------------------------------
//   Class used to store QAction events with other mouse/key events
// ----------------------------------------------------------------------------
{
public:
    TaoCheckEvent(int num, QImage* image, int delay)
        : TaoControlEvent(delay), number(num), image(image) {}

    virtual QString toTaoCmd();

    virtual QDataStream & serializeData(QDataStream &out);
    virtual QDataStream & unserializeData(QDataStream &in, quint32 e_type);
    virtual void simulateNow(QWidget *w);

    virtual quint32 getType() { return CHECK_EVENT_TYPE;}

protected:
    qint16 number;
    QImage *image;
public:
    TaoCheckEvent(uint delay)
        : TaoControlEvent(delay) {}

};

#endif // TAOCONTROLEVENT_H
