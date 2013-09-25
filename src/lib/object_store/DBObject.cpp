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

#include "config.h"
#include "DBObject.h"
#include "OSPathSep.h"
#include "TokenDB.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdio>
#include <map>

struct DBObject::Private
{
	TokenDB::Connection *connection;

	std::map<CK_ATTRIBUTE_TYPE,OSAttribute*> _attributes;

	Private(TokenDB::Connection *connection)
		: connection(connection) // connection is not owned by us!
	{
	}

	~Private()
	{
		for (std::map<CK_ATTRIBUTE_TYPE,OSAttribute*>::iterator it = _attributes.begin(); it!=_attributes.end(); ++it) {
			delete it->second;
			it->second = NULL;
		}
	}

	enum AttributeKind {
		akUnknown,
		akBoolean,
		akInteger,
		akBinary
	};

	AttributeKind attributeExists(CK_ATTRIBUTE_TYPE type, long long objectId)
	{
		TokenDB::Statement statement;
		TokenDB::Result result;

		// try to find the attribute in the boolean
		statement = connection->prepare(
			"select value from attribute_boolean where type=%d and object_id=%lld",
			type,
			objectId);
		if (!statement.isValid())
		{
			return akUnknown;
		}

		result = connection->perform(statement);
		if (result.isValid())
		{
			return akBoolean;
		}

		// try to find the attribute in the boolean
		statement = connection->prepare(
			"select value from attribute_integer where type=%d and object_id=%lld",
			type,
			objectId);
		if (!statement.isValid())
		{
			return akUnknown;
		}

		result = connection->perform(statement);
		if (result.isValid())
		{
			return akInteger;
		}

		// try to find the attribute in the boolean
		statement = connection->prepare(
			"select value from attribute_blob where type=%d and object_id=%lld",
			type,
			objectId);
		if (!statement.isValid())
		{
			return akUnknown;
		}

		result = connection->perform(statement);
		if (result.isValid())
		{
			return akBinary;
		}

		return akUnknown;
	}

	OSAttribute *getAttribute(CK_ATTRIBUTE_TYPE type, long long objectId)
	{
		// try to find the attribute in the boolean attribute table
		TokenDB::Statement statement = connection->prepare(
			"select value from attribute_boolean where type=%d and object_id=%lld",
			type,
			objectId);
		if (statement.isValid())
		{
			TokenDB::Result result = connection->perform(statement);
			if (result.isValid())
			{
				bool value = result.getInt(1) != 0;

				if (_attributes[type] && _attributes[type]->isBooleanAttribute())
					_attributes[type]->setBooleanValue(value);
				else
					_attributes[type] = new OSAttribute(value);

				return _attributes[type];
			}
		}

		// try to find the attribute in the integer attribute table
		statement = connection->prepare(
			"select value from attribute_integer where type=%d and object_id=%lld",
			type,
			objectId);
		if (statement.isValid())
		{
			TokenDB::Result result = connection->perform(statement);
			if (result.isValid())
			{
				unsigned long long value = result.getULongLong(1);
				if (_attributes[type] && _attributes[type]->isUnsignedLongAttribute())
					_attributes[type]->setUnsignedLongValue(value);
				else
					_attributes[type] = new OSAttribute(static_cast<unsigned long>(value));

				return _attributes[type];
			}
		}

		// try to find the attribute in the integer attribute table
		statement = connection->prepare(
			"select value from attribute_blob where type=%d and object_id=%lld",
			type,
			objectId);
		if (statement.isValid())
		{
			TokenDB::Result result = connection->perform(statement);
			if (result.isValid())
			{
				const unsigned char *value = result.getBinary(1);
				size_t size = result.getFieldLength(1);
				if (_attributes[type] && _attributes[type]->isByteStringAttribute())
					_attributes[type]->setByteStringValue(ByteString(value, size));
				else
					_attributes[type] = new OSAttribute(ByteString(value, size));

				return _attributes[type];
			}
		}

		// access integers
		// access binary
		return NULL;
	}
};

// Create an object that can access a record, but don't do anything yet.
DBObject::DBObject(TokenDB::Connection *connection)
	: _private(new DBObject::Private(connection)), _objectId(0)
{
}

// Destructor
DBObject::~DBObject()
{
	delete _private;
	_private = NULL;
}

void DBObject::dropConnection()
{
	_private->connection = NULL;
}

bool DBObject::hasConnection()
{
	return _private && _private->connection;
}

// create tables to support storage of attributes for the DBObject
bool DBObject::createTables()
{
	if (!hasConnection())
	{
		ERROR_MSG("Object is not connected to the database.");
		return false;
	}

	// Create the tables inside the database
	TokenDB::Statement cr_object = _private->connection->prepare("create table object (id integer primary key autoincrement);");
	if (!_private->connection->execute(cr_object))
	{
		ERROR_MSG("Failed to create \"object\" table");
		return false;
	}

	// attribute_text
	TokenDB::Statement cr_attr_text = _private->connection->prepare(
		"create table attribute_text ("
		"value text,"
		"type integer,"
		"object_id integer references object(id) on delete cascade,"
		"id integer primary key autoincrement)"
		);
	if (!_private->connection->execute(cr_attr_text))
	{
		ERROR_MSG("Failed to create \"attribute_text\" table");
		return false;
	}

	// attribute_integer
	TokenDB::Statement cr_attr_integer = _private->connection->prepare(
		"create table attribute_integer ("
		"value integer,"
		"type integer,"
		"object_id integer references object(id) on delete cascade,"
		"id integer primary key autoincrement)"
		);
	if (!_private->connection->execute(cr_attr_integer))
	{
		ERROR_MSG("Failed to create \"attribute_integer\" table");
		return false;
	}

	// attribute_blob
	TokenDB::Statement cr_attr_blob = _private->connection->prepare(
		"create table attribute_blob ("
		"value blob,"
		"type integer,"
		"object_id integer references object(id) on delete cascade,"
		"id integer primary key autoincrement)"
		);
	if (!_private->connection->execute(cr_attr_blob))
	{
		ERROR_MSG("Failed to create \"attribute_blob\" table");
		return false;
	}

	// attribute_boolean
	TokenDB::Statement cr_attr_boolean = _private->connection->prepare(
		"create table attribute_boolean ("
		"value boolean,"
		"type integer,"
		"object_id integer references object(id) on delete cascade,"
		"id integer primary key autoincrement)"
		);
	if (!_private->connection->execute(cr_attr_boolean))
	{
		ERROR_MSG("Failed to create \"attribute_boolean\" table");
		return false;
	}

	// attribute_datetime
	TokenDB::Statement cr_attr_datetime = _private->connection->prepare(
		"create table attribute_datetime ("
		"value datetime,"
		"type integer,"
		"object_id integer references object(id) on delete cascade,"
		"id integer primary key autoincrement)"
		);
	if (!_private->connection->execute(cr_attr_datetime))
	{
		ERROR_MSG("Failed to create \"attribute_datetime\" table");
		return false;
	}

	// attribute_real
	TokenDB::Statement cr_attr_real = _private->connection->prepare(
		"create table attribute_real ("
		"value real,"
		"type integer,"
		"object_id integer references object(id) on delete cascade,"
		"id integer primary key autoincrement)"
		);
	if (!_private->connection->execute(cr_attr_real))
	{
		ERROR_MSG("Failed to create \"attribute_real\" table");
		return false;
	}

	return true;
}

bool DBObject::find(long long objectId)
{
	if (!hasConnection())
	{
		ERROR_MSG("Object is not connected to the database.");
		return false;
	}


	if (objectId == 0) {
		ERROR_MSG("Invalid object_id 0 passed to find");
		return false;
	}

	// find the object in the database for the given object_id
	TokenDB::Statement statement = _private->connection->prepare(
				"select id from object where id=%lld",
				objectId);
	if (!statement.isValid()) {
		ERROR_MSG("Preparing object selection statement failed");
		return false;
	}

	TokenDB::Result result = _private->connection->perform(statement);
	if (result.getLongLong(1) != objectId) {
		ERROR_MSG("Failed to find object with id %lld",objectId);
		return false;
	}

	_objectId = objectId;
	return true;
}

bool DBObject::insert()
{
	if (!hasConnection())
	{
		ERROR_MSG("Object is not connected to the database.");
		return false;
	}

	TokenDB::Statement statement = _private->connection->prepare("insert into object default values");

	if (!_private->connection->execute(statement)) {
		ERROR_MSG("Failed to insert a new object");
		return false;
	}

	_objectId = _private->connection->lastInsertRowId();
	return _objectId != 0;
}

long long DBObject::objectId()
{
	return _objectId;
}

// Check if the specified attribute exists
bool DBObject::attributeExists(CK_ATTRIBUTE_TYPE type)
{
	if (!isValid())
	{
		ERROR_MSG("Cannot access invalid object.");
		return false;
	}

	// We have to search all attribute_xxxxx tables for a match on type and object_id.
	// Because it is known what type of attribute type is, we should be able to optimize this once we know the real type of the attribute.

	Private::AttributeKind ak = _private->attributeExists(type,_objectId);


	return ak != Private::akUnknown;
#if 0
	refresh();

	MutexLocker lock(objectMutex);

	return valid && (attributes[type] != NULL);
#endif
}

// Retrieve the specified attribute
OSAttribute* DBObject::getAttribute(CK_ATTRIBUTE_TYPE type)
{
	if (!isValid())
	{
		ERROR_MSG("Cannot read from invalid object.");
		return NULL;
	}


	return _private->getAttribute(type,_objectId);

#if 0
	refresh();

	MutexLocker lock(objectMutex);

	return attributes[type];
#else
	return NULL;
#endif
}

// Set the specified attribute
bool DBObject::setAttribute(CK_ATTRIBUTE_TYPE type, const OSAttribute& attribute)
{
	if (!isValid())
	{
		ERROR_MSG("Cannot update invalid object");
		return false;
	}

	Private::AttributeKind ak = _private->attributeExists(type,_objectId);
	TokenDB::Statement statement;

	// Update and existing attribute...
	switch (ak) {
		case Private::akBoolean:
			// update boolean attribute
			statement = _private->connection->prepare(
					"update attribute_boolean set value=%d where type=%d and object_id=%lld",
					attribute.getBooleanValue() ? 1 : 0,
					type,
					_objectId);

			if (!_private->connection->execute(statement))
			{
				ERROR_MSG("Failed to update boolean attribute %d for object %lld",type,_objectId);
				return false;
			}
			return true;

		case Private::akInteger:
			// update integer attribute
			statement = _private->connection->prepare(
					"update attribute_integer set value=%lld where type=%d and object_id=%lld",
					static_cast<long long>(attribute.getUnsignedLongValue()),
					type,
					_objectId);

			if (!_private->connection->execute(statement))
			{
				ERROR_MSG("Failed to update integer attribute %d for object %lld",type,_objectId);
				return false;
			}
			return true;


		case Private::akBinary:
			// update binary attribute
			statement = _private->connection->prepare(
					"update attribute_blob set value=? where type=%d and object_id=%lld",
					type,
					_objectId);

			statement.bindBlob(1, attribute.getByteStringValue().const_byte_str(), attribute.getByteStringValue().size(),NULL);

			if (!_private->connection->execute(statement))
			{
				ERROR_MSG("Failed to update blob attribute %d for object %lld",type,_objectId);
				return false;
			}
			return true;
	}


	// Insert the attribute, because it is currently uknown
	if (attribute.isBooleanAttribute())
	{
		// Could not update it, so we need to insert it.
		statement = _private->connection->prepare(
					"insert into attribute_boolean (value,type,object_id) values (%d,%d,%lld)",
					attribute.getBooleanValue() ? 1 : 0,
					type,
					_objectId);

		if (!_private->connection->execute(statement))
		{
			ERROR_MSG("Failed to insert boolean attribute %d for object %lld",type,_objectId);
			return false;
		}

		return true;
	}

	// Insert the attribute, because it is currently uknown
	if (attribute.isUnsignedLongAttribute())
	{
		// Could not update it, so we need to insert it.
		statement = _private->connection->prepare(
					"insert into attribute_integer (value,type,object_id) values (%lld,%d,%lld)",
					static_cast<long long>(attribute.getUnsignedLongValue()),
					type,
					_objectId);

		if (!_private->connection->execute(statement))
		{
			ERROR_MSG("Failed to insert integer attribute %d for object %lld",type,_objectId);
			return false;
		}

		return true;
	}


	// Insert the attribute, because it is currently uknown
	if (attribute.isByteStringAttribute())
	{
		// Could not update it, so we need to insert it.
		statement = _private->connection->prepare(
					"insert into attribute_blob (value,type,object_id) values (?,%d,%lld)",
					type,
					_objectId);

		statement.bindBlob(1, attribute.getByteStringValue().const_byte_str(), attribute.getByteStringValue().size(),NULL);

		if (!_private->connection->execute(statement))
		{
			ERROR_MSG("Failed to insert blob attribute %d for object %lld",type,_objectId);
			return false;
		}

		return true;
	}

	return false;
#if 0
	refresh();

	if (!valid)
	{
		DEBUG_MSG("Cannot update invalid object %s", path.c_str());

		return false;
	}

	{
		MutexLocker lock(objectMutex);

		if (attributes[type] != NULL)
		{
			delete attributes[type];

			attributes[type] = NULL;
		}

		attributes[type] = new OSAttribute(attribute);
	}

	store();

	return true;
#endif
}

// The validity state of the object
bool DBObject::isValid()
{
	return _objectId && hasConnection();
}

// Start an attribute set transaction; this method is used when - for
// example - a key is generated and all its attributes need to be
// persisted in one go.
//
// N.B.: Starting a transaction locks the object!
bool DBObject::startTransaction(Access access)
{
	if (!hasConnection())
	{
		ERROR_MSG("Object is not connected to the database.");
		return false;
	}

	// Always start a transaction that can be used for both reading and writing.
	if (access == ReadWrite)
		return _private->connection->beginTransactionRW();
	else
		return _private->connection->beginTransactionRO();
#if 0
	MutexLocker lock(objectMutex);

	if (inTransaction)
	{
		return false;
	}

	transactionLockFile = new File(path);

	if (!transactionLockFile->isValid() || !transactionLockFile->lock())
	{
		delete transactionLockFile;
		transactionLockFile = NULL;

		ERROR_MSG("Failed to lock file %s for attribute transaction", path.c_str());

		return false;
	}

	inTransaction = true;

	return true;
#endif
}

// Commit an attribute transaction
bool DBObject::commitTransaction()
{
	if (!hasConnection())
	{
		ERROR_MSG("Object is not connected to the database.");
		return false;
	}

	return _private->connection->commitTransaction();

#if 0
	{
		MutexLocker lock(objectMutex);

		if (!inTransaction)
		{
			return false;
		}

		// Unlock the file; theoretically, this can mean that another instance
		// of SoftHSM now gets the lock and writes back attributes that will be
		// overwritten when this transaction is committed. The chances of this
		// are deemed to be so small that we do nothing to prevent this...
		if (transactionLockFile == NULL)
		{
			ERROR_MSG("Transaction lock file instance invalid!");

			return false;
		}

		transactionLockFile->unlock();

		delete transactionLockFile;
		transactionLockFile = NULL;
		inTransaction = false;
	}

	store();

	return true;
#endif
}

// Abort an attribute transaction; loads back the previous version of the object from disk
bool DBObject::abortTransaction()
{
	if (!hasConnection())
	{
		ERROR_MSG("Object is not connected to the database.");
		return false;
	}

	return _private->connection->rollbackTransaction();

#if 0
	{
		MutexLocker lock(objectMutex);

		if (!inTransaction)
		{
			return false;
		}

		if (transactionLockFile == NULL)
		{
			ERROR_MSG("Transaction lock file instance invalid!");

			return false;
		}

		transactionLockFile->unlock();

		delete transactionLockFile;
		transactionLockFile = NULL;
		inTransaction = false;
	}

	// Force reload from disk
	refresh(true);

	return true;
#endif
}

// Destroy the object; WARNING: pointers to the object become invalid after this call
bool DBObject::destroyObject()
{
	if (!isValid())
	{
		ERROR_MSG("Object is not valid.");
		return false;
	}

#if 0
	if (token == NULL)
	{
		ERROR_MSG("Cannot destroy an object that is not associated with a token");

		return false;
	}
	return token->deleteObject(this);
#else
	return false;
#endif
}
