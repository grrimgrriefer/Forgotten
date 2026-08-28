// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#include "ConversationSubsystem.h"
#include "Forgotten/Character/ConversableNPC.h"
#include "Forgotten/Character/FirstPersonCharacter.h"

void UConversationSubsystem::RegisterConversableNPC(AConversableNPC* npc)
{
	if (IsValid(npc))
	{
		m_registeredNpcs.AddUnique(npc);
	}
}
void UConversationSubsystem::UnregisterConversableNPC(AConversableNPC* npc)
{
	if (IsValid(npc))
	{
		m_registeredNpcs.Remove(npc);
	}
}
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
bool UConversationSubsystem::IsPlayerInRangeForChat(const AFirstPersonCharacter* player) const
{
	if (!IsValid(player))
	{
		return false;
	}

	if (m_activeNpc.IsValid())
	{
		return m_activeNpc->IsInRangeForChat(player);
	}

	for (const TWeakObjectPtr<AConversableNPC>& weakNpc : m_registeredNpcs)
	{
		if (const AConversableNPC* npc = weakNpc.Get())
		{
			if (npc->IsInRangeForChat(player))
			{
				return true;
			}
		}
	}

	return false;
}
AConversableNPC* UConversationSubsystem::GetNearestConversableNPCInRange(const ACharacter* player) const
{
	if (!IsValid(player))
	{
		return nullptr;
	}

	if (m_activeNpc.IsValid() && m_activeNpc->IsInRangeForChat(player))
	{
		return m_activeNpc.Get();
	}

	AConversableNPC* nearestNpc = nullptr;
	float minDistanceSq = FLT_MAX;
	const FVector playerLoc = player->GetActorLocation();

	for (const TWeakObjectPtr<AConversableNPC>& weakNpc : m_registeredNpcs)
	{
		if (AConversableNPC* npc = weakNpc.Get())
		{
			if (npc->IsInRangeForChat(player))
			{
				const float distSq = FVector::DistSquared(playerLoc, npc->GetActorLocation());
				if (distSq < minDistanceSq)
				{
					minDistanceSq = distSq;
					nearestNpc = npc;
				}
			}
		}
	}

	return nearestNpc;
}
void UConversationSubsystem::SubmitMessageFromPlayer(const FText& messageText) const
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
