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

#include "TestableSpeechComponent.h"

void UTestableSpeechComponent::InitializePollyClient() {
    MyPollyClient = MakeUnique<MockPollyClient>();
}

void UTestableSpeechComponent::GenerateSpeechSync(const FString text, const EVoiceId VoiceId) {
    Super::GenerateSpeechSync(text, VoiceId);
}

void UTestableSpeechComponent::SetTimer(float CurrentVisemeDurationSeconds) {
    // We override SetTimer() to do nothing, since we don't create any actual timers
    // during unit testing (creation of timers in tests causes crashes, as tests only occur in a single frame) 
    // PlayNextViseme() will be called manually during unit testing to mock functionality of timer, 
    // which recursively calls on PlayNextViseme() on every call to SetTimer 
}

void UTestableSpeechComponent::ClearTimer() {
    // We override ClearTimer() to do nothing, since we don't create any actual timers
    // during unit testing (creation of timers in tests causes crashes, as tests only occur in a single frame) 
}

MockPollyClient* UTestableSpeechComponent::GetPollyClient() {
    // Normally we'd use dynamic casting here but RTTI is disabled by default by Unreal, 
    // which is needed to invoke dynamic_cast on Polymorphic objects 
    return StaticCast<MockPollyClient*>(MyPollyClient.Get());
}

void UTestableSpeechComponent::PlayNextViseme() {
    Super::PlayNextViseme();
}

EViseme UTestableSpeechComponent::GetCurrentViseme() {
    return CurrentViseme;
}

TArray<VisemeEvent> UTestableSpeechComponent::GetVisemeEventArray() {
    return VisemeEventArray;
}

int UTestableSpeechComponent::GetCurrentVisemeIndex() {
    return CurrentVisemeIndex;
}

TArray<uint8> UTestableSpeechComponent::GetAudiobuffer() {
    return Audiobuffer;
}

bool UTestableSpeechComponent::IsSpeaking() {
    return bIsSpeaking;
}

void UTestableSpeechComponent::SetSpeaking(bool boolean) {
    bIsSpeaking = boolean;
}
