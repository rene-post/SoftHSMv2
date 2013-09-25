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
 DBObjectSpec.cpp

 Contains test cases to test the database token object implementation
 *****************************************************************************/

#include <igloo/igloo_alt.h>
using namespace igloo;

#include "DBObject.h"
#include "TokenDB.h"

#include <cstdio>

#ifndef HAVE_SQLITE3_H
#error expected sqlite3 to be available
#endif

Describe(a_dbobject)
{
	void SetUp()
	{
		// FIXME: this only works on *NIX/BSD, not on other platforms
		AssertThat(system("rm -rf testdir && mkdir testdir"), Equals(0));
	}

	void TearDown()
	{
		// FIXME: this only works on *NIX/BSD, not on other platforms
		AssertThat(system("rm -rf testdir"), Equals(0));
	}

	Describe(with_a_connection)
	{
		void SetUp()
		{
			Root().connection = TokenDB::Connection::Create("./testdir","TestToken");
			AssertThat(Root().connection, Is().Not().EqualTo((TokenDB::Connection*)NULL));
			AssertThat(Root().connection->connect("<1>"), IsTrue());
			Root().connection->setBusyTimeout(10);

			DBObject testObject(Root().connection);
			AssertThat(testObject.startTransaction(DBObject::ReadWrite), IsTrue());
			AssertThat(testObject.createTables(), IsTrue());
			AssertThat(testObject.commitTransaction(), IsTrue());

			Root().connection2 = TokenDB::Connection::Create("./testdir","TestToken");
			AssertThat(Root().connection2, Is().Not().EqualTo((TokenDB::Connection*)NULL));
			AssertThat(Root().connection2->connect("<2>"), IsTrue());
			Root().connection2->setBusyTimeout(10);
		}

		void TearDown()
		{
			AssertThat(Root().connection, Is().Not().EqualTo((TokenDB::Connection*)NULL));
			Root().connection->close();
			delete Root().connection;

			AssertThat(Root().connection2, Is().Not().EqualTo((TokenDB::Connection*)NULL));
			Root().connection2->close();
			delete Root().connection2;
		}

		It_Skip(should_be_insertable)
		{
			DBObject tokenObject(Root().connection);
			AssertThat(tokenObject.isValid(), IsFalse());
			AssertThat(tokenObject.insert(), IsTrue());
			AssertThat(tokenObject.isValid(), IsTrue());
			AssertThat(tokenObject.objectId(), Is().EqualTo(1));
		}

		It_Skip(should_be_selectable)
		{
			should_be_insertable();

			DBObject tokenObject(Root().connection);
			AssertThat(tokenObject.find(1), IsTrue());
			AssertThat(tokenObject.isValid(), IsTrue());
		}

		Describe(with_an_object)
		{
			void SetUp()
			{
				DBObject tokenObject(Root().connection);
				AssertThat(tokenObject.startTransaction(DBObject::ReadWrite), IsTrue());
				AssertThat(tokenObject.isValid(), IsFalse());
				AssertThat(tokenObject.insert(), IsTrue());
				AssertThat(tokenObject.isValid(), IsTrue());
				AssertThat(tokenObject.objectId(), Is().EqualTo(1));
				AssertThat(tokenObject.commitTransaction(), IsTrue());

			}

			void TearDown()
			{
			}

			It(should_store_boolean_attributes)
			{
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					bool value1 = true;
					bool value2 = false;
					bool value3 = true;
					bool value4 = true;
					bool value5 = false;

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3);
					OSAttribute attr4(value4);
					OSAttribute attr5(value5);

					AssertThat(testObject.setAttribute(CKA_TOKEN, attr1),IsTrue());
					AssertThat(testObject.setAttribute(CKA_SENSITIVE, attr2),IsTrue());
					AssertThat(testObject.setAttribute(CKA_EXTRACTABLE, attr3),IsTrue());
					AssertThat(testObject.setAttribute(CKA_NEVER_EXTRACTABLE, attr4),IsTrue());
					AssertThat(testObject.setAttribute(CKA_SIGN, attr5),IsTrue());
				}

				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.attributeExists(CKA_TOKEN), IsTrue());
					AssertThat(testObject.attributeExists(CKA_SENSITIVE), IsTrue());
					AssertThat(testObject.attributeExists(CKA_EXTRACTABLE), IsTrue());
					AssertThat(testObject.attributeExists(CKA_NEVER_EXTRACTABLE), IsTrue());
					AssertThat(testObject.attributeExists(CKA_SIGN), IsTrue());
					AssertThat(!testObject.attributeExists(CKA_ID), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_SENSITIVE)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_EXTRACTABLE)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_NEVER_EXTRACTABLE)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_SIGN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_ID), Is().EqualTo((OSAttribute*)NULL));

					AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_SENSITIVE)->getBooleanValue(), IsFalse());
					AssertThat(testObject.getAttribute(CKA_EXTRACTABLE)->getBooleanValue(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_NEVER_EXTRACTABLE)->getBooleanValue(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_SIGN)->getBooleanValue(), IsFalse());

					bool value6 = true;
					OSAttribute attr6(value6);

					AssertThat(testObject.setAttribute(CKA_VERIFY, attr6), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VERIFY)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VERIFY)->getBooleanValue(), Is().EqualTo(value6));
				}
			}

			It(should_store_unsigned_long_attributes)
			{
				// Add unsigned long attributes to the object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					unsigned long value1 = 0x12345678;
					unsigned long value2 = 0x87654321;
					unsigned long value3 = 0x01010101;
					unsigned long value4 = 0x10101010;
					unsigned long value5 = 0xABCDEF;

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3);
					OSAttribute attr4(value4);
					OSAttribute attr5(value5);

					AssertThat(testObject.setAttribute(CKA_MODULUS_BITS, attr1), IsTrue());
					AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
					AssertThat(testObject.setAttribute(CKA_AUTH_PIN_FLAGS, attr3), IsTrue());
					AssertThat(testObject.setAttribute(CKA_SUBPRIME_BITS, attr4), IsTrue());
					AssertThat(testObject.setAttribute(CKA_KEY_TYPE, attr5), IsTrue());
				}

				// Now read back the object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.attributeExists(CKA_MODULUS_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_PRIME_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_AUTH_PIN_FLAGS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_SUBPRIME_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_KEY_TYPE), IsTrue());
					AssertThat(!testObject.attributeExists(CKA_ID), IsTrue());

					AssertThat(testObject.getAttribute(CKA_MODULUS_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_AUTH_PIN_FLAGS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_SUBPRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_KEY_TYPE)->isUnsignedLongAttribute(), IsTrue());

					AssertThat(testObject.getAttribute(CKA_MODULUS_BITS)->getUnsignedLongValue(), Is().EqualTo(0x12345678));
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(0x87654321));
					AssertThat(testObject.getAttribute(CKA_AUTH_PIN_FLAGS)->getUnsignedLongValue(), Is().EqualTo(0x01010101));
					AssertThat(testObject.getAttribute(CKA_SUBPRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(0x10101010));
					AssertThat(testObject.getAttribute(CKA_KEY_TYPE)->getUnsignedLongValue(), Is().EqualTo(0xABCDEF));

					unsigned long value6 = 0x90909090;
					OSAttribute attr6(value6);

					AssertThat(testObject.setAttribute(CKA_CLASS, attr6), IsTrue());
					AssertThat(testObject.getAttribute(CKA_CLASS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_CLASS)->getUnsignedLongValue(), Is().EqualTo(value6));
				}
			}

			It(should_store_binary_attributes)
			{
				ByteString value1 = "010203040506070809";
				ByteString value2 = "ABABABABABABABABABABABABABABABABAB";
				ByteString value3 = "BDEBDBEDBBDBEBDEBE792759537328";
				ByteString value4 = "98A7E5D798A7E5D798A7E5D798A7E5D798A7E5D798A7E5D7";
				ByteString value5 = "ABCDABCDABCDABCDABCDABCDABCDABCD";

				// Create the test object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3);
					OSAttribute attr4(value4);
					OSAttribute attr5(value5);

					AssertThat(testObject.setAttribute(CKA_MODULUS, attr1), IsTrue());
					AssertThat(testObject.setAttribute(CKA_COEFFICIENT, attr2), IsTrue());
					AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());
					AssertThat(testObject.setAttribute(CKA_PUBLIC_EXPONENT, attr4), IsTrue());
					AssertThat(testObject.setAttribute(CKA_SUBJECT, attr5), IsTrue());
				}

				// Now read back the object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.attributeExists(CKA_MODULUS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_COEFFICIENT), IsTrue());
					AssertThat(testObject.attributeExists(CKA_VALUE_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_PUBLIC_EXPONENT), IsTrue());
					AssertThat(testObject.attributeExists(CKA_SUBJECT), IsTrue());
					AssertThat(!testObject.attributeExists(CKA_ID), IsTrue());

					AssertThat(testObject.getAttribute(CKA_MODULUS)->isByteStringAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_COEFFICIENT)->isByteStringAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PUBLIC_EXPONENT)->isByteStringAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_SUBJECT)->isByteStringAttribute(), IsTrue());

					AssertThat(testObject.getAttribute(CKA_MODULUS)->getByteStringValue(), Is().EqualTo(value1));
					AssertThat(testObject.getAttribute(CKA_COEFFICIENT)->getByteStringValue(), Is().EqualTo(value2));
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3));
					AssertThat(testObject.getAttribute(CKA_PUBLIC_EXPONENT)->getByteStringValue(), Is().EqualTo(value4));
					AssertThat(testObject.getAttribute(CKA_SUBJECT)->getByteStringValue(), Is().EqualTo(value5));

					ByteString value6 = "909090908080808080807070707070FF";
					OSAttribute attr6(value6);

					AssertThat(testObject.setAttribute(CKA_ISSUER, attr6), IsTrue());
					AssertThat(testObject.getAttribute(CKA_ISSUER)->isByteStringAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_ISSUER)->getByteStringValue(), Is().EqualTo(value6));
				}
			}

			It(should_store_mixed_attributes)
			{
				ByteString value3 = "BDEBDBEDBBDBEBDEBE792759537328";

				// Create the test object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					bool value1 = true;
					unsigned long value2 = 0x87654321;

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3);

					AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
					AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
					AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());
				}

				// Now read back the object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.attributeExists(CKA_TOKEN), IsTrue());
					AssertThat(testObject.attributeExists(CKA_PRIME_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_VALUE_BITS), IsTrue());
					AssertThat(!testObject.attributeExists(CKA_ID), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(0x87654321));
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3));
				}
			}

			It(should_store_double_attributes)
			{
				ByteString value3 = "BDEBDBEDBBDBEBDEBE792759537328";
				ByteString value3a = "466487346943785684957634";

				// Create the test object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					bool value1 = true;
					unsigned long value2 = 0x87654321;

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3);

					AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
					AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
					AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());
				}

				// Now read back the object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.attributeExists(CKA_TOKEN), IsTrue());
					AssertThat(testObject.attributeExists(CKA_PRIME_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_VALUE_BITS), IsTrue());
					AssertThat(!testObject.attributeExists(CKA_ID), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(0x87654321));
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3));

					bool value1 = false;
					unsigned long value2 = 0x76767676;

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3a);

					// Change the attributes
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
					AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
					AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());

					// Check the attributes
					AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1));
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2));
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));
				}

				// Now re-read back the object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.attributeExists(CKA_TOKEN), IsTrue());
					AssertThat(testObject.attributeExists(CKA_PRIME_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_VALUE_BITS), IsTrue());
					AssertThat(!testObject.attributeExists(CKA_ID), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

					bool value1 = false;
					unsigned long value2 = 0x76767676;

					AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1));
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2));
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));
				}
			}

			It(can_refresh_attributes)
			{
				ByteString value3 = "BDEBDBEDBBDBEBDEBE792759537328";
				ByteString value3a = "466487346943785684957634";

				// Create the test object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					bool value1 = true;
					unsigned long value2 = 0x87654321;

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3);

					AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
					AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
					AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());
				}

				// Now read back the object
				{
					DBObject testObject(Root().connection);
					AssertThat(testObject.find(1), IsTrue());
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.attributeExists(CKA_TOKEN), IsTrue());
					AssertThat(testObject.attributeExists(CKA_PRIME_BITS), IsTrue());
					AssertThat(testObject.attributeExists(CKA_VALUE_BITS), IsTrue());
					AssertThat(!testObject.attributeExists(CKA_ID), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(0x87654321));
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3));

					bool value1 = false;
					unsigned long value2 = 0x76767676;

					OSAttribute attr1(value1);
					OSAttribute attr2(value2);
					OSAttribute attr3(value3a);

					// Change the attributes
					AssertThat(testObject.isValid(), IsTrue());

					AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
					AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
					AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());

					// Check the attributes
					AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

					AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1));
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2));
					AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));

					// Open the object a second time
					DBObject testObject2(Root().connection);
					AssertThat(testObject2.find(1), IsTrue());
					AssertThat(testObject2.isValid(), IsTrue());

					// Check the attributes on the second instance
					AssertThat(testObject2.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
					AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

					AssertThat(testObject2.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1));
					AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2));
					AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));

					// Add an attribute on the second object
					ByteString id = "0102010201020102010201020102010201020102";

					OSAttribute attr4(id);

					AssertThat(testObject.setAttribute(CKA_ID, attr4), IsTrue());

					// Check the attribute
					AssertThat(testObject2.attributeExists(CKA_ID), IsTrue());
					AssertThat(testObject2.getAttribute(CKA_ID)->isByteStringAttribute(), IsTrue());
					AssertThat(testObject2.getAttribute(CKA_ID)->getByteStringValue(), Is().EqualTo(id));

					// Now check that the first instance also knows about it
					AssertThat(testObject.attributeExists(CKA_ID), IsTrue());
					AssertThat(testObject.getAttribute(CKA_ID)->isByteStringAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_ID)->getByteStringValue(), Is().EqualTo(id));

					// Now change another attribute
					unsigned long value2a = 0x89898989;

					OSAttribute attr2a(value2a);

					AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2a), IsTrue());

					// Check the attribute
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2a));

					// Now check that the second instance also knows about the change
					AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
					AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2a));
				}
			}

			It(should_cleanup_statements_during_transactions)
			{
				// Create an object for accessing object 1 on the first connection.
				DBObject testObject(Root().connection);
				// check transaction start(ro)/abort sequence
				AssertThat(testObject.startTransaction(OSObject::ReadOnly), IsTrue());
				AssertThat(testObject.find(1), IsTrue());
				AssertThat(testObject.isValid(), IsTrue());
				AssertThat(testObject.abortTransaction(), IsTrue());
			}

			It(should_use_transactions)
			{
				DBObject testObject(Root().connection);
				AssertThat(testObject.find(1), IsTrue());

				AssertThat(testObject.isValid(), IsTrue());

				bool value1 = true;
				unsigned long value2 = 0x87654321;
				ByteString value3 = "BDEBDBEDBBDBEBDEBE792759537328";

				OSAttribute attr1(value1);
				OSAttribute attr2(value2);
				OSAttribute attr3(value3);

				AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
				AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
				AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());

				// Create secondary instance for the same object
				DBObject testObject2(Root().connection2);
				AssertThat(testObject2.find(1), IsTrue());

				AssertThat(testObject2.isValid(), IsTrue());

				// Check that it has the same attributes
				AssertThat(testObject2.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject2.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1));
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2));
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3));

				// New values
				bool value1a = false;
				unsigned long value2a = 0x12345678;
				ByteString value3a = "ABABABABABABABABABABABABABABAB";

				OSAttribute attr1a(value1a);
				OSAttribute attr2a(value2a);
				OSAttribute attr3a(value3a);

				// Start transaction on object
				AssertThat(testObject.startTransaction(DBObject::ReadWrite), IsTrue());

				// Change the attributes
				AssertThat(testObject.setAttribute(CKA_TOKEN, attr1a), IsTrue());
				AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2a), IsTrue());
				AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3a), IsTrue());

				// Verify that the attributes were set
				AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1a));
				AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2a));
				AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));

				// Verify that they are unchanged on the other instance
				AssertThat(testObject2.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject2.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1));
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2));
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3));

				// Commit the transaction
				AssertThat(testObject.commitTransaction(), IsTrue());

				// Verify that they have now changed on the other instance
				AssertThat(testObject2.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject2.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1a));
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2a));
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));

				// Start transaction on object
				AssertThat(testObject.startTransaction(DBObject::ReadWrite), IsTrue());

				// Change the attributes
				AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
				AssertThat(testObject.setAttribute(CKA_PRIME_BITS, attr2), IsTrue());
				AssertThat(testObject.setAttribute(CKA_VALUE_BITS, attr3), IsTrue());

				// Verify that the attributes were set
				AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1));
				AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2));
				AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3));

				// Verify that they are unchanged on the other instance
				AssertThat(testObject2.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject2.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1a));
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2a));
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));

				// Abort the transaction
				AssertThat(testObject.abortTransaction(), IsTrue());

				// Verify that they are unchanged on both instances
				AssertThat(testObject.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1a));
				AssertThat(testObject.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2a));
				AssertThat(testObject.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));

				AssertThat(testObject2.getAttribute(CKA_TOKEN)->isBooleanAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->isUnsignedLongAttribute(), IsTrue());
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->isByteStringAttribute(), IsTrue());

				AssertThat(testObject2.getAttribute(CKA_TOKEN)->getBooleanValue(), Is().EqualTo(value1a));
				AssertThat(testObject2.getAttribute(CKA_PRIME_BITS)->getUnsignedLongValue(), Is().EqualTo(value2a));
				AssertThat(testObject2.getAttribute(CKA_VALUE_BITS)->getByteStringValue(), Is().EqualTo(value3a));
			}
		};
	};

	TokenDB::Connection *connection;
	TokenDB::Connection *connection2;
};

