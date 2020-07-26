#include "Saffron/Event/Event.h"

namespace Saffron
{

Event::Type Event::GetType() const
{
	return Type::None;
}

const char *Event::GetName() const
{
	return "Unnamed";
}

std::string Event::ToString() const
{
	return GetName();
}

bool Event::InCategory(Category category) const
{
	return static_cast<int>(GetCategoryFlags()) & static_cast<unsigned int>(category);
}

}