// Copyright (c) 2014, Cornell University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of HyperDex nor the names of its contributors may be
//       used to endorse or promote products derived from this software without
//       specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// e
#include <e/endian.h>
#include <e/safe_math.h>

// HyperDex
#include "cityhash/city.h"
#include "common/datatype_timestamp.h"

using hyperdex::datatype_info;
using hyperdex::datatype_timestamp;

namespace
{

int64_t
unpack(const e::slice& value)
{
    assert(value.size() == sizeof(int64_t) || value.empty());

    if (value.empty())
    {
      return 0;
    }

    int64_t timestamp;
    e::unpack64le(value.data(), &timestamp);
    return timestamp;
}

}

datatype_timestamp :: datatype_timestamp(hyperdatatype t)
    : m_type(t)
{
    assert(CONTAINER_TYPE(m_type) == HYPERDATATYPE_TIMESTAMP_GENERIC);
}

datatype_timestamp :: ~datatype_timestamp() throw ()
{
}

hyperdatatype
datatype_timestamp :: datatype() const
{
    return m_type;

}

bool
datatype_timestamp :: validate(const e::slice& value) const
{
    return value.size() == sizeof(int64_t) || value.empty();
}

bool
datatype_timestamp :: check_args(const funcall& func) const
{
    return func.name == FUNC_SET && func.arg1_datatype == this->datatype() && validate(func.arg1);
}

uint8_t*
datatype_timestamp  ::  apply(const e::slice& old_value,
                              const funcall* funcs, size_t funcs_sz,
                              uint8_t* writeto)
{
    int64_t timestamp = unpack(old_value);

    for(size_t i = 0; i < funcs_sz; i++ )
    {
        const funcall* func = funcs + i;
        assert(func->name == FUNC_SET);
        timestamp = unpack(func->arg1);
    }

    return e::pack64le(timestamp, writeto);
}

bool
datatype_timestamp::hashable() const
{
    return true;
}

int64_t  lookup_interesting[] = { 1, 60, 3600, 24*60*60, 7*24*60*60, 30 *7 * 24*60*60};

uint64_t
datatype_timestamp :: hash(const e::slice& value)
{
    int64_t interesting_bits;
    int64_t extra;
    int64_t timestamp = unpack(value);
    interesting_bits = lookup_interesting[(this->m_type & 0x7) - 1];
    extra = timestamp % interesting_bits;
    timestamp /= interesting_bits;
    return CityHash64((const char*)&timestamp, sizeof(int64_t)) + extra;
}

bool
datatype_timestamp :: indexable() const
{
    return true;
}

bool
datatype_timestamp :: containable() const
{
    return true;
}

bool
datatype_timestamp :: step(const uint8_t** ptr,
                           const uint8_t* end,
                           e::slice* elem)
{
    if (static_cast<size_t>(end - *ptr) < sizeof(int64_t))
    {
        return false;
    }

    *elem = e::slice(*ptr, sizeof(int64_t));
    *ptr += sizeof(int64_t);
    return true;
}

uint8_t*
datatype_timestamp :: write(uint8_t* writeto,
                            const e::slice& elem)
{
    memmove(writeto, elem.data(), elem.size());
    return writeto + elem.size();
}

bool
datatype_timestamp :: comparable() const
{
    return true;
}

static int
compare(const e::slice& lhs,
        const e::slice& rhs)
{
    int64_t lhsnum = unpack(lhs);
    int64_t rhsnum = unpack(rhs);

    if (lhsnum < rhsnum)
    {
        return -1;
    }
    if (lhsnum > rhsnum)
    {
        return 1;
    }

    return 0;
}

int
datatype_timestamp :: compare(const e::slice& lhs, const e::slice& rhs)
{
    return ::compare(lhs, rhs);
}

static bool
compare_less(const e::slice& lhs,
             const e::slice& rhs)
{
    return compare(lhs, rhs) < 0;
}

datatype_info::compares_less
datatype_timestamp :: compare_less()
{
    return &::compare_less;
}
