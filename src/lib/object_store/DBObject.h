/*
 * Copyright (c) 2013 SURFnet bv
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
 DBObject.h

 This class represents object records in a database
 *****************************************************************************/

#ifndef _SOFTHSM_V2_DBOBJECT_H
#define _SOFTHSM_V2_DBOBJECT_H

#include "config.h"
#include "OSAttribute.h"
#include "cryptoki.h"
#include "OSObject.h"

#include <string>

namespace TokenDB { class Connection;  }

class DBObject : public OSObject
{
public:
	// Constructor for creating or accessing an object, don't do anything yet.
	DBObject(TokenDB::Connection *connection);

	// Destructor
	virtual ~DBObject();

	// Will drop any internal references to the connection
	void dropConnection();

	// Will return true when the object has not dropped its connection.
	bool hasConnection();

	// create tables to support storage of attributes for the DBObject
	bool createTables();

	// Find an existing object.
	bool find(long long objectId);

	// Insert a new object into the database and retrieve the object id associated with it.
	bool insert();

	// Object id associated with this object.
	long long objectId();

	// Check if the specified attribute exists
	virtual bool attributeExists(CK_ATTRIBUTE_TYPE type);

	// Retrieve the specified attribute
	virtual OSAttribute* getAttribute(CK_ATTRIBUTE_TYPE type);

	// Set the specified attribute
	virtual bool setAttribute(CK_ATTRIBUTE_TYPE type, const OSAttribute& attribute);

	// The validity state of the object
	virtual bool isValid();

	// Start an attribute set transaction; this method is used when - for
	// example - a key is generated and all its attributes need to be
	// persisted in one go.
	//
	// N.B.: Starting a transaction locks the object!
	//
	// Function returns false in case a transaction is already in progress
	virtual bool startTransaction(Access access);

	// Commit an attribute transaction; returns false if no transaction is in progress
	virtual bool commitTransaction();

	// Abort an attribute transaction; loads back the previous version of the object from disk;
	// returns false if no transaction was in progress
	virtual bool abortTransaction();

	// Destroys the object (warning, any pointers to the object are no longer
	// valid after this call because delete is called!)
	virtual bool destroyObject();

private:
	// Disable copy constructor and assignment
	DBObject();
	DBObject(const DBObject&);
	DBObject & operator= (const DBObject &);

	// Pimpl Idiom to hide implementation details
	class Private;
	struct Private *_private;

	long long _objectId;
};

#endif // !_SOFTHSM_V2_DBOBJECT_H

