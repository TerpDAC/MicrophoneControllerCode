# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MicDataCollectorGUI.ui'
#
# Created: Fri Dec 11 18:44:12 2015
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_mdcDialog(object):
    def setupUi(self, mdcDialog):
        mdcDialog.setObjectName(_fromUtf8("mdcDialog"))
        mdcDialog.resize(253, 140)
        self.verticalLayout = QtGui.QVBoxLayout(mdcDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.serialPortLbl = QtGui.QLabel(mdcDialog)
        self.serialPortLbl.setObjectName(_fromUtf8("serialPortLbl"))
        self.verticalLayout.addWidget(self.serialPortLbl)
        self.serialPortCombo = QtGui.QComboBox(mdcDialog)
        self.serialPortCombo.setObjectName(_fromUtf8("serialPortCombo"))
        self.verticalLayout.addWidget(self.serialPortCombo)
        self.refreshBtn = QtGui.QPushButton(mdcDialog)
        self.refreshBtn.setObjectName(_fromUtf8("refreshBtn"))
        self.verticalLayout.addWidget(self.refreshBtn)
        self.startBtn = QtGui.QPushButton(mdcDialog)
        self.startBtn.setObjectName(_fromUtf8("startBtn"))
        self.verticalLayout.addWidget(self.startBtn)

        self.retranslateUi(mdcDialog)
        QtCore.QMetaObject.connectSlotsByName(mdcDialog)

    def retranslateUi(self, mdcDialog):
        mdcDialog.setWindowTitle(_translate("mdcDialog", "Mic Data Collector", None))
        self.serialPortLbl.setText(_translate("mdcDialog", "Select the serial port:", None))
        self.refreshBtn.setText(_translate("mdcDialog", "Refresh Serial Ports", None))
        self.startBtn.setText(_translate("mdcDialog", "Start!", None))

