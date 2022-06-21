#pragma once
#include <deque>
#include <string>


/*
	A notification system for games
	Based on ImGui

	Displays a black box on the top left side of screen
	with messages
*/
class NotifiyPopup
{
private:
	struct Notification
	{
		std::string message;
		double timer;
	};

	static inline const float m_fOffset = 10.0f;
	static inline const float m_fDuration = 1.5f;
	static inline const size_t m_nMaxDisplayCount = 3;
	static inline std::deque<Notification> m_Messages;

	static bool IsMessagesInQueue();
	static size_t GetQueueSize();

public:
	NotifiyPopup() = delete;
	NotifiyPopup(NotifiyPopup&) = delete;
	void operator=(NotifiyPopup const&) = delete;

	static void Draw();
	static void AddToQueue(std::string&& message);
};
