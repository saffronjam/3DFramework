#include "SaffronPCH.h"
#include "Saffron/Common/Event.h"

namespace Se
{
String Event::ToString() const
{
	return GetName();
}

bool Event::InCategory(Category category) const
{
	return static_cast<int>(GetCategoryFlags()) & static_cast<unsigned int>(category);
}
}
