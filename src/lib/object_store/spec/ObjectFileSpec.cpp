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
 ObjectFileSpec.cpp

 Contains test cases to test the object file implementation
 *****************************************************************************/

#include <igloo/igloo_alt.h>
using namespace igloo;

#include "ObjectFile.h"

Describe(an_object_file)
{
	void SetUp()
	{
		// FIXME: this only works on *NIX/BSD, not on other platforms
		AssertThat(system("rm -rf testdir && mkdir testdir"), Is().EqualTo(0));
	}

	void TearDown()
	{
		AssertThat(system("rm -rf testdir"), Is().EqualTo(0));
	}


	It(can_be_created)
	{
		ObjectFile testObject(NULL, "testdir", "testobject", true);
		AssertThat(testObject.isValid(), IsTrue());
	}

	It(can_store_boolean_attributes)
	{
		ObjectFile testObject(NULL, "testdir", "testobject", true);
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

		AssertThat(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
		AssertThat(testObject.setAttribute(CKA_SENSITIVE, attr2), IsTrue());
		AssertThat(testObject.setAttribute(CKA_EXTRACTABLE, attr3), IsTrue());
		AssertThat(testObject.setAttribute(CKA_NEVER_EXTRACTABLE, attr4), IsTrue());
		AssertThat(testObject.setAttribute(CKA_SIGN, attr5), IsTrue());
	}


	It(can_retrieve_boolean_attributes)
	{
		can_store_boolean_attributes();

		ObjectFile testObject(NULL, "testdir", "testobject");

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
};
