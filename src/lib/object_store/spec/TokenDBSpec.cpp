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
		Assert::That(system("rm -rf testdir && mkdir testdir"), Equals(0));
		null = NULL;
	}

	void TearDown()
	{
		// FIXME: this only works on *NIX/BSD, not on other platforms
		Assert::That(system("rm -rf testdir"), Equals(0));
	}

	It(checks_for_empty_connection_parameters)
	{
		TokenDB::setLogErrorHandler(dummy_print);

		TokenDB::Connection *connection = TokenDB::Connection::Create("","TestToken");
		Assert::That(connection, Is().EqualTo(null));

		connection = TokenDB::Connection::Create("./testdir","");
		Assert::That(connection, Is().EqualTo(null));

		connection = TokenDB::Connection::Create("","");
		Assert::That(connection, Is().EqualTo(null));

		TokenDB::resetLogErrorHandler();
	}

	It(can_be_connected_to_database)
	{
		TokenDB::Connection *connection = TokenDB::Connection::Create("./testdir","TestToken");
		Assert::That(connection, Is().Not().EqualTo(null));
		bool isConnected = connection->connect();
		delete connection;
		Assert::That(isConnected, IsTrue());
		Assert::That(system("test -f ./testdir/TestToken"), Equals(0));
	}

	Describe(with_a_connection)
	{
		void SetUp()
		{
			connection = TokenDB::Connection::Create("./testdir","TestToken");
			Assert::That(connection, Is().Not().EqualTo(Root().null));
			Assert::That(connection->connect(), IsTrue());
		}

		void TearDown()
		{
			Assert::That(connection, Is().Not().EqualTo(Root().null));
			connection->close();
			delete connection;
		}

		It(can_prepare_statements)
		{
			TokenDB::Statement statement = connection->prepare("PRAGMA database_list;");
			Assert::That(statement.isValid(), IsTrue());
		}

		It(can_copy_statements)
		{
			TokenDB::Statement statement = connection->prepare("PRAGMA database_list;");
			{
				TokenDB::Statement statement1 = statement;
				TokenDB::Statement statement2 = statement;
				Assert::That(statement.refcount(), Equals(3));
				Assert::That(statement1.isValid(), IsTrue());
				Assert::That(statement2.isValid(), IsTrue());
			}
			Assert::That(statement.isValid(), IsTrue());
			Assert::That(statement.refcount(), Equals(1));
		}

		It(can_create_tables)
		{
			Assert::That(connection->tableExists("object"), IsFalse());
			TokenDB::Statement cr_object = connection->prepare("create table object (id integer primary key autoincrement);");
			Assert::That(connection->execute(cr_object), IsTrue());
			Assert::That(connection->tableExists("object"), IsTrue());
		}

		Describe(with_tables)
		{
			void SetUp()
			{
				Parent().can_create_tables();

				Assert::That(Parent().connection->tableExists("attribute_text"), IsFalse());
				TokenDB::Statement cr_attr_text = Parent().connection->prepare(
					"create table attribute_text ("
					"value text,"
					"type integer,"
					"object_id integer references object(id) on delete cascade,"
					"id integer primary key autoincrement)"
					);
				Assert::That(Parent().connection->execute(cr_attr_text), IsTrue());
				Assert::That(Parent().connection->tableExists("attribute_text"), IsTrue());
			}

			void TearDown()
			{
			}

			It(can_insert_records)
			{
				TokenDB::Statement statement = Parent().connection->prepare("insert into object default values");
				Assert::That(Parent().connection->execute(statement), IsTrue());
				long long object_id = Parent().connection->lastInsertRowId();
				Assert::That(object_id, Is().Not().EqualTo(0));

				statement = Parent().connection->prepare(
							"insert into attribute_text (value,type,object_id) values ('%s',%d,%lld)",
							"testing testing testing",
							1234,
							object_id);
				Assert::That(Parent().connection->execute(statement), IsTrue());
			}

			It(can_retrieve_records)
			{
				can_insert_records();

				TokenDB::Statement statement = Parent().connection->prepare(
							"select value from attribute_text as t where t.type=%d",
							1234);
				TokenDB::Result result = Parent().connection->perform(statement);
				Assert::That(statement.refcount(), Equals(2));
				Assert::That(result.getString(1), Is().EqualTo("testing testing testing"));
			}

			It(can_cascade_delete_objects_and_attributes)
			{
				can_insert_records();

				TokenDB::Statement statement = Parent().connection->prepare("select id from object");
				TokenDB::Result result = Parent().connection->perform(statement);
				Assert::That(result.isValid(),IsTrue());

				long long object_id = result.getLongLong(1);

				statement = Parent().connection->prepare("delete from object where id = %lld",object_id);
				Assert::That(Parent().connection->execute(statement), IsTrue());

				statement = Parent().connection->prepare("select * from attribute_text where object_id = %lld",object_id);
				result = Parent().connection->perform(statement);

				// Check cascade delete was successful.
				Assert::That(result.isValid(), IsFalse());
			}

		};

		TokenDB::Connection *connection;
	};
};
