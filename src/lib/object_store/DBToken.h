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
 DBToken.h

 The token class; a token is stored in a directory containing several files.
 Each object is stored in a separate file and a token object is present that
 has the token specific attributes
 *****************************************************************************/

#ifndef _SOFTHSM_V2_DBTOKEN_H
#define _SOFTHSM_V2_DBTOKEN_H

#include "config.h"
#include "OSAttribute.h"
#include "OSObject.h"
#include "OSToken.h"
#include "Directory.h"
#include "UUID.h"
#include "IPCSignal.h"
#include "MutexFactory.h"
#include "cryptoki.h"
#include <string>
#include <set>
#include <map>
#include <list>

class DBToken : public OSToken
{
private:
	// Constructor
	DBToken(const std::string basePath, const std::string tokenName);

	// Set the SO PIN
	bool setSOPIN(const ByteString& soPINBlob);

	// Get the SO PIN
	bool getSOPIN(ByteString& soPINBlob);

	// Set the user PIN
	bool setUserPIN(ByteString userPINBlob);

	// Get the user PIN
	bool getUserPIN(ByteString& userPINBlob);

	// Get the token flags
	bool getTokenFlags(CK_ULONG& flags);

	// Set the token flags
	bool setTokenFlags(const CK_ULONG flags);

	// Retrieve the token label
	bool getTokenLabel(ByteString& label);

	// Retrieve the token serial
	bool getTokenSerial(ByteString& serial);

	// Retrieve objects
	std::set<OSObject*> getObjects();

	// Insert objects into the given set
	void getObjects(std::set<OSObject*> &objects);

	// Create a new object
	OSObject* createObject();

	// Delete an object
	bool deleteObject(OSObject* object);

	// Destructor
	virtual ~DBToken();

	// Checks if the token is consistent
	bool isValid();

	// Invalidate the token (for instance if it is deleted)
	void invalidate();

	// Delete the token
	bool clearToken();

};

#endif // !_SOFTHSM_V2_DBTOKEN_H

