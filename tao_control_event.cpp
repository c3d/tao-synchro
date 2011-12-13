// ****************************************************************************
//  taocontrolevent.cpp						    Tao project
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
#include "tao_control_event.h"
#include "tao_synchro.h"
#include <QApplication>

#include <QAction>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <iostream>


void TaoControlEvent::simulateQEvent(QEvent *e, QWidget* w)
// ----------------------------------------------------------------------------
//   Will post the QEvent onto the event queue for the QWidget
// ----------------------------------------------------------------------------
{
    // No provided widget, will try to get the one with the keyboard focus.
    if (!w)
        w = QWidget::keyboardGrabber();

    if (!w)
        return;

    qApp->postEvent(w, e);
}


QDataStream & TaoControlEvent::serialize(QDataStream &out)
// ----------------------------------------------------------------------------
//   Serialize delay and event type, then data using the virtual method
// ----------------------------------------------------------------------------
{
    out << delay;
    out << getType();
    return serializeData(out);
}


TaoControlEvent *TaoControlEvent::unserialize(QDataStream &in)
// ----------------------------------------------------------------------------
//   Unserialize delay and event type, then data.
// ----------------------------------------------------------------------------
// Data are unserialized by the specific class.
// Presence and length of data is not checked because it will consume too
// much time. We assume the content is good due to initial handshake.
{
    // Read delay
    quint32 delay = 0;
    in >> delay;
    // Read event type from stream;
    quint32 e_type = 0;
    in >> e_type;
    // Call the unserializeData of that type
    TaoControlEvent *evt;
    switch (e_type) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        evt = new TaoKeyEvent(delay);
        break;
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
        evt = new TaoMouseEvent(delay);
        break;
    case ACTION_EVENT_TYPE:
        evt = new TaoActionEvent(delay);
        break;
    case COLOR_EVENT_TYPE:
        evt = new TaoColorActionEvent(delay);
        break;
    case FONT_EVENT_TYPE:
        evt = new TaoFontActionEvent(delay);
        break;
    case FILE_EVENT_TYPE:
        evt = new TaoFileActionEvent(delay);
        break;
    case DIALOG_EVENT_TYPE:
        evt = new TaoDialogActionEvent(delay);
        break;
    case CHECK_EVENT_TYPE:
        evt = new TaoCheckEvent(delay);
        break;
    default:
        return NULL;
    }
    evt->unserializeData(in, e_type);
    return evt;
}


// ****************************************************************************
//   TaoKeyEvent Class
// ****************************************************************************

quint32 TaoKeyEvent::getType()
// ----------------------------------------------------------------------------
//   Tao control event type
// ----------------------------------------------------------------------------
{
    return event->type();
}


QString TaoKeyEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString cmd = QString("test_add_key_%5 %1, %2, %3 // %4\n")
                  .arg(event->key()).arg(event->modifiers()).arg(delay)
                  .arg(event->text())
                  .arg((event->type() == QEvent::KeyPress ?
                        "press" : "release"));
    return cmd;
}


QDataStream &TaoKeyEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    out << (quint32) event->key();
    out << (quint32) event->modifiers();
    out << event->text();
    out << event->isAutoRepeat();
    out << (quint16) event->count();

    return out;
}


QDataStream & TaoKeyEvent::unserializeData(QDataStream &in,
                                           quint32 e_type)
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    Qt::Key               key;
    Qt::KeyboardModifiers modifiers;
    QString               k_text;
    bool                  auto_repeat;
    ushort                count;

    in >> (quint32&) key;
    in >> (quint32&) modifiers;
    in >> k_text;
    in >> auto_repeat;
    in >> (quint16&) count;

    event = new QKeyEvent ((QEvent::Type)e_type, key, modifiers, k_text,
                           auto_repeat, count);

    return in;
}


// ****************************************************************************
//   TaoMouseEvent Class
// ****************************************************************************

quint32 TaoMouseEvent::getType()
// ----------------------------------------------------------------------------
//   Tao control event type
// ----------------------------------------------------------------------------
{
    return event->type();
}


QString TaoMouseEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString action;
    switch (event->type())
    {
    case QEvent::MouseButtonPress:    action = "press"  ; break;
    case QEvent::MouseButtonRelease:  action = "release"; break;
    case QEvent::MouseMove:           action = "move"   ; break;
    case QEvent::MouseButtonDblClick: action = "dclick" ; break;
    default: action = "unknown";
    }

    QString cmd = QString("test_add_mouse_%6 %1, %2, %3, %4, %5\n")
                  .arg(event->button()).arg(event->modifiers())
                  .arg(event->pos().x()).arg(event->pos().y())
                  .arg(delay).arg(action);
    return cmd;
}


QDataStream &TaoMouseEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    qint32 ux = 0, uy = 0, uz = 0;
    synchroBasic::base->unproject(event->x(), event->y(), 0, &ux, &uy, &uz);
    out << ux;
    out << uy;
    out << (quint32) event->button();
    out << (quint32) event->buttons();
    out << (quint32) event->modifiers();

    return out;
}


QDataStream & TaoMouseEvent::unserializeData(QDataStream &in,
                                             quint32 e_type)
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    int px = 0, py = 0, pz = 0;
    qint32 ux = 0, uy = 0;
    Qt::MouseButton button;
    Qt::MouseButtons buttons;
    Qt::KeyboardModifiers modifiers;
    in >> ux;
    in >> uy;
    in >> (quint32&) button;
    in >> (quint32&) buttons;
    in >> (quint32&) modifiers;
    synchroBasic::base->project(ux, uy, 0, &px, &py, &pz);
    QPoint pos(px, py);
    event = new QMouseEvent ((QEvent::Type)e_type, pos, button,
                             buttons, modifiers);

    return in;
}



// ****************************************************************************
//   TaoActionEvent Class
// ****************************************************************************

QString TaoActionEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString cmd = QString("test_add_action \"%1\", %2 \n")
                  .arg(action_name).arg(delay);
    return cmd;
}


QDataStream &TaoActionEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    out << action_name;
    return out;
}


QDataStream & TaoActionEvent::unserializeData(QDataStream &in,
                                             quint32 )
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    in >> action_name;
    return in;
}


void TaoActionEvent::simulateNow(QWidget *w)
// ----------------------------------------------------------------------------
//  Runs immediatly the action associated with this event.
// ----------------------------------------------------------------------------
{
    QAction* act = w->parent()->findChild<QAction*>(action_name);
    if (act)
        act->activate(QAction::Trigger);
}



// ****************************************************************************
//   TaoColorActionEvent Class
// ****************************************************************************

QString TaoColorActionEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString cmd = QString("test_add_color \"%1\", \"%2\", %3, %4\n")
                  .arg(objName).arg(colorName).arg(alpha).arg(delay);
    return cmd;
}


QDataStream &TaoColorActionEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    out << objName;
    out << colorName;
    out << alpha;
    return out;
}


QDataStream & TaoColorActionEvent::unserializeData(QDataStream &in,
                                                   quint32)
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    in >> objName;
    in >> colorName;
    in >> alpha;
    return in;
}


void TaoColorActionEvent::simulateNow(QWidget *w)
// ----------------------------------------------------------------------------
//  Runs immediatly the action associated with this event.
// ----------------------------------------------------------------------------
{
    QColorDialog* diag = w->findChild<QColorDialog*>(objName);
    QColor col(colorName);
    col.setAlphaF(alpha);
    if (diag &&col.isValid())
        diag->setCurrentColor(col);

}



// ****************************************************************************
//   TaoFontActionEvent Class
// ****************************************************************************

QString TaoFontActionEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString cmd = QString("test_add_font \"%1\", \"%2\", %3\n")
                  .arg(objName).arg(fontName).arg(delay);
    return cmd;
}


QDataStream &TaoFontActionEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    out << objName;
    out << fontName;
    return out;
}


QDataStream & TaoFontActionEvent::unserializeData(QDataStream &in,
                                                  quint32)
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    in >> objName;
    in >> fontName;
    return in;
}


void TaoFontActionEvent::simulateNow(QWidget *w)
// ----------------------------------------------------------------------------
//  Runs immediatly the action associated with this event.
// ----------------------------------------------------------------------------
{
    QFontDialog* diag = w->findChild<QFontDialog*>(objName);
    QFont ft;
    ft.fromString(fontName);
    if (diag )
        diag->setCurrentFont(ft);
}


// ****************************************************************************
//   TaoFileActionEvent Class
// ****************************************************************************

QString TaoFileActionEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString cmd = QString("test_add_file \"%1\", \"%2\", %3\n")
                  .arg(objName).arg(fileName).arg(delay);
    return cmd;
}


QDataStream &TaoFileActionEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    out << objName;
    out << fileName;
    return out;
}


QDataStream & TaoFileActionEvent::unserializeData(QDataStream &in,
                                                  quint32)
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    in >> objName;
    in >> fileName;
    return in;
}


void TaoFileActionEvent::simulateNow(QWidget *w)
// ----------------------------------------------------------------------------
//  Runs immediatly the action associated with this event.
// ----------------------------------------------------------------------------
{
    QFileDialog* diag = w->findChild<QFileDialog*>(objName);
    if (diag )
    {
        diag->setVisible(false);
        diag->selectFile(fileName);
        QDialog *d = (QDialog *) diag;
        d->accept();
    }
}



// ****************************************************************************
//   TaoDialogActionEvent Class
// ****************************************************************************

QString TaoDialogActionEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString cmd = QString("test_dialog_action \"%1\", \"%2\", %3\n")
                  .arg(objName).arg(result).arg(delay);
    return cmd;
}


QDataStream &TaoDialogActionEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    out << objName;
    out << result;
    return out;
}


QDataStream & TaoDialogActionEvent::unserializeData(QDataStream &in,
                                                    quint32)
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    in >> objName;
    in >> result;
    return in;
}


void TaoDialogActionEvent::simulateNow(QWidget *w)
// ----------------------------------------------------------------------------
//  Runs immediatly the action associated with this event.
// ----------------------------------------------------------------------------
{
    QDialog* diag = w->findChild<QDialog*>(objName);

    if (diag)
        diag->done(result);
 }


// ****************************************************************************
//   TaoCheckEvent Class
// NOT YET USED nor finished
// ****************************************************************************

QDataStream &TaoCheckEvent::serializeData(QDataStream &out)
// ----------------------------------------------------------------------------
//  Serialize event specific data.
// ----------------------------------------------------------------------------
{
    out << number;
    out << image;
    return out;
}

QDataStream & TaoCheckEvent::unserializeData(QDataStream &in,
                                             quint32)
// ----------------------------------------------------------------------------
//  Unserialize event specific data.
// ----------------------------------------------------------------------------
{
    in >> number;
    image = new QImage;
    in >> *image;
    return in;
}


void TaoCheckEvent::simulateNow(QWidget */*w*/)
// ----------------------------------------------------------------------------
//  Perform a check againts the reference view.
// ----------------------------------------------------------------------------
{
    /*
    QGLWidget * widget = (QGLWidget *)w;
    QString testName = taoTester::tester()->currentTest()->name;
    QFileInfo refFile(QString("image:%1/%1_%2.png").arg(testName).arg(number));
    QImage ref(refFile.canonicalFilePath());
    QImage shot = widget->grabFrameBuffer(true);

    shot.save(QString("%1/%2_played_%3.png")
              .arg(refFile.canonicalPath()).arg(testName).arg(number), "PNG");

    QString diffFilename = QString("%1/%2_diff_%3.png").arg(refFile.canonicalPath())
                           .arg(testName).arg(number);
    double resDiff = taoTester::tester()->currentTest()->diff(ref, shot, diffFilename);

    std::cerr << +testName <<  "\t Intermediate check " << number ;
    bool inError = resDiff > taoTester::tester()->currentTest()->threshold;
    if (inError)
    {
        std::cerr<< " fails. Diff is " << resDiff << "%\n";
        taoTester::tester()->currentTest()->nbChkPtKO++;
    }
    else
        std::cerr<< " succeeds. Diff is " << resDiff << "%\n";
    taoTester::tester()->currentTest()->log(number, !inError, resDiff);
    */
}


QString TaoCheckEvent::toTaoCmd()
// ----------------------------------------------------------------------------
//  Return a command line for Tao.
// ----------------------------------------------------------------------------
{
    QString cmd = QString("test_add_check %1, %2\n").arg(number).arg(delay);
    // TEST add image save.
    return cmd;
}
