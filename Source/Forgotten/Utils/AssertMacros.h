// Copyright(c) 2026 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "Misc/MessageDialog.h"
#include "Editor.h"
#endif

inline void HandleAssertFailed(
	const TCHAR* checkExpression,
	const TCHAR* fileName,
	int32 lineNumber,
	const FString& customMessage = FString())
{
	FString errorMessage = FString::Printf(
		TEXT("Assertion Failed: %s\n\nFile: %s\nLine: %d"),
		checkExpression,
		fileName,
		lineNumber
	);

	if (!customMessage.IsEmpty())
	{
		errorMessage += FString::Printf(TEXT("\nMessage: %s"), *customMessage);
	}

	UE_LOG(LogTemp, Error, TEXT("%s"), *errorMessage);

#if WITH_EDITOR
	if (GIsEditor && GEditor)
	{
		GEditor->SetPIEWorldsPaused(true);

		FMessageDialog::Open(
			EAppMsgType::Ok,
			FText::FromString(errorMessage),
			FText::FromString("Assert Failure Triggered")
		);
	}
#endif
}

#define ASSERT_CHECK(expr, ...) \
do { \
	if (UNLIKELY(!(expr))) { \
		HandleAssertFailed(TEXT(#expr), TEXT(__FILE__), __LINE__, ##__VA_ARGS__); \
		return; \
	} \
} while(0)

#define ASSERT_CHECK_RETURN(expr, retVal, ...) \
do { \
	if (UNLIKELY(!(expr))) { \
		HandleAssertFailed(TEXT(#expr), TEXT(__FILE__), __LINE__, ##__VA_ARGS__); \
		return (retVal); \
	} \
} while(0)
