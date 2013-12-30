//
// Created by David Seery on 27/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __xml_serializable_H_
#define __xml_serializable_H_


#include <list>
#include <string>

#include "boost/lexical_cast.hpp"

#include "dbxml/DbXml.hpp"

#include "transport/db-xml/db_xml.h"


namespace transport
  {

    class xml_serializable
      {
      public:
        virtual ~xml_serializable()
          {
          }

        // SERIALIZATION METHOD

        virtual void serialize_xml(DbXml::XmlEventWriter& writer) = 0;

        // STANDARD SERIALIZATION METHODS

      protected:

        // begin XML node (its type is recorded on the stack), with arbitrary number of attributes
        template <typename... attrs>
        void begin_node(DbXml::XmlEventWriter& writer, const std::string& name, bool empty, attrs... attributes);

        // end the current XML node
        void end_node(DbXml::XmlEventWriter& writer, const std::string& name);

        // write a node with a single value type
        template <typename T, typename... attrs>
        void write_value_node(DbXml::XmlEventWriter& writer, const std::string& name, const T& value, attrs... attributes);

        // write attributes
        void write_attributes(DbXml::XmlEventWriter& writer);

        template <typename T, typename... attrs>
        void write_attributes(DbXml::XmlEventWriter& writer, const std::string& attr_name, const T& attr_val, attrs... other_attributes);

        // INTERNAL DATA

      private:
        std::list<std::string> xml_stack;
      };


    template <typename... attrs>
    void xml_serializable::begin_node(DbXml::XmlEventWriter& writer, const std::string& name, bool empty, attrs... attributes)
      {
        this->xml_stack.push_front(name);

        writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(name.c_str()), nullptr, nullptr, sizeof...(attributes), empty);
        this->write_attributes(writer, std::forward<attrs>(attributes)...);
      }


    void xml_serializable::write_attributes(DbXml::XmlEventWriter& writer)
      {
      }


    template <typename T, typename... attrs>
    void xml_serializable::write_attributes(DbXml::XmlEventWriter& writer, const std::string& attr_name, const T& attr_val, attrs... other_attributes)
      {
        std::string value = boost::lexical_cast<std::string>(attr_val);

        writer.writeAttribute(__CPP_TRANSPORT_DBXML_STRING(attr_name.c_str()), nullptr, nullptr,
                              __CPP_TRANSPORT_DBXML_STRING(value.c_str()), true);
        this->write_attributes(writer, std::forward<attrs>(other_attributes)...);
      }


    void xml_serializable::end_node(DbXml::XmlEventWriter& writer, const std::string& name)
      {
        if(this->xml_stack.size() > 0 && this->xml_stack.front() == name)
          {
            this->xml_stack.pop_front();

            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(name.c_str()), nullptr, nullptr);
          }
        else
          {
            throw std::invalid_argument(__CPP_TRANSPORT_XML_MISMATCH);
          }
      }


    template <typename T, typename... attrs>
    void xml_serializable::write_value_node(DbXml::XmlEventWriter& writer, const std::string& name, const T& value, attrs... attributes)
      {
        this->begin_node(writer, name, false, std::forward<attrs>(attributes)...);

        std::string val = boost::lexical_cast<std::string>(value);
        writer.writeText(DbXml::XmlEventReader::XmlEventType::Characters, __CPP_TRANSPORT_DBXML_STRING(val.c_str()), val.length());

        this->end_node(writer, name);
      }

  }


#endif //__xml_serializable_H_