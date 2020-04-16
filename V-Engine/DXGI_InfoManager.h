#pragma once

#include <vector>
#include <string>
#include <dxgidebug.h>

namespace ve
{
class DXGI_InfoManager
{
public:
	DXGI_InfoManager();
	~DXGI_InfoManager();
	DXGI_InfoManager( const DXGI_InfoManager & ) = delete;
	DXGI_InfoManager &operator=( const DXGI_InfoManager & ) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long m_next = 0u;
	IDXGIInfoQueue *m_pDXGI_InfoQueue = nullptr;
};
}
