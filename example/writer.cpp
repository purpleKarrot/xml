/*
 * Copyright (C) 2012 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <fstream>
#include <xml/writer.hpp>

static std::string foo = "http://example.com/2013/foo";
static std::string bar = "http://example.com/2013/bar";

int main(int argc, char* argv[])
  {
  std::ofstream out("out.xml");
  xml::writer writer(out);

  writer.start_document("1.0", true);
  writer.start_element("foo", foo);
  writer.write_attribute("name", foo, "FooBar");
  writer.write_attribute("href", foo, "http://localhorst");
  writer.write_namespace(foo);
  writer.start_element("bar", bar);
  writer.write_namespace(bar, "bb");
  writer.write_element("cow", foo, "Something \"Foo\" escaped");
  writer.start_element("foo", foo);
  writer.write_attribute("bar", foo, "foobar");
  writer.end_document();

  return 0;
  }
