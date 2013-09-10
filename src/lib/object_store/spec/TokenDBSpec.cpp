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
 TokenDBSpec.cpp

 Contains test cases to test the token DB implementation
 *****************************************************************************/

#include <igloo/igloo_alt.h>
using namespace igloo;

#include "TokenDB.h"

int dummy_print(const char *format, va_list ap)
{
}

Describe(a_token_db)
{
	TokenDB::Connection *null;

	void SetUp()
	{
		// FIXME: this only works on *NIX/BSD, not on other platforms
		AssertThat(system("rm -rf testdir && mkdir testdir"), Equals(0));
		null = NULL;
	}

	void TearDown()
	{
		// FIXME: this only works on *NIX/BSD, not on other platforms
		AssertThat(system("rm -rf testdir"), Equals(0));
	}

	It(checks_for_empty_connection_parameters)
	{
		TokenDB::setLogErrorHandler(dummy_print);

		TokenDB::Connection *connection = TokenDB::Connection::Create("","TestToken");
		AssertThat(connection, Is().EqualTo(null));

		connection = TokenDB::Connection::Create("./testdir","");
		AssertThat(connection, Is().EqualTo(null));

		connection = TokenDB::Connection::Create("","");
		AssertThat(connection, Is().EqualTo(null));

		TokenDB::resetLogErrorHandler();
	}

	It(can_be_connected_to_database)
	{
		TokenDB::Connection *connection = TokenDB::Connection::Create("./testdir","TestToken");
		AssertThat(connection, Is().Not().EqualTo(null));
		bool isConnected = connection->connect();
		delete connection;
		AssertThat(isConnected, IsTrue());
		AssertThat(system("test -f ./testdir/TestToken"), Equals(0));
	}

	Describe(with_a_connection)
	{
		void SetUp()
		{
			connection = TokenDB::Connection::Create("./testdir","TestToken");
			AssertThat(connection, Is().Not().EqualTo(Root().null));
			AssertThat(connection->connect(), IsTrue());
		}

		void TearDown()
		{
			AssertThat(connection, Is().Not().EqualTo(Root().null));
			connection->close();
			delete connection;
		}

		It(can_prepare_statements)
		{
			TokenDB::Statement statement = connection->prepare("PRAGMA database_list;");
			AssertThat(statement.isValid(), IsTrue());
		}

		It(can_perform_statements)
		{
			TokenDB::Statement statement = connection->prepare("PRAGMA database_list;");
			AssertThat(statement.isValid(), IsTrue());
			TokenDB::Result result = connection->perform(statement);
			AssertThat(result.isValid(), IsTrue());
			// only expect a single row in the result, so nextRow should now fail
			AssertThat(result.nextRow(), IsFalse());
		}

		It(maintains_correct_refcounts)
		{
			TokenDB::Statement statement = connection->prepare("PRAGMA database_list;");
			AssertThat(statement.refcount(), Equals(1));
			{
				TokenDB::Statement statement1 = statement;
				TokenDB::Statement statement2 = statement;
				AssertThat(statement.refcount(), Equals(3));
				AssertThat(statement1.isValid(), IsTrue());
				AssertThat(statement2.isValid(), IsTrue());
			}
			AssertThat(statement.isValid(), IsTrue());
			AssertThat(statement.refcount(), Equals(1));

			TokenDB::Result result = connection->perform(statement);
			AssertThat(result.isValid(), IsTrue());

			// Statement is referenced by the result because it provides the query record cursor state.
			AssertThat(statement.refcount(), Equals(2));

			result = TokenDB::Result();
			AssertThat(statement.refcount(), Equals(1));
		}

		It(can_create_tables)
		{
			AssertThat(connection->tableExists("object"), IsFalse());
			TokenDB::Statement cr_object = connection->prepare("create table object (id integer primary key autoincrement);");
			AssertThat(connection->execute(cr_object), IsTrue());
			AssertThat(connection->tableExists("object"), IsTrue());
		}

		Describe(with_tables)
		{
			void SetUp()
			{
				Parent().can_create_tables();

				// attribute_text
				AssertThat(Parent().connection->tableExists("attribute_text"), IsFalse());
				TokenDB::Statement cr_attr_text = Parent().connection->prepare(
					"create table attribute_text ("
					"value text,"
					"type integer,"
					"object_id integer references object(id) on delete cascade,"
					"id integer primary key autoincrement)"
					);
				AssertThat(Parent().connection->execute(cr_attr_text), IsTrue());
				AssertThat(Parent().connection->tableExists("attribute_text"), IsTrue());

				// attribute_integer
				AssertThat(Parent().connection->tableExists("attribute_integer"), IsFalse());
				TokenDB::Statement cr_attr_integer = Parent().connection->prepare(
					"create table attribute_integer ("
					"value integer,"
					"type integer,"
					"object_id integer references object(id) on delete cascade,"
					"id integer primary key autoincrement)"
					);
				AssertThat(Parent().connection->execute(cr_attr_integer), IsTrue());
				AssertThat(Parent().connection->tableExists("attribute_integer"), IsTrue());

				// attribute_blob
				AssertThat(Parent().connection->tableExists("attribute_blob"), IsFalse());
				TokenDB::Statement cr_attr_blob = Parent().connection->prepare(
					"create table attribute_blob ("
					"value blob,"
					"type integer,"
					"object_id integer references object(id) on delete cascade,"
					"id integer primary key autoincrement)"
					);
				AssertThat(Parent().connection->execute(cr_attr_blob), IsTrue());
				AssertThat(Parent().connection->tableExists("attribute_blob"), IsTrue());

				// attribute_boolean
				AssertThat(Parent().connection->tableExists("attribute_boolean"), IsFalse());
				TokenDB::Statement cr_attr_boolean = Parent().connection->prepare(
					"create table attribute_boolean ("
					"value boolean,"
					"type integer,"
					"object_id integer references object(id) on delete cascade,"
					"id integer primary key autoincrement)"
					);
				AssertThat(Parent().connection->execute(cr_attr_boolean), IsTrue());
				AssertThat(Parent().connection->tableExists("attribute_boolean"), IsTrue());

				// attribute_datetime
				AssertThat(Parent().connection->tableExists("attribute_datetime"), IsFalse());
				TokenDB::Statement cr_attr_datetime = Parent().connection->prepare(
					"create table attribute_datetime ("
					"value datetime,"
					"type integer,"
					"object_id integer references object(id) on delete cascade,"
					"id integer primary key autoincrement)"
					);
				AssertThat(Parent().connection->execute(cr_attr_datetime), IsTrue());
				AssertThat(Parent().connection->tableExists("attribute_datetime"), IsTrue());

				// attribute_real
				AssertThat(Parent().connection->tableExists("attribute_real"), IsFalse());
				TokenDB::Statement cr_attr_real = Parent().connection->prepare(
					"create table attribute_real ("
					"value real,"
					"type integer,"
					"object_id integer references object(id) on delete cascade,"
					"id integer primary key autoincrement)"
					);
				AssertThat(Parent().connection->execute(cr_attr_real), IsTrue());
				AssertThat(Parent().connection->tableExists("attribute_real"), IsTrue());
			}

			void TearDown()
			{
			}

			It(can_insert_records)
			{
				TokenDB::Statement statement = Parent().connection->prepare("insert into object default values");
				AssertThat(Parent().connection->execute(statement), IsTrue());
				long long object_id = Parent().connection->lastInsertRowId();
				AssertThat(object_id, Is().Not().EqualTo(0));

				statement = Parent().connection->prepare(
							"insert into attribute_text (value,type,object_id) values ('%s',%d,%lld)",
							"testing testing testing",
							1234,
							object_id);
				AssertThat(Parent().connection->execute(statement), IsTrue());
			}

			It(can_retrieve_records)
			{
				can_insert_records();

				TokenDB::Statement statement = Parent().connection->prepare(
							"select value from attribute_text as t where t.type=%d",
							1234);
				TokenDB::Result result = Parent().connection->perform(statement);
				AssertThat(result.getString(1), Is().EqualTo("testing testing testing"));
			}

			It(can_cascade_delete_objects_and_attributes)
			{
				can_insert_records();

				TokenDB::Statement statement = Parent().connection->prepare("select id from object");
				TokenDB::Result result = Parent().connection->perform(statement);
				AssertThat(result.isValid(),IsTrue());

				long long object_id = result.getLongLong(1);

				statement = Parent().connection->prepare("delete from object where id=%lld",object_id);
				AssertThat(Parent().connection->execute(statement), IsTrue());

				statement = Parent().connection->prepare("select * from attribute_text where object_id=%lld",object_id);
				result = Parent().connection->perform(statement);

				// Check cascade delete was successful.
				AssertThat(result.isValid(), IsFalse());
			}

			It(can_update_text_attribute)
			{
				can_insert_records();

				// query all objects
				TokenDB::Statement statement = Parent().connection->prepare("select id from object");
				AssertThat(statement.isValid(), IsTrue());
				TokenDB::Result result = Parent().connection->perform(statement);
				AssertThat(result.isValid(),IsTrue());

				long long object_id = result.getLongLong(1); // field indices start at 1

				statement = Parent().connection->prepare(
							"update attribute_text set value='test test test' where type=%d and object_id=%lld",
							1234,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());
			}

			It(can_update_text_attribute_bound_value)
			{
				can_insert_records();

				// query all objects
				TokenDB::Statement statement = Parent().connection->prepare("select id from object");
				AssertThat(statement.isValid(), IsTrue());
				TokenDB::Result result = Parent().connection->perform(statement);
				AssertThat(result.isValid(),IsTrue());

				long long object_id = result.getLongLong(1); // field indices start at 1

				statement = Parent().connection->prepare(
							"update attribute_text set value=? where type=%d and object_id=%lld",
							1234,
							object_id);
				AssertThat(statement.isValid(), IsTrue());

				std::string msg("testing quote ' and accents é.");

				AssertThat(statement.bindText(1,msg.c_str(),msg.size(),NULL), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());

				statement = Parent().connection->prepare(
							"select value from attribute_text as t where t.type=%d and t.object_id=%lld",
							1234,
							object_id);
				result = Parent().connection->perform(statement);
				AssertThat(result.getString(1), Is().EqualTo(msg));

			}

			It(can_update_integer_attribute_bound_value)
			{
				// insert new object
				TokenDB::Statement statement = Parent().connection->prepare(
							"insert into object default values");
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());
				long long object_id = Parent().connection->lastInsertRowId();
				AssertThat(object_id, Is().Not().EqualTo(0));

				// insert integer attribute
				statement = Parent().connection->prepare(
							"insert into attribute_integer (value,type,object_id) values (%lld,%d,%lld)",
							1111,
							1235,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// prepare update integer attribute statement
				statement = Parent().connection->prepare(
							"update attribute_integer set value=? where type=%d and object_id=%lld",
							1235,
							object_id);
				AssertThat(statement.isValid(), IsTrue());

				// bind long long value to the parameter an update the record
				AssertThat(statement.bindInt64(1,2222), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// Retrieve the value from the record
				TokenDB::Statement retrieveStmt = Parent().connection->prepare(
							"select value from attribute_integer as t where t.type=%d and t.object_id=%lld",
							1235,
							object_id);
				AssertThat(retrieveStmt.isValid(), IsTrue());
				TokenDB::Result result = Parent().connection->perform(retrieveStmt);
				AssertThat(result.getLongLong(1), Is().EqualTo(2222));

				// verify that binding to a parameter before resetting the statement will fail.
				TokenDB::setLogErrorHandler(dummy_print);
				AssertThat(statement.bindInt(1,3333), IsFalse());
				TokenDB::resetLogErrorHandler();

				// reset statement and bind another value to the statement
				AssertThat(statement.reset(), IsTrue());
				AssertThat(statement.bindInt(1,3333), IsTrue());

				// perform the update statement again with the newly bound value
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// reset the retrieve statment and perform it again to get the latest value of the integer attribute
				AssertThat(retrieveStmt.reset(), IsTrue());
				result = Parent().connection->perform(retrieveStmt);
				AssertThat(result.isValid(), IsTrue());
				AssertThat(result.getLongLong(1), Is().EqualTo(3333));
			}

			It(can_update_blob_attribute_bound_value)
			{
				// insert new object
				TokenDB::Statement statement = Parent().connection->prepare(
							"insert into object default values");
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());
				long long object_id = Parent().connection->lastInsertRowId();
				AssertThat(object_id, Is().Not().EqualTo(0));

				// insert blob attribute
				statement = Parent().connection->prepare(
							"insert into attribute_blob (value,type,object_id) values (X'012345',%d,%lld)",
							1236,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// prepare update blob attribute statement
				statement = Parent().connection->prepare(
							"update attribute_blob set value=? where type=%d and object_id=%lld",
							1236,
							object_id);
				AssertThat(statement.isValid(), IsTrue());

				// bind blob (with embedded zero!) to the parameter
				const char data[] = {10,11,0,12,13,14,15,16};
				std::string msg(data,sizeof(data));
				AssertThat(statement.bindBlob(1,msg.data(),msg.size(),NULL), IsTrue());

				// update the blob value of the attribute
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// retrieve the blob value from the attribute
				statement = Parent().connection->prepare(
							"select value from attribute_blob as t where t.type=%d and t.object_id=%lld",
							1236,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				TokenDB::Result result = Parent().connection->perform(statement);
				AssertThat(result.isValid(), IsTrue());

				// check that the retrieved blob value matches the original data.
				AssertThat(result.getFieldLength(1), Is().EqualTo(sizeof(data)));
				std::string msgstored((const char *)result.getBinary(1),result.getFieldLength(1));
				AssertThat(msg, Is().EqualTo(msgstored));
			}

			It(can_update_boolean_attribute_bound_value)
			{
				//SQLite doesn't have a boolean data type, use 0 (false) and 1 (true)

				// insert new object
				TokenDB::Statement statement = Parent().connection->prepare(
							"insert into object default values");
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());
				long long object_id = Parent().connection->lastInsertRowId();
				AssertThat(object_id, Is().Not().EqualTo(0));

				// insert boolean attribute
				statement = Parent().connection->prepare(
							"insert into attribute_boolean (value,type,object_id) values (1,%d,%lld)",
							1237,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// prepare update boolean attribute statement
				statement = Parent().connection->prepare(
							"update attribute_boolean set value=? where type=%d and object_id=%lld",
							1237,
							object_id);
				AssertThat(statement.isValid(), IsTrue());

				// Bind 0 (false) to the first parameter
				AssertThat(statement.bindInt(1,0), IsTrue());

				// Execute the statement to update the attribute value.
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// Retrieve the int value from the attribute
				statement = Parent().connection->prepare(
							"select value from attribute_boolean as t where t.type=%d and t.object_id=%lld",
							1237,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				TokenDB::Result result = Parent().connection->perform(statement);
				AssertThat(result.isValid(), IsTrue());

				// check that the retrieved value matches the original value
				AssertThat(result.getInt(1), Equals(0));
			}

			It(can_update_real_attribute_bound_value)
			{
				// insert new object
				TokenDB::Statement statement = Parent().connection->prepare(
							"insert into object default values");
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());
				long long object_id = Parent().connection->lastInsertRowId();
				AssertThat(object_id, Is().Not().EqualTo(0));

				// insert real value
				statement = Parent().connection->prepare(
							"insert into attribute_real (value,type,object_id) values(%f,%d,%lld)",
							1.238,
							1238,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// prepare update real attribute statement
				statement = Parent().connection->prepare(
							"update attribute_real set value=? where type=%d and object_id=%lld",
							1238,
							object_id);
				AssertThat(statement.isValid(), IsTrue());

				// Bind 3333.3333 to the first parameter
				AssertThat(statement.bindDouble(1,3333.3333), IsTrue());

				// Execute the statement to update the attribute value
				AssertThat(Parent().connection->execute(statement), IsTrue());

				// Retrieve the double value from the attribute
				statement = Parent().connection->prepare(
							"select value from attribute_real as t where t.type=%d and t.object_id=%lld",
							1238,
							object_id);
				AssertThat(statement.isValid(), IsTrue());
				TokenDB::Result result = Parent().connection->perform(statement);
				AssertThat(result.isValid(), IsTrue());

				// check that the retrieved value matches the original value.
				AssertThat(result.getDouble(1), Is().EqualToWithDelta(3333.3333,0.00001));
			}
		};

		TokenDB::Connection *connection;
	};
};
