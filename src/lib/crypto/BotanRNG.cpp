/*
 * Copyright (c) 2010 .SE (The Internet Infrastructure Foundation)
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
 BOTANRNG.cpp

 Botan random number generator class
 *****************************************************************************/

#include "config.h"
#include "BotanRNG.h"

#include <botan/libstate.h>

// Base constructor
BotanRNG::BotanRNG()
{
	rng = &Botan::global_state().global_rng();
}

// Destructor
BotanRNG::~BotanRNG()
{
}

// Generate random data
bool BotanRNG::generateRandom(ByteString& data, const size_t len)
{
	data.wipe(len);

	rng->randomize(&data[0], len);

	return true;
}

// Seed the random pool
void BotanRNG::seed(ByteString& seedData)
{
	rng->add_entropy(seedData.byte_str(), seedData.size());
	rng->reseed(seedData.size());
}

// Get the RNG
Botan::RandomNumberGenerator* BotanRNG::getRNG()
{
	return rng;
}
