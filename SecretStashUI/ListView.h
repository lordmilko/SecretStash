#pragma once

#include "../Shared/DataStructures.h"
#include "../Shared/Vector.h"
#include "../Shared/_Test.h"

namespace SecretStash
{
	typedef CWinTraitsOR<LVS_REPORT> ListViewTraits;

	class ListView : public CWindowImpl < ListView, CListViewCtrl, ListViewTraits >
	{
	public:
		DECLARE_WND_SUPERCLASS(NULL, WC_LISTVIEW)

		BEGIN_MSG_MAP(ListView)

		END_MSG_MAP()

		//https://msdn.microsoft.com/en-us/magazine/cc163384.aspx - see the list view styles...we'd like to be able to check all the items

		RECT CreateControlRect(int xPos, int yPos, int height, int width)
		{
			RECT rect;

			rect.left = xPos;
			rect.top = yPos;

			rect.right = xPos + width;
			rect.bottom = yPos + height;

			return rect;
		}

		void Init(HWND hwnd)
		{
			//RECT rect = CreateControlRect(100, 200, 300, 400);

			Create(hwnd, rcDefault, L"ListView", LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_CHILD | WS_VISIBLE);
			SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

			//Create Columns

			InsertColumn(0, CString(LPCTSTR(IDS_COLHDR_NUMBER)), LVCFMT_LEFT, 0);
			InsertColumn(1, CString(LPCTSTR(IDS_COLHDR_PATH)), LVCFMT_LEFT, 0);
            InsertColumn(2, CString(LPCTSTR(IDS_COLHDR_VISIBLE)), LVCFMT_LEFT, 0);

			//Set the width of the columns. Autosizing needs to be done after _all_ the columns have been created

			SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
            SetColumnWidth(1, 600);
            SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

			Insert();
		}

		void Insert()
		{
			//need to check if an item already exists before adding

			InsertRow(L"Blah");



			FolderVector fv = { 0, 0, 0, 0 };
            FolderVectorInitialize(&fv, 10);
            AddNewFolder(&fv);

            
			for (size_t i = 0; i < fv.size; i++)
			{

				//1: copy our path to a new unicode string
				//2: set the maxlength to fit both strings - should we do this first? how does rtlcopy work
				//3: append the filename to the new string


				UNICODE_STRING destination;

				Folder* folder = FolderVectorGet(&fv, i);

				for (size_t j = 0; j < folder->FileNames.size; j++)
				{
					UNICODE_STRING* fileName = UnicodeStringVectorGet(&folder->FileNames, j);

					destination.Length = 0;
					destination.MaximumLength = (folder->PartitionDirPath.Length + fileName->Length + sizeof(WCHAR));
					destination.Buffer = (PWSTR)malloc(destination.MaximumLength);

					RtlUnicodeStringCopy(&destination, &folder->PartitionDirPath);

					RtlAppendUnicodeStringToString(&destination, fileName);

					InsertRow(destination.Buffer);
                    SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
				}


				//RtlAppendUnicodeStringToString()
			}
		}

		void InsertRow(LPCTSTR path)
		{
			const int bufferSize = 256;
			wchar_t buffer[bufferSize];
			swprintf_s(buffer, bufferSize, L"%d", GetItemCount() + 1);

			int itemIndex = InsertItem(GetItemCount(), buffer); //Insert the item number

			SetItemText(itemIndex, 1, path); //Insert the path name
            SetItemText(itemIndex, 2, L"True");
		}
	};
}