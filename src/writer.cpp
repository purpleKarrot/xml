/*
 * Copyright (C) 2012 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <xml/writer.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <fstream>
#include <cassert>
#include <limits>
#include <vector>

namespace xml
{
namespace
{

struct Element
  {
  std::string name;
  std::string namespace_uri;
  std::size_t previous_mappings;
  };

struct Attribute
  {
  std::string name;
  std::string namespace_uri;
  std::string value;
  };

struct Mapping
  {
  std::string namespace_uri;
  std::string prefix;
  };

} // namespace

struct writer::implementation
  {
  implementation(std::ostream& output)
    : output_(output)
    , element_open(false)
    , indent_(std::numeric_limits<std::size_t>::max())
    {
    }
  void write_content(const std::string& content)
    {
    for (char c : content)
      {
      switch (c)
        {
        case '<': output_ << "&lt;";   break;
        case '>': output_ << "&gt;";   break;
        case '&': output_ << "&amp;";  break;
        case '"': output_ << "&quot;"; break;
        default : output_ << c;        break;
        }
      }
    }
  void prefix(const std::string& namespace_uri)
    {
    for (const Mapping& mapping : ns_mappings)
      {
      if (mapping.namespace_uri == namespace_uri)
        {
        if (!mapping.prefix.empty())
          {
          output_ << mapping.prefix << ':';
          }
        return;
        }
      }
    std::cout << "warning: namespace not registered: " << namespace_uri << std::endl;
    }
  void finalize_start()
    {
    prefix(elements.back().namespace_uri);
    output_ << elements.back().name;
    for (const Attribute& attr : attributes)
      {
      output_ << ' ' ;
      prefix(attr.namespace_uri);
      output_ << attr.name  << '=' << '"' << attr.value << '"';
      }
    for (std::size_t i = elements.back().previous_mappings; i < ns_mappings.size(); ++i)
      {
      output_ << ' ' << "xmlns";
      if (!ns_mappings[i].prefix.empty())
        {
        output_ << ':' << ns_mappings[i].prefix;
        }
      output_ << '=' << '"' << ns_mappings[i].namespace_uri << '"';
      }
    }
  std::ostream& output_;
  bool element_open;
  std::size_t indent_;
  std::vector<Attribute> attributes;
  std::vector<Mapping> ns_mappings;
  std::vector<Element> elements;
  };

writer::writer(std::ostream& output) :
    self(new implementation(output))
  {
  }

writer::~writer() = default;

void writer::start_document(const std::string& version, bool standalone)
  {
  self->output_
    << "<?xml version=\"" << version
    << "\" encoding=\"" << "utf-8"
    << "\" standalone=\"" << (standalone ? "yes" : "no") << "\" ?>"
    ;
  }

void writer::end_document()
  {
  while (!self->elements.empty())
    {
    end_element();
    }
  self->output_ << '\n';
  }

void writer::start_element(
    const std::string& name,
    const std::string& namespace_uri)
  {
  if (self->element_open)
    {
    self->finalize_start();
    self->output_ << ">";
    }
  if (self->elements.size() < self->indent_)
    {
    self->output_ << '\n';
    for (size_t i = self->elements.size(); i > 0; --i)
      {
      self->output_ << "  ";
      }
    }
  self->output_ << "<";
  self->element_open = true;
  self->attributes.clear();
  self->elements.push_back({name, namespace_uri, self->ns_mappings.size()});
  }

void writer::end_element()
  {
  if (!self->elements.empty())
    {
    if (self->element_open)
      {
      self->finalize_start();
      self->output_ << "/>";
      }
    else
      {
      if (self->elements.size() < self->indent_)
        {
        self->output_ << '\n';
        for (size_t i = self->elements.size() - 1; i > 0; --i)
          {
          self->output_ << "  ";
          }
        }
      else if (self->elements.size() == self->indent_)
        {
        self->indent_ = std::numeric_limits < std::size_t > ::max();
        }
      self->output_ << "</";
      const Element& element = self->elements.back();
      self->prefix(element.namespace_uri);
      self->output_ << element.name << ">";
      }
    self->ns_mappings.resize(self->elements.back().previous_mappings);
    self->elements.pop_back();
    }
  self->element_open = false;
  }

void writer::write_element(
    const std::string& name,
    const std::string& namespace_uri)
  {
  start_element(name, namespace_uri);
  end_element();
  }

void writer::write_element(
    const std::string& name,
    const std::string& namespace_uri,
    const std::string& value)
  {
  start_element(name, namespace_uri);
  write_content(value);
  end_element();
  }

void writer::write_content(const std::string& content)
  {
  if (self->element_open)
    {
    self->finalize_start();
    self->output_ << ">";
    self->element_open = false;
    }
  self->indent_ = std::min(self->indent_, self->elements.size());
  self->write_content(content);
  }

void writer::write_attribute(
    const std::string& name,
    const std::string& namespace_uri,
    const std::string& value)
  {
  self->attributes.push_back({name, namespace_uri, value});
  }

void writer::write_namespace(
    const std::string& namespace_uri,
    const std::string& prefix)
  {
  self->ns_mappings.push_back({namespace_uri, prefix});
  }

} // namespace xml
