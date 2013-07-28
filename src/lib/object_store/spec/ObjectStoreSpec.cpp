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
 ObjectStoreSpec.cpp

 Contains test cases to test the object store implementation
 *****************************************************************************/

#include <igloo/igloo_alt.h>
using namespace igloo;

#include "ObjectStore.h"

Describe(a_newly_created_object_store)
{
	void SetUp()
	{
		// FIXME: this only works on *NIX/BSD, not on other platforms
		Assert::That(system("rm -rf testdir && mkdir testdir"), Equals(0));

		store = new ObjectStore("./testdir");
		nulltoken = NULL;
		label1 = "DEADC0FFEE";
		label2 = "DEADBEEF";
	}

	void TearDown()
	{
		delete store;

		// FIXME: this only works on *NIX/BSD, not on other platforms
		Assert::That(system("rm -rf testdir"), Equals(0));
	}

	It(contains_no_items)
	{
		Assert::That(store->getTokenCount(), Equals(0));
	}

	It(can_create_a_new_token)
	{
		OSToken *token1 = store->newToken(label1);
		Assert::That(token1, Is().Not().EqualTo( Root().nulltoken));
		Assert::That(store->getTokenCount(), Is().EqualTo(1));
	}

	Describe(containing_two_tokens)
	{
		void SetUp()
		{
			OSToken* token1 = Root().store->newToken(Root().label1);
			Assert::That(token1, Is().Not().EqualTo(Root().nulltoken));
			Assert::That(Root().store->getTokenCount(), Is().EqualTo(1));

			OSToken* token2 = Root().store->newToken(Root().label2);
			Assert::That(token2, Is().Not().EqualTo(Root().nulltoken));
			Assert::That(Root().store->getTokenCount(), Is().EqualTo(2));
		}

		void TearDown()
		{
			OSToken* token1 = Root().store->getToken(0);
			OSToken* token2 = Root().store->getToken(1);
			Assert::That(Root().store->destroyToken(token1), IsTrue());
			Assert::That(Root().store->destroyToken(token2), IsTrue());
		}

		It(has_two_tokens)
		{
			Assert::That(Root().store->getTokenCount(), Is().EqualTo(2));
		}

		It(can_access_both_tokens)
		{
			// Retrieve both tokens and check that both are present
			OSToken* token1 = Root().store->getToken(0);
			OSToken* token2 = Root().store->getToken(1);

			Assert::That(token1, Is().Not().EqualTo(Root().nulltoken));
			Assert::That(token2, Is().Not().EqualTo(Root().nulltoken));
		}

		It(assigned_labels_correctly_to_tokens)
		{
			// Retrieve both tokens and check that both are present
			OSToken* token1 = Root().store->getToken(0);
			OSToken* token2 = Root().store->getToken(1);

			ByteString retrieveLabel1, retrieveLabel2;

			Assert::That(token1->getTokenLabel(retrieveLabel1), IsTrue());
			Assert::That(token2->getTokenLabel(retrieveLabel2), IsTrue());

			Assert::That(Root().label1, Is().EqualTo(retrieveLabel1).Or().EqualTo(retrieveLabel2));
			Assert::That(Root().label2, Is().EqualTo(retrieveLabel1).Or().EqualTo(retrieveLabel2));
			Assert::That(Root().label1, Is().Not().EqualTo(Root().label2));
		}

		It(assigned_a_unique_serial_number_to_each_token)
		{
			// Retrieve both tokens and check that both are present
			OSToken* token1 = Root().store->getToken(0);
			OSToken* token2 = Root().store->getToken(1);

			ByteString retrieveSerial1, retrieveSerial2;

			Assert::That(token1->getTokenSerial(retrieveSerial1), IsTrue());
			Assert::That(token2->getTokenSerial(retrieveSerial2), IsTrue());

			Assert::That(retrieveSerial1,Is().Not().EqualTo(retrieveSerial2));
		}
	};


	ObjectStore *store;
	OSToken *nulltoken;
	ByteString label1;
	ByteString label2;
};
