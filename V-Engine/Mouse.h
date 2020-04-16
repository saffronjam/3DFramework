#pragma once

#include <queue>
#include <optional>

namespace ve
{
class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type m_type;
		std::pair<bool, bool> m_state;
		std::pair<int, int> m_coords;
	public:
		Event() noexcept
			:
			m_type(Type::Invalid),
			m_state(std::make_pair(false, false)),
			m_coords(std::make_pair(0, 0))
		{

		}
		Event(Type type, const Mouse &parent) noexcept
			:
			m_type(type),
			m_state(parent.m_state),
			m_coords(parent.m_coords)
		{
		}
		bool IsValid() const noexcept { return m_type != Type::Invalid; }
		Type GetType() const noexcept { return m_type; }
		auto GetPos() const noexcept { return m_coords; }
		int GetPosX() const noexcept { return m_coords.first; }
		int GetPosY() const noexcept { return m_coords.second; }
	};
public:
	Mouse() = default;
	Mouse(const Mouse &) = delete;
	Mouse &operator=(const Mouse &) = delete;

	void Update();

	std::pair<int, int> GetPos() const noexcept { return m_coords; }
	int GetPosX() const noexcept { return m_coords.first; }
	int GetPosY() const noexcept { return m_coords.second; }
	bool LeftIsDown() const noexcept;
	bool RightIsDown() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool LeftIsReleased() const noexcept;
	bool RightIsReleased() const noexcept;
	std::optional<Event> Read() noexcept;
	bool IsEmpty() const noexcept;
	void Clear() noexcept;
	bool IsInWindow() const noexcept { return m_inWindow; }
private:
	void OnMove(int x, int y) noexcept;
	void OnLeave(int x, int y) noexcept;
	void OnEnter(int x, int y) noexcept;
	void OnLeftPress(int x, int y) noexcept;
	void OnLeftRelease(int x, int y) noexcept;
	void OnRightPress(int x, int y) noexcept;
	void OnRightRelease(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
	void TrimBuffer() noexcept;
private:
	static constexpr uint32_t m_bufferSize = 16u;
	std::pair<int, int> m_coords;
	std::pair<bool, bool> m_state;
	std::pair<bool, bool> m_prevState;
	int m_wheelDeltaCarry = 0;
	std::queue<Event> m_buffer;
	bool m_inWindow = false;
};
}
