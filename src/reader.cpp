/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <xml/reader.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <fstream>
#include <sstream>
#include <vector>

namespace xml
{
namespace
{

struct Name
  {
  std::string prefix;
  std::string local;
  std::string namespace_uri;
  };

struct Tag
  {
  Name name;
  std::size_t previous_mappings;
  };

struct Attribute
  {
  Name name;
  std::string value;
  };

struct Mapping
  {
  std::string prefix;
  std::string namespace_uri;
  };

} // namespace

struct reader::implementation
  {
  public:
    implementation(std::string const& filename) :
        token_(xml::token::none), is_empty_element(false)
      {
      std::ifstream input(filename);
      input.unsetf(std::ios::skipws);
      input.seekg(0, std::ios::end);
      std::size_t size = static_cast<std::size_t>(input.tellg());
      input.seekg(0);
      buffer.resize(size + 1);
      cursor = marker = Iterator(buffer.begin());
      input.read(&buffer[0], static_cast<std::streamsize>(size));
      buffer[size] = 0;
      }
    bool read();
  public:
    void push_tag();
    void pop_tag();
    std::size_t push_namespaces();
    void pop_namespaces(std::size_t n);
    void lookup_namespace(Name& name);
    void throw_error(std::string const& message = std::string()) const;
  public:
    void parse_name(Name& name);
    void parse_attribute(Attribute& attribute);
    void parse_pi();
    void parse_comment();
    void parse_element();
    void parse_end_element();
    bool parse_text();
  public:
    std::vector<char> buffer;
    typedef std::vector<char>::iterator VectorIterator;
    typedef boost::spirit::line_pos_iterator<VectorIterator> Iterator;
    Iterator cursor;
    Iterator marker;
    xml::token token_;
    Name current_name;
    std::vector<Attribute> attributes;
    std::vector<Mapping> ns_mappings;
    std::vector<Tag> open_tags;
    bool is_empty_element;
  };

#include "re2c.hpp"

void reader::implementation::lookup_namespace(Name& name)
  {
  for (const Mapping& mapping : boost::adaptors::reverse(ns_mappings))
    {
    if (name.prefix == mapping.prefix)
      {
      name.namespace_uri = mapping.namespace_uri;
      return;
      }
    }
  name.namespace_uri.clear();
  }

std::size_t reader::implementation::push_namespaces()
  {
  Mapping mapping;
  std::size_t previous_mappings = ns_mappings.size();
  for (const Attribute& attr : attributes)
    {
    if (attr.name.prefix.empty() && attr.name.local == "xmlns")
      {
      mapping.prefix.clear();
      mapping.namespace_uri = attr.value;
      ns_mappings.push_back(mapping);
      }
    else if (attr.name.prefix == "xmlns")
      {
      mapping.prefix = attr.name.local;
      mapping.namespace_uri = attr.value;
      ns_mappings.push_back(mapping);
      }
    }
  lookup_namespace(current_name);
  for (Attribute& attr : attributes)
    {
    lookup_namespace(attr.name);
    }
  return previous_mappings;
  }

void reader::implementation::pop_namespaces(std::size_t n)
  {
  ns_mappings.resize(n);
  }

void reader::implementation::push_tag()
  {
  Tag tag;
  tag.previous_mappings = push_namespaces();
  tag.name = current_name;
  open_tags.push_back(tag);
  }

void reader::implementation::pop_tag()
  {
  const Tag& tag = open_tags.back();
  pop_namespaces(tag.previous_mappings);
  current_name = tag.name;
  open_tags.pop_back();
  }

void reader::implementation::throw_error(std::string const& message) const
  {
  auto begin = marker.base();
  for (; begin != buffer.begin(); --begin)
    {
    if (*begin == '\n' || *begin == '\n')
      {
      break;
      }
    }
  auto end = marker.base();
  for (; end != buffer.end(); ++end)
    {
    if (*end == '\n' || *end == '\n')
      {
      break;
      }
    }
  std::stringstream error;
  error
    << "XML parse error"
    << " in line " << marker.position() << ".\n"
    << message << '\n'
    << std::string(begin, end) << '\n'
    << std::string(marker.base() - begin, ' ') << "^\n"
    ;
  throw std::runtime_error(error.str());
  }

/******************************************************************************/

//#include "xml_re2c.h"

reader::reader(std::string const& filename) :
    self(new implementation(filename))
  {
  }

reader::~reader() = default;

bool reader::read()
  {
  return self->read();
  }

token reader::token() const
  {
  return self->token_;
  }

std::string reader::name() const
  {
  return self->current_name.local;
  }

std::string reader::namespace_uri() const
  {
  return  self->current_name.namespace_uri;
  }

std::string reader::attribute(
    const std::string& namespace_uri,
    const std::string& name) const
  {
  if (auto attr = optional_attribute(name, namespace_uri))
    {
    return *attr;
    }
  std::stringstream error;
  error << "Required XML attribute '" << name << "'";
  if (!namespace_uri.empty())
    {
    error << " (namespace: '" << namespace_uri << "')";
    }
  error << " is missing in element '" << self->current_name.local << "'";
  if (!self->current_name.namespace_uri.empty())
    {
    error << " (namespace: '" << self->current_name.namespace_uri << "')";
    }
  throw std::runtime_error(error.str());
  }

boost::optional<std::string> reader::optional_attribute(
    const std::string& name,
    const std::string& namespace_uri) const
  {
  for (auto& attr : self->attributes)
    {
    if (attr.name.local == name && attr.name.namespace_uri == namespace_uri)
      {
      return attr.value;
      }
    }
  return boost::none;
  }

void reader::end_element()
  {
  if (self->is_empty_element)
    {
    self->is_empty_element = false;
    return;
    }
  std::size_t depth = 0;
  do
    {
    if ((self->token_ == xml::token::element) && (!self->is_empty_element))
      {
      ++depth;
      }
    else if (self->token_ == xml::token::end_element)
      {
      --depth;
      }
    }
  while (read() && depth > 0);
  }

bool reader::start_element()
  {
  if (!self->is_empty_element)
    {
    while (read())
      {
      if (self->token_ == xml::token::element)
        {
        return true;
        }
      if (self->token_ == xml::token::end_element)
        {
        return false;
        }
      }
    }
  return false;
  }

std::string reader::read_content()
  {
  if (self->is_empty_element)
    {
    self->is_empty_element = false;
    return std::string();
    }
  implementation::Iterator begin = self->cursor;
  implementation::Iterator end;
  std::size_t depth = 0;
  do
    {
    if ((self->token_ == xml::token::element) && (!self->is_empty_element))
      {
      ++depth;
      }
    else if (self->token_ == xml::token::end_element)
      {
      --depth;
      }
    if (depth > 0)
      {
      end = self->cursor;
      }
    }
  while (read() && depth > 0);
  return std::string(begin, end);
  }

} // namespace xml
