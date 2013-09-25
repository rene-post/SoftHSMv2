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
 DBTokenSpec.cpp

 Contains test cases to test the database token implementation
 *****************************************************************************/

#include <igloo/igloo_alt.h>
using namespace igloo;

#include "DBToken.h"

#include <cstdio>

#ifndef HAVE_SQLITE3_H
#error expected sqlite3 to be available
#endif

Describe(a_dbtoken)
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



	It(should_be_creatable)
	{
		ByteString label = "40414243"; // ABCD
		ByteString serial = "0102030405060708";

		OSToken* newToken = new DBToken("./testdir", "newToken", label, serial);


		AssertThat(newToken, Is().Not().EqualTo((OSToken*)NULL));

		AssertThat(newToken->isValid(), IsTrue());
	}



};
