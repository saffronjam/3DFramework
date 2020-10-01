#include "SaffronPCH.h"

#include "Saffron/Input/Input.h"
#include "Saffron/Renderer/EditorViewport.h"

namespace Se
{

glm::vec2 EditorViewport::TopLeft{ 0.0f, 0.0f };
glm::vec2 EditorViewport::BottomRight{ 0.0f, 0.0f };
bool EditorViewport::Focused = false;
bool EditorViewport::Hovered = false;

bool EditorViewport::InViewport(glm::vec2 positionNDC)
{
	positionNDC.x -= TopLeft.x;
	positionNDC.y -= TopLeft.y;
	const auto viewportWidth = BottomRight.x - TopLeft.x;
	const auto viewportHeight = BottomRight.y - TopLeft.y;

	return positionNDC.x < BottomRight.x &&positionNDC.y < BottomRight.y;
}

glm::vec2 EditorViewport::GetMousePosition()
{
	glm::vec2 position = Input::GetMousePositionNDC();
	position.x -= TopLeft.x;
	position.y -= TopLeft.y;
	const auto viewportWidth = BottomRight.x - TopLeft.x;
	const auto viewportHeight = BottomRight.y - TopLeft.y;

	return { (position.x / viewportWidth) * 2.0f - 1.0f, ((position.y / viewportHeight) * 2.0f - 1.0f) * -1.0f };
}
}
