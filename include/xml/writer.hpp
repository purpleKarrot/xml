/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef XML_WRITER_HPP
#define XML_WRITER_HPP

#include <string>
#include <memory>
#include <iosfwd>

namespace xml
{

class writer
  {
  public:
    writer(std::ostream& output);
    ~writer();

    void start_document(const std::string& version, bool standalone);

    void end_document();

    void start_element(
        const std::string& name,
        const std::string& namespace_uri);

    void end_element();

    void write_element(
        const std::string& name,
        const std::string& namespace_uri,
        const std::string& value);

    void write_element(
        const std::string& name,
        const std::string& namespace_uri);

    void write_attribute(
        const std::string& name,
        const std::string& namespace_uri,
        const std::string& value);

    void write_namespace(
        const std::string& namespace_uri,
        const std::string& prefix = std::string());

    void write_content(const std::string& content);

  private:
    struct implementation;
    std::unique_ptr<implementation> self;
  };

} // namespace xml

#endif /* XML_WRITER_HPP */
