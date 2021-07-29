#include "SaffronPCH.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/ErrorHandling/DxgiInfoManager.h"


namespace Se
{
DxgiInfoManager::DxgiInfoManager() :
	SingleTon(this)
{
	using DXGIGetDebugInterface = HRESULT(WINAPI*)(REFIID, void**);

	constexpr const auto* dll = "dxgidebug.dll";

	const auto handle = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (handle == nullptr)
	{
		Debug::Break("Failed to load {0}", dll);
		return;
	}

	const auto DxgiGetDebugInterface = static_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(
		handle,
		"DXGIGetDebugInterface"
	)));

	ThrowIfBad(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &_queue));
}

void DxgiInfoManager::Begin()
{
	_startIndex = _queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

auto DxgiInfoManager::End() -> std::vector<std::string>
{
	const auto messages = _queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	if (messages == 0)
	{
		return {};
	}

	std::vector<std::string> result;
	result.reserve(messages);

	for (int i = _startIndex; i < messages; i++)
	{
		size_t messageLength = 0;
		if (BadHResult(_queue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength)))
		{
			Debug::Break("Failed to get message size in DxgiInfoManager");
			continue;
		}

		const auto messageBuffer = std::make_unique<uchar[]>(messageLength);
		const auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(messageBuffer.get());

		if (BadHResult(_queue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength)))
		{
			Debug::Break("Failed to get message in DxgiInfoManager");
			continue;
		}
		result.emplace_back(message->pDescription);
	}

	return result;
}
}
