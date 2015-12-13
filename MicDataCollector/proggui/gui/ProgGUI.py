# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Progress.ui'
#
# Created by: PyQt4 UI code generator 4.11.4
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

class Ui_progDlg(object):
    def setupUi(self, progDlg):
        progDlg.setObjectName(_fromUtf8("progDlg"))
        progDlg.resize(442, 64)
        self.verticalLayout = QtGui.QVBoxLayout(progDlg)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.loadingLbl = QtGui.QLabel(progDlg)
        self.loadingLbl.setObjectName(_fromUtf8("loadingLbl"))
        self.verticalLayout.addWidget(self.loadingLbl)
        self.progHLayout = QtGui.QHBoxLayout()
        self.progHLayout.setObjectName(_fromUtf8("progHLayout"))
        self.progressBar = QtGui.QProgressBar(progDlg)
        self.progressBar.setProperty("value", 24)
        self.progressBar.setObjectName(_fromUtf8("progressBar"))
        self.progHLayout.addWidget(self.progressBar)
        self.cancelBtn = QtGui.QPushButton(progDlg)
        self.cancelBtn.setObjectName(_fromUtf8("cancelBtn"))
        self.progHLayout.addWidget(self.cancelBtn)
        self.verticalLayout.addLayout(self.progHLayout)

        self.retranslateUi(progDlg)
        QtCore.QMetaObject.connectSlotsByName(progDlg)

    def retranslateUi(self, progDlg):
        progDlg.setWindowTitle(_translate("progDlg", "Loading...", None))
        self.loadingLbl.setText(_translate("progDlg", "Loading...", None))
        self.cancelBtn.setText(_translate("progDlg", "Cancel", None))

