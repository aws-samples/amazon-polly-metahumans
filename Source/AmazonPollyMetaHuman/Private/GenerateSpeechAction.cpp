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

#include "GenerateSpeechAction.h"

FGenerateSpeechAction::FGenerateSpeechAction(
    const struct FLatentActionInfo& LatentActionInfo,
    USpeechComponent* SpeechComponent,
    const FString& Text,
    const EVoiceId& VoiceId,
    EGenerateSpeechExecPins& GenerateSpeechExecPins
) :
    ExecutionFunction(LatentActionInfo.ExecutionFunction),
    Linkage(LatentActionInfo.Linkage),
    CallbackTarget(LatentActionInfo.CallbackTarget),
    SpeechComponent(SpeechComponent),
    GenerateSpeechExecPins(GenerateSpeechExecPins),
    bIsDone(false)
{
    this->GenerateSpeechExecPins = EGenerateSpeechExecPins::Failure;
    AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [this, Text, VoiceId] ()
    {
        this->SpeechComponent->GenerateSpeechSync(Text, VoiceId);
        AsyncTask(ENamedThreads::GameThread, [this] ()
        {
            this->GenerateSpeechExecPins = EGenerateSpeechExecPins::Success;
            this->bIsDone = true;
        });
    });
}

void FGenerateSpeechAction::UpdateOperation(FLatentResponse& Response)
{
    Response.FinishAndTriggerIf(bIsDone, ExecutionFunction, Linkage, CallbackTarget);
}