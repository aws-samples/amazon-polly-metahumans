/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "Runtime/Engine/Public/LatentActions.h"
#include "SpeechComponent.h"

/**
 * Latent action corresponding to the latent USpeechComponent::GenerateSpeech function, which
 * is required to avoid blocking the game thread.
 */
class FGenerateSpeechAction : public FPendingLatentAction {
public:
    /**
     * @brief Construct a new FGenerateSpeechAction object
     * 
     * @param LatentActionInfo - Contains information required to update latent response and inform completion
     * @param SpeechComponent - SpeechComponent instance generating speech
     * @param Text - Text to generate speech for
     * @param VoiceId - Id of the voice use for generated audio
     * @param GenerateSpeechExecPins - Execution pin reference to be updated on completion
     */
    FGenerateSpeechAction(
        const struct FLatentActionInfo& LatentActionInfo,
        USpeechComponent* SpeechComponent,
        const FString& Text,
        const EVoiceId& VoiceId,
        EGenerateSpeechExecPins& GenerateSpeechExecPins
    );
    void UpdateOperation(FLatentResponse& Response) override;

private:
    /** Information required to update latent response and inform completion */
    const FName ExecutionFunction;

    /** Information required to update latent response and inform completion */
    const int32 Linkage;

    /** Information required to update latent response and inform completion */
    UObject* const CallbackTarget;

    /** SpeechComponent instance generating speech */
    USpeechComponent* SpeechComponent;

    /** Execution pin reference to be updated on completion */
    EGenerateSpeechExecPins& GenerateSpeechExecPins;

    /** Flag to signal completion */
    bool bIsDone;
};