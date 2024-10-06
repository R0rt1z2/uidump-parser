/*
 * Copyright 2024 Roger Ortiz (R0r1z2)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdarg>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <string>

#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

int debug = 0;

void dprint(const char *format, ...) {
    if (!debug)
        return;
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void print_help() {
    std::cout << "Usage: uidump-parser --file <xml_file> [OPTIONS]\n";
    std::cout << "Options:\n";
    std::cout << "  --file, -f <xml_file>            : Path to the XML file to "
                 "parse (required)\n";
    std::cout << "  --resource-id, -r <id>           : Search for a node with "
                 "the given resource-id\n";
    std::cout << "  --class, -c <class_name>         : Search for a node with "
                 "the given class name\n";
    std::cout << "  --text, -t <text_value>          : Search for a node with "
                 "the given text value\n";
    std::cout << "  --filter-attribute, -F <attr=val>: Filter by any attribute "
                 "dynamically (e.g., package, content-desc)\n";
    std::cout << "  --print-only, -p <attribute>     : Print only the "
                 "specified attribute for matched nodes\n";
    std::cout << "  --bounds, -b                     : Print bounds for "
                 "matched nodes\n";
    std::cout << "  --debug, -d                      : Enable debug mode for "
                 "verbose output\n";
    std::cout << "  --help, -h                       : Show this help message "
                 "and exit\n";
    std::cout << "\nExamples:\n";
    std::cout << "  ./uidump-parser --file dump.xml --resource-id com.example "
                 "--print-only bounds --debug\n";
    std::cout << "  ./uidump-parser --file dump.xml --resource-id com.example "
                 "--filter-attribute text=Grindr --print-only bounds\n";
    std::cout << "  ./uidump-parser --file dump.xml --class "
                 "android.widget.TextView --filter-attribute enabled=true\n";
    std::cout << "  ./uidump-parser --file dump.xml --text Instagram "
                 "--filter-attribute package=com.example --bounds\n";
}

void print_node_attributes(const XMLElement *element, const char *only_print) {
    dprint("Processing node: %s\n", element->Name());

    if (only_print && strlen(only_print) > 0) {
        const char *attr = element->Attribute(only_print);
        if (attr) {
            std::cout << only_print << ": " << attr << "\n";
        } else {
            std::cout << "Attribute '" << only_print << "' not found on node "
                      << element->Name() << "\n";
        }
        return;
    }

    std::cout << "Node: " << element->Name() << "\n";
    bool hasAttributes = false;

    for (const XMLAttribute *attr = element->FirstAttribute(); attr;
         attr = attr->Next()) {
        hasAttributes = true;
        std::cout << "  " << attr->Name() << ": " << attr->Value() << "\n";
    }

    if (!hasAttributes) {
        std::cout << "  No attributes found for node: " << element->Name()
                  << "\n";
    }

    std::cout << "\n";
}

bool node_matches_additional_filter(const XMLElement *element,
                                    const std::string &filter_attribute,
                                    const std::string &filter_value) {
    if (!filter_attribute.empty() && !filter_value.empty()) {
        const char *attr_value = element->Attribute(filter_attribute.c_str());
        return attr_value && filter_value == attr_value;
    }
    return true;
}

void find_node_by_resource_id(const XMLElement *element,
                              const std::string &resource_id,
                              const char *only_print,
                              const std::string &filter_attribute = "",
                              const std::string &filter_value = "") {
    const char *res_id_attr = element->Attribute("resource-id");

    if (res_id_attr && resource_id == res_id_attr) {
        if (node_matches_additional_filter(element, filter_attribute,
                                           filter_value)) {
            print_node_attributes(element, only_print);
        }
    }

    for (const XMLElement *child = element->FirstChildElement();
         child != nullptr; child = child->NextSiblingElement()) {
        find_node_by_resource_id(child, resource_id, only_print,
                                 filter_attribute, filter_value);
    }
}

void find_node_by_class(const XMLElement *element,
                        const std::string &class_name, const char *only_print,
                        const std::string &filter_attribute = "",
                        const std::string &filter_value = "") {
    const char *class_attr = element->Attribute("class");
    if (class_attr && class_name == class_attr) {
        if (node_matches_additional_filter(element, filter_attribute,
                                           filter_value)) {
            print_node_attributes(element, only_print);
        }
    }

    for (const XMLElement *child = element->FirstChildElement();
         child != nullptr; child = child->NextSiblingElement()) {
        find_node_by_class(child, class_name, only_print, filter_attribute,
                           filter_value);
    }
}

void find_node_by_text(const XMLElement *element, const std::string &text_value,
                       const char *only_print,
                       const std::string &filter_attribute = "",
                       const std::string &filter_value = "") {
    const char *text_attr = element->Attribute("text");
    if (text_attr && text_value == text_attr) {
        if (node_matches_additional_filter(element, filter_attribute,
                                           filter_value)) {
            print_node_attributes(element, only_print);
        }
    }

    for (const XMLElement *child = element->FirstChildElement();
         child != nullptr; child = child->NextSiblingElement()) {
        find_node_by_text(child, text_value, only_print, filter_attribute,
                          filter_value);
    }
}

void find_node_by_filter(const XMLElement *element,
                         const std::string &filter_attribute,
                         const std::string &filter_value,
                         const char *only_print) {
    for (const XMLElement *child = element; child != nullptr;
         child = child->NextSiblingElement()) {
        if (node_matches_additional_filter(child, filter_attribute,
                                           filter_value)) {
            print_node_attributes(child, only_print);
        }
        find_node_by_filter(child->FirstChildElement(), filter_attribute,
                            filter_value, only_print);
    }
}

int main(int argc, char **argv) {
    std::string xml_file;
    std::string filter_attribute, filter_value, only_print;
    std::string resource_id, class_name, text_value;

    static struct option long_options[] = {
        {"file", required_argument, 0, 'f'},
        {"resource-id", required_argument, 0, 'r'},
        {"class", required_argument, 0, 'c'},
        {"text", required_argument, 0, 't'},
        {"filter-attribute", required_argument, 0, 'F'},
        {"print-only", required_argument, 0, 'p'},
        {"debug", no_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "f:r:c:t:F:p:dh", long_options,
                              NULL)) != -1) {
        switch (opt) {
        case 'f':
            xml_file = optarg;
            break;
        case 'r':
            resource_id = optarg;
            break;
        case 'c':
            class_name = optarg;
            break;
        case 't':
            text_value = optarg;
            break;
        case 'F': {
            std::string filter(optarg);
            auto equal_pos = filter.find('=');
            if (equal_pos != std::string::npos) {
                filter_attribute = filter.substr(0, equal_pos);
                filter_value = filter.substr(equal_pos + 1);
            }
            break;
        }
        case 'p':
            only_print = optarg;
            break;
        case 'd':
            debug = 1;
            break;
        case 'h':
            print_help();
            return 0;
        default:
            std::cerr << "Usage: " << argv[0]
                      << " --file <xml_file> [--resource-id <id>] [--class "
                         "<class_name>] "
                      << "[--text <text_value>] [--filter-attribute "
                         "<attr=value>] [--print-only <attribute>] "
                      << "[--debug] [--help]\n";
            exit(EXIT_FAILURE);
        }
    }

    if (xml_file.empty()) {
        std::cerr << "Error: XML file is required. Use --file <xml_file>\n";
        exit(EXIT_FAILURE);
    }

    dprint("Opening XML file: %s\n", xml_file.c_str());

    XMLDocument doc;
    if (doc.LoadFile(xml_file.c_str()) != XML_SUCCESS) {
        std::cerr << "Error: could not parse file " << xml_file << "\n";
        return 1;
    }

    dprint("Successfully loaded XML file\n");

    const XMLElement *root_element = doc.RootElement();

    if (!resource_id.empty()) {
        find_node_by_resource_id(root_element, resource_id, only_print.c_str(),
                                 filter_attribute, filter_value);
    } else if (!class_name.empty()) {
        find_node_by_class(root_element, class_name, only_print.c_str(),
                           filter_attribute, filter_value);
    } else if (!text_value.empty()) {
        find_node_by_text(root_element, text_value, only_print.c_str(),
                          filter_attribute, filter_value);
    } else if (!filter_attribute.empty() && !filter_value.empty()) {
        find_node_by_filter(root_element, filter_attribute, filter_value,
                            only_print.c_str());
    } else {
        std::cerr << "No search criteria specified. Use --resource-id, "
                     "--class, --text, or --filter-attribute <attr=value>.\n";
    }

    return 0;
}
