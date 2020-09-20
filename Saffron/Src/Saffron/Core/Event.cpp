#include "SaffronPCH.h"
#include "Saffron/Core/Event.h"

namespace Se
{

std::string Event::ToString() const
{
	return GetName();
}

bool Event::InCategory(Category category) const
{
	return static_cast<int>(GetCategoryFlags()) & static_cast<unsigned int>(category);
}

}