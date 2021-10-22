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

#include "UnrealAWSUtils.h"

Aws::String UnrealAWSUtils::FStringToAwsString(const FString& UnrealString) {
    return Aws::String(TCHAR_TO_UTF8(UnrealString.GetCharArray().GetData()), UnrealString.Len());
}

FString UnrealAWSUtils::AwsStringToFString(const Aws::String& AwsString) {
    return FString(UTF8_TO_TCHAR(AwsString.c_str()));
}

/*
* Returns the size of a Polly stream for use in allocating size in buffers
* @param PollyStream - the Polly stream
* @return size - the size of the Polly stream
*/
long GetStreamSize(Aws::IOStream& PollyStream) {
    PollyStream.seekg(0, PollyStream.end);
    long size = PollyStream.tellg();
    PollyStream.seekg(0);
    return size;
}

TArray<uint8> UnrealAWSUtils::PreparePollyData(Aws::IOStream& PollyStream) {
    TArray<uint8> Buffer;
    long size = GetStreamSize(PollyStream);
    Buffer.AddUninitialized(size);
    PollyStream.read(reinterpret_cast<char*>(Buffer.GetData()), size);
    return Buffer;
}