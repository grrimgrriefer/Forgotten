// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ConversationSubsystem.generated.h"

class AConversableNPC;
class AFirstPersonCharacter;
class ACharacter;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTranscriptEntryAdded, const FText&, const FText&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveNpcChanged, AConversableNPC*);

/**
 * Handles the proximity-based conversation with the NPC(s)
 * Is only persistent within the loaded level.
 */
UCLASS()
class FORGOTTEN_API UConversationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FOnTranscriptEntryAdded m_OnTranscriptEntryAdded;
	FOnActiveNpcChanged m_OnCurrentConversableNpcChanged;

	void RegisterConversableNPC(AConversableNPC* npc);
	void UnregisterConversableNPC(AConversableNPC* npc);

	void SetCurrentConversableNpc(AConversableNPC* npc);
	AConversableNPC* GetCurrentConversableNpc() const;
	bool HasCurrentConversableNpc() const;

	bool IsPlayerInRangeForChat(const AFirstPersonCharacter* player) const;
	AConversableNPC* GetNearestConversableNPCInRange(const ACharacter* player) const;

	void SubmitMessageFromPlayer(const FText& messageText) const;

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<AConversableNPC> m_activeNpc = nullptr;
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AConversableNPC>> m_registeredNpcs;
};
