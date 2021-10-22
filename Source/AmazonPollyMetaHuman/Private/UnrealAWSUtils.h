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
#include "CoreMinimal.h"
#include <aws/core/Aws.h>

namespace UnrealAWSUtils {
    /**
    * Converts FString to AwsString.
    * NOTE: When setting text for a PollyRequest, this method should not be invoked inline.
    * This causes crashes for longer text inputs. Create a Aws::String object and pass it 
    * into the PollyRequest. 
    * @param UnrealString - FString to be converted
    * @return The converted AwsString
    */
    Aws::String FStringToAwsString(const FString& UnrealString);
    /**
    * Converts AwsString to FString.
    * @param AwsString - AwsString to be converted
    * @return The converted FString
    */
    FString AwsStringToFString(const Aws::String& AwsString);
    /**
    * Reads a Polly stream to fill a buffer. 
    * @param PollyStream - the stream returned by the call to Polly
    * @return - the filled buffer 
    */
    TArray<uint8> PreparePollyData(Aws::IOStream& PollyStream);
}
