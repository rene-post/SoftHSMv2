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
		Assert::That(system("rm -rf testdir && mkdir testdir"), Is().EqualTo(0));
	}

	void TearDown()
	{
		Assert::That(system("rm -rf testdir"), Is().EqualTo(0));
	}


	It(can_be_created)
	{
		ObjectFile testObject(NULL, "testdir/testobject", true);
		Assert::That(testObject.isValid(), IsTrue());
	}

	It(can_store_boolean_attributes)
	{
		ObjectFile testObject(NULL, "testdir/testobject", true);
		Assert::That(testObject.isValid(), IsTrue());
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

		Assert::That(testObject.setAttribute(CKA_TOKEN, attr1), IsTrue());
		Assert::That(testObject.setAttribute(CKA_SENSITIVE, attr2), IsTrue());
		Assert::That(testObject.setAttribute(CKA_EXTRACTABLE, attr3), IsTrue());
		Assert::That(testObject.setAttribute(CKA_NEVER_EXTRACTABLE, attr4), IsTrue());
		Assert::That(testObject.setAttribute(CKA_SIGN, attr5), IsTrue());
	}

	It(can_remember_boolean_attributes)
	{


	}


};
