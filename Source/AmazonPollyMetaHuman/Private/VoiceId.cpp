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

#include "VoiceId.h"

DEFINE_LOG_CATEGORY(LogAmazonPollyVoiceId);

/*
* Returns a VoiceId enum to its corresponding VoiceId object with the same name
* @param VoiceId - enum to be mapped to the VoiceId object 
* @return Aws::Polly::Model::VoiceId - VoiceId object to be used in configuring PollyRequest 
*/
Aws::Polly::Model::VoiceId ToPollyVoiceId(const EVoiceId VoiceId)
{
    switch (VoiceId)
    {
    case EVoiceId::Nicole:
        return Aws::Polly::Model::VoiceId::Nicole;
    case EVoiceId::Russell:
        return Aws::Polly::Model::VoiceId::Russell;
    case EVoiceId::Amy:
    case EVoiceId::AmyNeural:
        return Aws::Polly::Model::VoiceId::Amy;
    case EVoiceId::Emma:
    case EVoiceId::EmmaNeural:
        return Aws::Polly::Model::VoiceId::Emma;
    case EVoiceId::Brian:
    case EVoiceId::BrianNeural:
        return Aws::Polly::Model::VoiceId::Brian;
    case EVoiceId::Aditi:
        return Aws::Polly::Model::VoiceId::Aditi;
    case EVoiceId::Raveena:
        return Aws::Polly::Model::VoiceId::Raveena;
    case EVoiceId::Ivy:
    case EVoiceId::IvyNeural:
        return Aws::Polly::Model::VoiceId::Ivy;
    case EVoiceId::Joanna:
    case EVoiceId::JoannaNeural:
        return Aws::Polly::Model::VoiceId::Joanna;
    case EVoiceId::Kendra:
    case EVoiceId::KendraNeural:
        return Aws::Polly::Model::VoiceId::Kendra;
    case EVoiceId::Kimberly:
    case EVoiceId::KimberlyNeural:
        return Aws::Polly::Model::VoiceId::Kimberly;
    case EVoiceId::Salli:
    case EVoiceId::SalliNeural:
        return Aws::Polly::Model::VoiceId::Salli;
    case EVoiceId::Joey:
    case EVoiceId::JoeyNeural:
        return Aws::Polly::Model::VoiceId::Joey;
    case EVoiceId::Justin:
    case EVoiceId::JustinNeural:
        return Aws::Polly::Model::VoiceId::Justin;
    case EVoiceId::Matthew:
    case EVoiceId::MatthewNeural:
        return Aws::Polly::Model::VoiceId::Matthew;
    case EVoiceId::Geraint:
        return Aws::Polly::Model::VoiceId::Geraint;
    default:
        UE_LOG(LogAmazonPollyVoiceId, Error, TEXT("ToPollyVoiceId: Invalid VoiceId (EVoiceId Index: %d)."), VoiceId);
        return Aws::Polly::Model::VoiceId::NOT_SET;
    }
}

/*
* Returns neural or standard voice engine depending on the voiceId
* @param VoiceId - enum to be mapped to the voice engine
* @return Aws::Polly::Model::Engine - Engine object to be used in configuring PollyRequest 
*/
Aws::Polly::Model::Engine ToPollyVoiceEngine(const EVoiceId VoiceId)
{
    switch (VoiceId)
    {
    case EVoiceId::Nicole:
    case EVoiceId::Russell:
    case EVoiceId::Amy:
    case EVoiceId::Emma:
    case EVoiceId::Brian:
    case EVoiceId::Aditi:
    case EVoiceId::Raveena:
    case EVoiceId::Ivy:
    case EVoiceId::Joanna:
    case EVoiceId::Kendra:
    case EVoiceId::Kimberly:
    case EVoiceId::Salli:
    case EVoiceId::Joey:
    case EVoiceId::Justin:
    case EVoiceId::Matthew:
    case EVoiceId::Geraint:
        return Aws::Polly::Model::Engine::standard;
    case EVoiceId::AmyNeural:
    case EVoiceId::EmmaNeural:
    case EVoiceId::BrianNeural:
    case EVoiceId::IvyNeural:
    case EVoiceId::JoannaNeural:
    case EVoiceId::KendraNeural:
    case EVoiceId::KimberlyNeural:
    case EVoiceId::SalliNeural:
    case EVoiceId::JoeyNeural:
    case EVoiceId::JustinNeural:
    case EVoiceId::MatthewNeural:
        return Aws::Polly::Model::Engine::neural;
    default:
        UE_LOG(LogAmazonPollyVoiceId, Error, TEXT("ToPollyVoiceEngine: Invalid VoiceId (EVoiceId Index: %d)."), VoiceId);
        return Aws::Polly::Model::Engine::standard;
    }
}