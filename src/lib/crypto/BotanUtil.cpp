/*
 * Copyright (c) .SE (The Internet Infrastructure Foundation)
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
 BotanUtil.h

 Botan convenience functions
 *****************************************************************************/

#include "config.h"
#include "BotanUtil.h"
#include <botan/der_enc.h>
#include <botan/ber_dec.h>
#include <botan/asn1_obj.h>

// Convert a Botan BigInt to a ByteString
ByteString BotanUtil::bigInt2ByteString(const Botan::BigInt& bigInt)
{
	ByteString rv;

	rv.resize(bigInt.bytes());
	bigInt.binary_encode(&rv[0]);

	return rv;
}

// Convert a ByteString to an Botan BigInt
Botan::BigInt BotanUtil::byteString2bigInt(const ByteString& byteString)
{
	return Botan::BigInt(byteString.const_byte_str(), byteString.size());
}

#ifdef WITH_ECC
// Convert a Botan EC group to a ByteString
ByteString BotanUtil::ecGroup2ByteString(const Botan::EC_Group& ecGroup)
{
	Botan::SecureVector<Botan::byte> der = ecGroup.DER_encode(Botan::EC_DOMPAR_ENC_OID);
	return ByteString(&der[0], der.size());
}

// Convert a ByteString to a Botan EC group
Botan::EC_Group BotanUtil::byteString2ECGroup(const ByteString& byteString)
{
	return Botan::EC_Group(Botan::MemoryVector<Botan::byte>(byteString.const_byte_str(), byteString.size()));
}

// Convert a Botan EC point to a ByteString
ByteString BotanUtil::ecPoint2ByteString(const Botan::PointGFp& ecPoint)
{
	ByteString point;

	try
	{
		Botan::SecureVector<Botan::byte> repr = Botan::EC2OSP(ecPoint, Botan::PointGFp::UNCOMPRESSED);
		Botan::SecureVector<Botan::byte> der;
		der = Botan::DER_Encoder()
			.encode(repr, Botan::OCTET_STRING)
			.get_contents();
		point.resize(der.size());
		memcpy(&point[0], &der[0], der.size());
	}
	catch (...)
	{
		ERROR_MSG("Can't convert from EC point");
	}
	return point;
}

// Convert a ByteString to a Botan EC point
Botan::PointGFp BotanUtil::byteString2ECPoint(const ByteString& byteString, const Botan::EC_Group& ecGroup)
{
	Botan::SecureVector<Botan::byte> repr;
	Botan::BER_Decoder(byteString.const_byte_str(), byteString.size())
		.decode(repr, Botan::OCTET_STRING)
		.verify_end();
	return Botan::OS2ECP(&repr[0], repr.size(), ecGroup.get_curve());
}
#endif
