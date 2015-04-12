#include "stdafx.h"
#include "MainWindow.h"
#include "UnicodeString.h"

namespace SecretStash
{
	int Run(int cmdShow);

	CAppModule ApplicationModule;

	int Main(HINSTANCE hInstance, LPTSTR cmdLine, int cmdShow)
	{
		UNREFERENCED_PARAMETER(cmdLine);

		AtlInitCommonControls(ICC_WIN95_CLASSES);

		HRESULT result = ApplicationModule.Init(NULL, hInstance);
		ATLASSERT(SUCCEEDED(result));

		int returnCode = Run(cmdShow);

		ApplicationModule.Term();

		return returnCode;

	}

	int Run(int cmdShow = SW_SHOWDEFAULT)
	{
		CMessageLoop msgLoop;
		ApplicationModule.AddMessageLoop(&msgLoop);

		MainWindow wndMain;

		RECT windowDimensions = { 0, 0, 900, 700 };

		if (wndMain.CreateEx(NULL, windowDimensions) == NULL)
			return 1;

		wndMain.CenterWindow();

		wndMain.ShowWindow(cmdShow);

		int returnCode = msgLoop.Run();
		ApplicationModule.RemoveMessageLoop();

		return returnCode;
	}

}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR cmdLine, int cmdShow)
{
	return SecretStash::Main(hInstance, cmdLine, cmdShow);
}