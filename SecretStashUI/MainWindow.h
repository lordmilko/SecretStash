#pragma once

#include "resource.h"
#include "ListView.h"
#include "Ribbon.h"
#include <atldlgs.h>

#include "CommunicationPortUser.h"

namespace SecretStash
{
	class MainWindow : public CRibbonFrameWindowImpl < MainWindow >
	{
	public:
		DECLARE_FRAME_WND_CLASS_EX(NULL, IDR_MAINFRAME, 0, 4) //----------------------we dont want to use 4, we want to use some kind of constant

		BEGIN_MSG_MAP(MainWindow)
			MESSAGE_HANDLER(WM_CREATE, OnCreate)
			COMMAND_ID_HANDLER(cmdButtonNewFile, OnNewFile)
			COMMAND_ID_HANDLER(cmdButtonNewFolder, OnNewFolder)
			COMMAND_ID_HANDLER(cmdButtonExit, OnFileExit)
			CHAIN_MSG_MAP(CRibbonFrameWindowImpl<MainWindow>)
		END_MSG_MAP()

		LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&)
		{
			RECT rect1;
			GetClientRect(&rect1);
			listView.Init(*this);

			m_hWndClient = listView;

			ShowRibbonUI(true);

            testcommunication();

			return 0;
		}

		LRESULT OnNewFile(WORD, WORD, HWND, BOOL&)
		{
			CString selectedFile;
			CShellFileOpenDialog dialog; //we need to allow multi select. but how to get all the values in our cstring?
			//also need to prevent showing of previous versions
			//CFileDialog blah(TRUE, NULL, NULL, NULL,  L"All Files (*.*)");
			//CFileDialog blah(TRUE);

			//we will want to have a multi select folder and file dialog
			dialog.DoModal();
			dialog.GetFilePath(selectedFile);

			return 0;
		}

		LRESULT OnNewFolder(WORD, WORD, HWND, BOOL&)
		{
			CShellFileOpenDialog dialog(NULL, FOS_PICKFOLDERS); //we need to allow multi select. but how to get all the values in our cstring?
			dialog.DoModal();
			return 0;
		}

		LRESULT OnFileExit(WORD, WORD, HWND, BOOL&)
		{
			PostMessage(WM_CLOSE);
			return 0;
		}
	protected:
		ListView listView;
	};

}