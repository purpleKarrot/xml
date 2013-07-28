/*
 * Copyright (C) 2012 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <fstream>
#include <xml/reader.hpp>
#include <boost/detail/lightweight_test.hpp>

static std::string foo = "http://example.com/2013/foo";
static std::string bar = "http://example.com/2013/bar";

int simple(int argc, char* argv[])
{
    std::vector<char> buffer;

    std::ifstream input(argv[1]);
    input.unsetf(std::ios::skipws);
    input.seekg(0, std::ios::end);
    std::size_t size = static_cast<std::size_t>(input.tellg());
    input.seekg(0);
    buffer.resize(size + 1);
    input.read(&buffer[0], static_cast<std::streamsize>(size));
    buffer[size] = 0;

    xml::reader reader(&buffer[0], &buffer[size]);

    BOOST_TEST(reader.start_element());
    BOOST_TEST_EQ(reader.name(), "foo");
    BOOST_TEST_EQ(reader.namespace_uri(), foo);

    BOOST_TEST(reader.start_element());
    BOOST_TEST_EQ(reader.name(), "bar");
    BOOST_TEST_EQ(reader.namespace_uri(), bar);

    BOOST_TEST(reader.start_element());
    BOOST_TEST_EQ(reader.name(), "cow");
    BOOST_TEST_EQ(reader.namespace_uri(), foo);
//  BOOST_TEST_EQ(reader.read_content(), "Something \"Foo\" escaped");
    reader.end_element();

    BOOST_TEST(reader.start_element());
    BOOST_TEST_EQ(reader.name(), "foo");
    BOOST_TEST_EQ(reader.namespace_uri(), foo);
    BOOST_TEST_EQ(reader.attribute("bar", foo), "foobar");
    reader.end_element();

    reader.end_element();
    reader.end_element();

    return boost::report_errors();
}
