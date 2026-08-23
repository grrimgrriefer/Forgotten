// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationSubsystem.h"
#include "Forgotten/Character/ConversableNPC.h"

void UConversationSubsystem::SetCurrentConversableNpc(AConversableNPC* npc)
{
	if (m_activeNpc.Get() != npc)
	{
		m_activeNpc = npc;
		m_OnCurrentConversableNpcChanged.Broadcast(m_activeNpc.Get());
	}
}
AConversableNPC* UConversationSubsystem::GetCurrentConversableNpc() const
{
	return m_activeNpc.Get();
}
bool UConversationSubsystem::HasCurrentConversableNpc() const
{
	return m_activeNpc.IsValid();
}
void UConversationSubsystem::SubmitPlayerMessage(const FText& messageText)
{
	if (messageText.IsEmptyOrWhitespace())
	{
		return;
	}

	// TODO: Get player name from Voxta
	const FText playerSpeaker = NSLOCTEXT("Conversation", "YouSpeaker", "You");
	m_OnTranscriptEntryAdded.Broadcast(playerSpeaker, messageText);

	// TODO: Forward input to Voxta
}
