//
// PROJECT:         Aspia Remote Desktop
// FILE:            ui/file_manager_panel.h
// LICENSE:         See top-level directory
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_UI__FILE_MANAGER_PANEL_H
#define _ASPIA_UI__FILE_MANAGER_PANEL_H

#include "base/macros.h"
#include "proto/file_transfer_session.pb.h"
#include "ui/resource.h"

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlmisc.h>
#include <memory>

namespace aspia {

class UiFileManagerPanel : public CWindowImpl<UiFileManagerPanel, CWindow>
{
public:
    enum class PanelType { LOCAL, REMOTE };

    class Delegate
    {
    public:
        virtual void OnDriveListRequest(PanelType panel_type) = 0;

        virtual void OnDirectoryListRequest(PanelType _panel_type,
                                            const std::string& path,
                                            const std::string& name) = 0;

        virtual void OnCreateDirectoryRequest(PanelType panel_type,
                                              const std::string& path,
                                              const std::string& name) = 0;

        virtual void OnRenameRequest(PanelType panel_type,
                                     const std::string& path,
                                     const std::string& old_name,
                                     const std::string& new_name) = 0;

        virtual void OnRemoveRequest(PanelType panel_type,
                                     const std::string& path,
                                     const std::string& item_name) = 0;
    };

    UiFileManagerPanel(PanelType panel_type,
                       Delegate* delegate);
    virtual ~UiFileManagerPanel() = default;

    void ReadDriveList(std::unique_ptr<proto::DriveList> drive_list);
    void ReadDirectoryList(std::unique_ptr<proto::DirectoryList> directory_list);

private:
    static const int kToolBarControl = 100;
    static const int kDriveControl = 101;
    static const int kListControl = 101;

    BEGIN_MSG_MAP(UiFileManagerPanel)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        NOTIFY_CODE_HANDLER(TTN_GETDISPINFOW, OnGetDispInfo)
        NOTIFY_HANDLER(kDriveControl, CBEN_ENDEDITW, OnDriveEndEdit)
        NOTIFY_HANDLER(kListControl, NM_DBLCLK, OnListDoubleClock)
        NOTIFY_HANDLER(kListControl, LVN_ENDLABELEDIT, OnListEndLabelEdit)
        NOTIFY_HANDLER(kListControl, LVN_ITEMCHANGED, OnListItemChanged)

        COMMAND_HANDLER(kDriveControl, CBN_SELCHANGE, OnDriveChange)
        COMMAND_ID_HANDLER(ID_FOLDER_UP, OnFolderUp)
        COMMAND_ID_HANDLER(ID_FOLDER_ADD, OnFolderAdd)
        COMMAND_ID_HANDLER(ID_REFRESH, OnRefresh)
        COMMAND_ID_HANDLER(ID_DELETE, OnRemove)
        COMMAND_ID_HANDLER(ID_HOME, OnHome)
        COMMAND_ID_HANDLER(ID_SEND, OnSend)
    END_MSG_MAP()

    LRESULT OnCreate(UINT message, WPARAM wparam, LPARAM lparam, BOOL& handled);
    LRESULT OnDestroy(UINT message, WPARAM wparam, LPARAM lparam, BOOL& handled);
    LRESULT OnSize(UINT message, WPARAM wparam, LPARAM lparam, BOOL& handled);
    LRESULT OnDrawItem(UINT message, WPARAM wparam, LPARAM lparam, BOOL& handled);
    LRESULT OnGetDispInfo(int control_id, LPNMHDR hdr, BOOL& handled);
    LRESULT OnDriveEndEdit(int control_id, LPNMHDR hdr, BOOL& handled);
    LRESULT OnListDoubleClock(int control_id, LPNMHDR hdr, BOOL& handled);
    LRESULT OnListEndLabelEdit(int control_id, LPNMHDR hdr, BOOL& handled);
    LRESULT OnListItemChanged(int control_id, LPNMHDR hdr, BOOL& handled);

    LRESULT OnDriveChange(WORD notify_code, WORD control_id, HWND control, BOOL& handled);
    LRESULT OnFolderUp(WORD notify_code, WORD control_id, HWND control, BOOL& handled);
    LRESULT OnFolderAdd(WORD notify_code, WORD control_id, HWND control, BOOL& handled);
    LRESULT OnRefresh(WORD notify_code, WORD control_id, HWND control, BOOL& handled);
    LRESULT OnRemove(WORD notify_code, WORD control_id, HWND control, BOOL& handled);
    LRESULT OnHome(WORD notify_code, WORD control_id, HWND control, BOOL& handled);
    LRESULT OnSend(WORD notify_code, WORD control_id, HWND control, BOOL& handled);

    bool IsValidDirectoryObjectIndex(int object_index);
    void MoveToDrive(int object_index);
    int GetKnownDriveObjectIndex(const std::string& path);
    void SetComputerViews();
    void SetFolderViews();
    void AddToolBarIcon(UINT icon_id, const CSize& icon_size);
    int GetColumnCount();
    void DeleteAllColumns();
    void AddColumn(UINT string_id, int width);
    int GetItemUnderMousePointer();
    void SetToolBarButtonText(int command_id, UINT resource_id);
    int GetDriveIndexByObjectIndex(int object_index);
    int SelectDriveByObjectIndex(int object_index);

    const PanelType panel_type_;
    Delegate* delegate_;

    CStatic title_;

    std::unique_ptr<proto::DirectoryList> directory_list_;
    CListViewCtrl list_;
    CImageListManaged list_imagelist_;

    CToolBarCtrl toolbar_;
    CImageListManaged toolbar_imagelist_;

    std::unique_ptr<proto::DriveList> drive_list_;
    CComboBoxEx drive_combo_;
    CImageListManaged drive_imagelist_;

    CStatic status_;

    DISALLOW_COPY_AND_ASSIGN(UiFileManagerPanel);
};

} // namespace aspia

#endif // _ASPIA_UI__FILE_MANAGER_PANEL_H
