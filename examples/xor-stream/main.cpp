
// Copyright (c) 2010-2014 niXman (i dot nixman dog gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <iostream>
#include <string>

#include <yas/mem_streams.hpp>
#include <yas/text_oarchive.hpp>
#include <yas/text_iarchive.hpp>
#include <yas/serializers/std_types_serializers.hpp>

/***************************************************************************/

struct my_ostream: yas::mem_ostream {
	my_ostream(const char key)
		:yas::mem_ostream()
		,key(key)
	{}

	std::size_t write(const void *ptr, std::size_t size) {
		const char *sp = ((const char*)ptr);
		for ( std::size_t idx = 0; idx < size; ++idx ) {
			const char ch = *sp++ ^ key;
			yas::mem_ostream::write(&ch, sizeof(ch));
		}
		return size;
	}

private:
	const char key;
};

struct my_istream: yas::mem_istream {
	my_istream(const yas::intrusive_buffer &buf, const char key)
		:yas::mem_istream(buf)
		,key(key)
	{}

	std::size_t read(void *ptr, std::size_t size) {
		char *sp = ((char*)ptr);
		for ( std::size_t idx = 0; idx < size; ++idx ) {
			char ch = 0;
			yas::mem_istream::read(&ch, sizeof(ch));
			*sp++ = (ch ^ key);
		}
		return size;
	}

private:
	const char key;
};

/***************************************************************************/

int main() {
	const char key = 0x33;
	std::string src = "my mega super string", dst;
	my_ostream os(key);
	yas::text_oarchive<my_ostream> oa(os);
	oa & src;

	my_istream is(os.get_intrusive_buffer(), key);
	yas::text_iarchive<my_istream> ia(is);
	ia & dst;

	if ( src != dst )
		YAS_THROW_EXCEPTION(std::runtime_error, "bad value");
}

/***************************************************************************/