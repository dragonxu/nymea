#ifndef PYSTDOUTHANDLER_H
#define PYSTDOUTHANDLER_H

#include <Python.h>
#include "structmember.h"

#include <QStringList>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dcPythonIntegrations)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

typedef struct {
    PyObject_HEAD
    char *category;
    QtMsgType msgType;
} PyStdOutHandler;

static int PyStdOutHandler_init(PyStdOutHandler *self, PyObject *args, PyObject */*kwds*/)
{
    qCDebug(dcPythonIntegrations()) << "+++ PyStdOutHandler";
    char *category = nullptr;
    QtMsgType msgType;
    if (!PyArg_ParseTuple(args, "si", &category, &msgType)) {
        qCWarning(dcPythonIntegrations()) << "PyStdOutHandler: Error parsing parameters";
        return -1;
    }

    self->category = (char*)malloc(qstrlen(category));
    self->msgType = msgType;
    qstrcpy(self->category, category);

    return 0;
}

static void PyStdOutHandler_dealloc(PyStdOutHandler * self)
{
    qCDebug(dcPythonIntegrations()) << "--- PyStdOutHandler";
    free(self->category);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* PyStdOutHandler_write(PyStdOutHandler* self, PyObject* args)
{
    const char *what;
    if (!PyArg_ParseTuple(args, "s", &what))
        return nullptr;
    if (!QByteArray(what).trimmed().isEmpty()) {
        switch (self->msgType) {
        case QtMsgType::QtInfoMsg:
            qCInfo(QLoggingCategory(self->category)) << what;
            break;
        case QtMsgType::QtDebugMsg:
            qCDebug(QLoggingCategory(self->category)) << what;
            break;
        case QtMsgType::QtWarningMsg:
            qCWarning(QLoggingCategory(self->category)) << what;
            break;
        case QtMsgType::QtCriticalMsg:
            qCCritical(QLoggingCategory(self->category)) << what;
            break;
        default:
            qCDebug(QLoggingCategory(self->category)) << what;
            break;
        }
    }
    Py_RETURN_NONE;
}

static PyObject* PyStdOutHandler_flush(PyObject* /*self*/, PyObject* /*args*/)
{
    // Not really needed... QDebug flushes already on its own
    Py_RETURN_NONE;
}

static PyMethodDef PyStdOutHandler_methods[] = {
    { "write", (PyCFunction)PyStdOutHandler_write, METH_VARARGS, "Writes to stdout through qDebug()"},
    { "flush", (PyCFunction)PyStdOutHandler_flush, METH_VARARGS, "no-op"},
    {nullptr, nullptr, 0, nullptr} // sentinel
};

static PyTypeObject PyStdOutHandlerType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "nymea.StdOutHandler",   /* tp_name */
    sizeof(PyStdOutHandler), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyStdOutHandler_dealloc,/* tp_dealloc */
};

static void registerStdOutHandler(PyObject *module)
{

    PyStdOutHandlerType.tp_new = PyType_GenericNew;
    PyStdOutHandlerType.tp_init = reinterpret_cast<initproc>(PyStdOutHandler_init);
    PyStdOutHandlerType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyStdOutHandlerType.tp_methods = PyStdOutHandler_methods;
    PyStdOutHandlerType.tp_doc = "Logging handler for nymea.";

    if (PyType_Ready(&PyStdOutHandlerType) == 0) {
        PyModule_AddObject(module, "NymeaLoggingHandler", (PyObject *)&PyStdOutHandlerType);
    }
}

#endif // PYSTDOUTHANDLER_H
