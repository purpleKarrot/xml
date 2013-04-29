/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef XML_READER_HPP
#define XML_READER_HPP

#include <string>
#include <memory>
#include <boost/optional.hpp>

namespace xml
{

enum class token
  {
  none,
  pi,
  element,
  end_element,
  text,
  cdata,
  comment
  };

class reader
  {
  public:
    reader(std::string const& filename);
    ~reader();

    bool read();

    xml::token token() const;

    std::string name() const;

    std::string namespace_uri() const;

    std::string attribute(
        std::string const& name,
        std::string const& namespace_uri) const;

    boost::optional<std::string> optional_attribute(
        std::string const& name,
        std::string const& namespace_uri) const;

    bool start_element();

    void end_element();

    std::string read_content();

  private:
    struct implementation;
    std::unique_ptr<implementation> self;
  };

} // namespace xml

#endif /* XML_READER_HPP */
