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

#include <aws/core/Aws.h>
#include <aws/polly/model/SynthesizeSpeechRequest.h>
#include "UnrealAWSUtils.h"
#include <aws/polly/PollyClient.h>

/**
* Struct containing Polly data, to be used in SynthesizeSpeech 
* NOTE: This struct was created to encapsulate all of the details 
* of the Polly SDK into this class. Due to being unable to create 
* custom SynthesizeSpeechOutcome objects, we use this struct to aid in 
* mocking for unit tests. 
*/
struct PollyOutcome {
    bool IsSuccess;
    TArray<uint8> StreamBuffer;
    Aws::String PollyErrorMsg; 
};

/**
* Wrapper for PollyClient that calls on Polly API, encapsulates AWS Polly SDK  
*/
class PollyClient {

private:
    /**
    * PollyClient used to invoke the Polly API (SynthesizeSpeech)
    */
    TUniquePtr<Aws::Polly::PollyClient> AwsPollyClient;

public:
    /*
    * Creates the PollyClient
    */
    PollyClient();

    virtual ~PollyClient();
    /**
    * Calls on Polly SDK and returns a PollyOutcome struct object with Polly data 
    * @param SpeechRequest - a configured SpeechRequest to be synthesized 
    * @return PollyOutcome - the struct containing the PollyData
    */
    virtual PollyOutcome SynthesizeSpeech(const Aws::Polly::Model::SynthesizeSpeechRequest& SpeechRequest);
};