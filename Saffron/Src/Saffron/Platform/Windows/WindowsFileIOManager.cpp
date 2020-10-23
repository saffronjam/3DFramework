#include "SaffronPCH.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <commdlg.h>
#include <Windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Saffron/Core/FileIOManager.h"

namespace Se
{

const char *FormatFilter(const FileIOManager::Filter &filter)
{
	static char buffer[1000] = { 0 };
	char *bufferP = buffer;

	sprintf_s(bufferP, 1000 - (bufferP - buffer), "%s", filter.Description.c_str());
	bufferP += filter.Description.size() + 1;

	for ( const auto &extension : filter.Extensions )
	{
		sprintf_s(bufferP, 1000 - (bufferP - buffer), "%s", extension.c_str());
		bufferP += extension.size() + 1;
	}
	sprintf_s(bufferP, 1000 - (bufferP - buffer), "\0");

	return buffer;
}

fs::path FileIOManager::OpenFile(const Filter &filter)
{
	SE_CORE_ASSERT(m_Window, "FileIOManager was not initialized");

	const char *formattedFilter = FormatFilter(filter);

	OPENFILENAMEA ofn;       // common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(m_Window->GetNativeWindow()));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = formattedFilter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if ( GetOpenFileNameA(&ofn) == TRUE )
	{
		return ofn.lpstrFile;
	}
	return fs::path();
}

fs::path FileIOManager::SaveFile(const Filter &filter)
{
	SE_CORE_ASSERT(m_Window, "FileIOManager was not initialized");

	const char *formattedFilter = FormatFilter(filter);
	const char *fallbackExtension = filter.Extensions.front() == "*.*" ? "" : filter.Extensions.front().c_str();

	OPENFILENAMEA ofn;       // common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(m_Window->GetNativeWindow()));
	ofn.lpstrFile = szFile;
	ofn.lpstrDefExt = fallbackExtension;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = formattedFilter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT;

	if ( GetSaveFileNameA(&ofn) == TRUE )
	{
		return ofn.lpstrFile;
	}
	return fs::path();
}


}