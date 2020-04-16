#include "DXGI_InfoManager.h"

#include <memory>

#include "Window.h"
#include "Graphics.h"

ve::DXGI_InfoManager::DXGI_InfoManager()
{
	typedef HRESULT( WINAPI *DXGIGetDebugInterface )( REFIID, void ** );

	const auto hModDXGIDebug = LoadLibraryEx( "dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32 );
	if ( hModDXGIDebug == nullptr )
		throw VEWND_LAST_EXCEPT();

	const auto DXGIGetDebugInterface_extract = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void *>( GetProcAddress( hModDXGIDebug, "DXGIGetDebugInterface" ) )
		);
	if ( DXGIGetDebugInterface_extract == nullptr )
		throw VEWND_LAST_EXCEPT();

	HRESULT hr = {};
	GFX_THROW_NOINFO(
		DXGIGetDebugInterface_extract(
			__uuidof( IDXGIInfoQueue ),
			&m_pDXGI_InfoQueue
		)
	);
}

void ve::DXGI_InfoManager::Set() noexcept
{
	m_next = m_pDXGI_InfoQueue->GetNumStoredMessages( DXGI_DEBUG_ALL );
}

std::vector<std::string> ve::DXGI_InfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = m_pDXGI_InfoQueue->GetNumStoredMessages( DXGI_DEBUG_ALL );
	for ( auto i = m_next; i < end; i++ )
	{
		HRESULT hr = {};
		SIZE_T messageLength;
		GFX_THROW_NOINFO(
			m_pDXGI_InfoQueue->GetMessage( DXGI_DEBUG_ALL,
										   i,
										   nullptr,
										   &messageLength )
		);

		auto bytes = std::make_unique<byte[]>( messageLength );
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE *>( bytes.get() );

		GFX_THROW_NOINFO(
			m_pDXGI_InfoQueue->GetMessage( DXGI_DEBUG_ALL,
										   i,
										   pMessage,
										   &messageLength )
		);
		messages.emplace_back( pMessage->pDescription );
	}
	return messages;
}
