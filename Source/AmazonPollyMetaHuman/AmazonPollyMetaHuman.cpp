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

#include "AmazonPollyMetaHuman.h"
#include "Core.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FAmazonPollyMetaHumanModule"
DEFINE_LOG_CATEGORY(LogAmazonPollyMetaHuman);

void* MemoryManagerWrapper::AllocateMemory(std::size_t blockSize, std::size_t /*alignment*/, const char* /*allocationTag*/)
{
    return ::operator new(blockSize);
}

void MemoryManagerWrapper::FreeMemory(void* memoryPtr)
{
    return ::operator delete(memoryPtr);
}

FAmazonPollyMetaHumanModule::FAmazonPollyMetaHumanModule()
: 
    m_sdkOptions(new Aws::SDKOptions)
{

}

FAmazonPollyMetaHumanModule::~FAmazonPollyMetaHumanModule()
{
    delete static_cast<Aws::SDKOptions *>(m_sdkOptions);
    m_sdkOptions = nullptr;
}

void FAmazonPollyMetaHumanModule::StartupModule()
{
    Aws::SDKOptions* awsSDKOptions = static_cast<Aws::SDKOptions*>(m_sdkOptions);
    awsSDKOptions->memoryManagementOptions.memoryManager = &m_memoryManager;
    Aws::InitAPI(*awsSDKOptions);
    m_apiInitialized = true;
}

void FAmazonPollyMetaHumanModule::ShutdownModule()
{
    if (!m_apiInitialized) {
        return;
    }
    m_apiInitialized = false;
    Aws::ShutdownAPI(*static_cast<Aws::SDKOptions *>(m_sdkOptions));
}


#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_PRIMARY_GAME_MODULE( FAmazonPollyMetaHumanModule, AmazonPollyMetaHuman, "AmazonPollyMetaHuman" );