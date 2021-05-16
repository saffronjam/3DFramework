#pragma once

#include "Saffron/Core/Managed.h"

namespace Se
{
class Resource : public Managed
{
public:
	virtual ~Resource() = default;

	virtual ulong GetResourceID() const { return 0; }
	bool IsLoaded() const;

	void MarkAsFallback();
	void UnmarkAsFallback();
	bool IsFallback() const;

protected:
	void SuccessfulLoad();
	static ulong HashFilepath(const Path& filepath);
	static ulong HashString(const String& string);

private:
	bool _loaded = false;
	bool _isFallback = false;
};
}
