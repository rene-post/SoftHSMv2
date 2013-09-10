/*
 * Copyright (c) 2010 SURFnet bv
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************
 DBToken.cpp

 The token class; a token is stored in a directory containing several files.
 Each object is stored in a separate file and a token object is present that
 has the token specific attributes
 *****************************************************************************/

#include "config.h"
#include "log.h"
#include "OSAttributes.h"
#include "OSAttribute.h"
#include "ObjectFile.h"
#include "Directory.h"
#include "UUID.h"
#include "IPCSignal.h"
#include "cryptoki.h"
#include "DBToken.h"
#include "OSPathSep.h"
#include <vector>
#include <string>
#include <set>
#include <map>
#include <list>
#include <stdio.h>

#include <sqlite3.h>

// Constructor
DBToken::DBToken(const std::string basePath, const std::string tokenName)
{
}

// Destructor
DBToken::~DBToken()
{
}

// Set the SO PIN
bool DBToken::setSOPIN(const ByteString& soPINBlob)
{
	return false;
}

// Get the SO PIN
bool DBToken::getSOPIN(ByteString& soPINBlob)
{
	return false;
}

// Set the user PIN
bool DBToken::setUserPIN(ByteString userPINBlob)
{
	return false;
}

// Get the user PIN
bool DBToken::getUserPIN(ByteString& userPINBlob)
{
	return false;
}

// Retrieve the token label
bool DBToken::getTokenLabel(ByteString& label)
{
	return false;
}

// Retrieve the token serial
bool DBToken::getTokenSerial(ByteString& serial)
{
	return false;
}

// Get the token flags
bool DBToken::getTokenFlags(CK_ULONG& flags)
{
	return false;
}

// Set the token flags
bool DBToken::setTokenFlags(const CK_ULONG flags)
{
	return false;
}

// Retrieve objects
std::set<OSObject *> DBToken::getObjects()
{
	return std::set<OSObject *>();
}

void DBToken::getObjects(std::set<OSObject*> &objects)
{
}

// Create a new object
OSObject *DBToken::createObject()
{
	return NULL;
}

// Checks if the token is consistent
bool DBToken::isValid()
{
	return false;
}

// Invalidate the token (for instance if it is deleted)
void DBToken::invalidate()
{
}

// Delete the token
bool DBToken::clearToken()
{
	return false;
}
