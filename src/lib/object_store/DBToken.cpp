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
#include "OSPathSep.h"

#include "cryptoki.h"
#include "DBToken.h"
#include "DBObject.h"
#include "TokenDB.h"

#include <vector>
#include <string>
#include <set>
#include <map>
#include <list>
#include <cstdio>
#include <sys/stat.h>

const char * const DBTOKEN_FILE = "sqlite3.db";
const CK_ULONG DBTOKEN_OBJECT_TOKENINFO = 1;

// Constructor for creating a new token.
DBToken::DBToken(const std::string &baseDir, const std::string &tokenName, const ByteString &label, const ByteString &serial)
	: _connection(NULL)
{
	std::string tokenDir = baseDir + OS_PATHSEP + tokenName;
	std::string tokenPath = tokenDir + OS_PATHSEP + DBTOKEN_FILE;

	// Refuse to open an already existing database.
	FILE *f = fopen(tokenPath.c_str(),"r");
	if (f)
	{
		fclose(f);
		ERROR_MSG("Refusing to overwrite and existing database at \"%s\"", tokenPath.c_str());
		return;
	}

	// First create the directory for the token, we expect basePath to already exist
	if (mkdir(tokenDir.c_str(), S_IFDIR | S_IRWXU))
	{
		ERROR_MSG("Unable to create directory \"%s\"", tokenDir.c_str());
		return;
	}

	// Create
	_connection = TokenDB::Connection::Create(tokenDir, DBTOKEN_FILE);
	if (_connection == NULL)
	{
		ERROR_MSG("Failed to create a database connection for \"%s\"", tokenPath.c_str());
		return;
	}

	if (!_connection->connect())
	{
		delete _connection;
		_connection = NULL;

		ERROR_MSG("Failed to connect to the database at \"%s\"", tokenPath.c_str());

		// Now remove the token directory
		if (remove(tokenDir.c_str()))
		{
			ERROR_MSG("Failed to remove the token directory \"%s\"", tokenDir.c_str());
		}

		return;
	}

	// Create a DBObject for the established connection to the database.
	DBObject tokenObject(_connection);

	// First create the tables that support storage of object attributes and then insert the object containing
	// the token info into the database.
	if (!tokenObject.createTables() || !tokenObject.insert() || tokenObject.objectId()!=DBTOKEN_OBJECT_TOKENINFO)
	{
		tokenObject.dropConnection();

		_connection->close();
		delete _connection;
		_connection = NULL;

		ERROR_MSG("Failed to create tables for storing objects in database at \"%s\"", tokenPath.c_str());
		return;
	}

	// Set the initial attributes
	CK_ULONG flags =
		CKF_RNG |
		CKF_LOGIN_REQUIRED | // FIXME: check
		CKF_RESTORE_KEY_NOT_NEEDED |
		CKF_TOKEN_INITIALIZED |
		CKF_SO_PIN_LOCKED |
		CKF_SO_PIN_TO_BE_CHANGED;

	OSAttribute tokenLabel(label);
	OSAttribute tokenSerial(serial);
	OSAttribute tokenFlags(flags);

	if (!tokenObject.setAttribute(CKA_OS_TOKENLABEL, tokenLabel) ||
		!tokenObject.setAttribute(CKA_OS_TOKENSERIAL, tokenSerial) ||
		!tokenObject.setAttribute(CKA_OS_TOKENFLAGS, tokenFlags))
	{
		_connection->close();
		delete _connection;
		_connection = NULL;

		// Now remove the token file
		if (remove(tokenPath.c_str()))
		{
			ERROR_MSG("Failed to remove the token file at \"%s\"", tokenPath.c_str());
		}

		// Now remove the token directory
		if (remove(tokenDir.c_str()))
		{
			ERROR_MSG("Failed to remove the token directory at \"%s\"", tokenDir.c_str());
		}
		return;
	}

	// Success!
}

// Constructor for accessing an existing token.
DBToken::DBToken(const std::string &baseDir, const std::string &tokenName)
{
}

// Destructor
DBToken::~DBToken()
{
	delete _connection;
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

bool DBToken::deleteObject(OSObject *object)
{
	return false;
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
