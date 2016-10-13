#生成文件类型
TEMPLATE = app
#生成对象名
TARGET = FKWebBrowser
#依赖的QT库
QT += webenginewidgets network widgets core gui
#源文件
SOURCES += \
    main.cpp \
    fkbrowserapp.cpp \
    fkcookiejarmgr.cpp \
    fkautosaver.cpp \
    fkcookiemodel.cpp \
    fkcookiesdialog.cpp \
    fkcookieexceptionsdialog.cpp \
    fkcookieexceptionsmodel.cpp \
    fkdownloadwidget.cpp \
    fkdownloadmodel.cpp \
    fkui_edittableview.cpp \
    fkdownloaddialog.cpp \
    fkui_squeezelabel.cpp \
    fkui_clearbutton.cpp \
    fkui_exlineedit.cpp \
    fkui_urliconlabel.cpp \
    fkui_webpage.cpp \
    fkui_popupwindow.cpp \
    fkui_webview.cpp \
    fkui_featurepermissionbar.cpp \
    fkui_tabbar.cpp \
    fkui_fullscreennotification.cpp \
    fkwebactionmapper.cpp \
    fkui_tabwidget.cpp \
    fkui_urllineedit.cpp \
    fkui_savepagedialog.cpp \
    fkhistoryitem.cpp \
    fkhistorymodel.cpp \
    fkhistorymgr.cpp \
    fkhistoryfiltermodel.cpp \
    fkhistorymenumodel.cpp \
    fkui_modelmenu.cpp \
    fkhistorymenu.cpp \
    fkhistorycompletionmodel.cpp \
    fktreeproxymodel.cpp \
    fkhistorytreemodel.cpp \
    fkhistorydialog.cpp \
    fkui_edittreeview.cpp \
    fkui_searchbutton.cpp \
    fkui_searchlineedit.cpp \
    fkbookmarksremovecommand.cpp \
    fkbookmarknode.cpp \
    fkbookmarksinsertcommand.cpp \
    fkbookmarkschangecommand.cpp \
    fkbookmarksmodel.cpp \
    fkfreader.cpp \
    fkfwriter.cpp \
    fkbookmarksmenu.cpp \
    fkaddbookmarkproxymodel.cpp \
    fkbookmarkstoolbar.cpp \
    fkbrowsermainwindow.cpp \
    fkaddbookmarkdialog.cpp \
    fkbookmarksdialog.cpp \
    fkbookmarksmgr.cpp \
    fkui_chasewidget.cpp \
    fkui_toolbarsearch.cpp \
    fksettingdialog.cpp \
    fkprinttopdfdialog.cpp
#头文件
HEADERS += \
    fkbrowserapp.h \
    fkbrowsermainwindow.h \
    fkcookiejarmgr.h \
    fkautosaver.h \
    fkcookiemodel.h \
    fkcookiesdialog.h \
    fkcookieexceptionsmodel.h \
    fkcookieexceptionsdialog.h \
    fkdownloadwidget.h \
    fkdownloaddialog.h \
    fkdownloadmodel.h \
    fkui_edittableview.h \
    fkui_squeezelabel.h \
    fkui_exlineedit.h \
    fkui_clearbutton.h \
    fkui_urliconlabel.h \
    fkui_webpage.h \
    fkui_popupwindow.h \
    fkui_webview.h \
    fkui_featurepermissionbar.h \
    fkui_tabbar.h \
    fkui_fullscreennotification.h \
    fkui_tabwidget.h \
    fkwebactionmapper.h \
    fkui_urllineedit.h \
    fkui_savepagedialog.h \
    fkhistoryitem.h \
    fkhistorymodel.h \
    fkhistorymgr.h \
    fkhistoryfiltermodel.h \
    fkhistorymenumodel.h \
    fkhistorymenu.h \
    fkui_modelmenu.h \
    fkhistorycompletionmodel.h \
    fktreeproxymodel.h \
    fkhistorytreemodel.h \
    fkhistorydialog.h \
    fkui_edittreeview.h \
    fkui_searchlineedit.h \
    fkui_searchbutton.h \
    fkbookmarksmgr.h \
    fkbookmarksinsertcommand.h \
    fkbookmarksremovecommand.h \
    fkbookmarknode.h \
    fkbookmarkschangecommand.h \
    fkbookmarksmodel.h \
    fkfreader.h \
    fkfwriter.h \
    fkbookmarksmenu.h \
    fkaddbookmarkproxymodel.h \
    fkbookmarkstoolbar.h \
    fkaddbookmarkdialog.h \
    fkbookmarksdialog.h \
    fkui_chasewidget.h \
    fkui_toolbarsearch.h \
    fksettingdialog.h \
    fkprinttopdfdialog.h

RESOURCES += \
    Res/res.qrc

FORMS += \
    cookies.ui \
    cookiesexceptions.ui \
    downloaditem.ui \
    downloads.ui \
    passworddialog.ui \
    proxy.ui \
    savepagedialog.ui \
    history.ui \
    addbookmarkdialog.ui \
    bookmarks.ui \
    settings.ui \
    printtopdfdialog.ui
