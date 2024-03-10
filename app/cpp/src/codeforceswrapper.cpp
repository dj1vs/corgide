#include "codeforceswrapper.hpp"

#include <locale>
#include <codecvt>

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>

#include <libxml/HTMLparser.h>

QString xmlCharToQString(const xmlChar *xmlString)
{    
    if (!xmlString) { return ""; } //provided string was null
    try
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
        return QString::fromStdWString(conv.from_bytes((const char*)xmlString));
    }
    catch(const std::range_error& e)
    {
        return ""; //wstring_convert failed
    }
}

void printNode(xmlNodePtr node, int depth) {
    xmlNodePtr curNode = NULL;

    for (curNode = node; curNode; curNode = curNode->next) {
        if (curNode->type == XML_ELEMENT_NODE) {
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            printf("<%s>", curNode->name);
            if (curNode->children) {
                printf("\n");
                printNode(curNode->children, depth + 1);
                for (int i = 0; i < depth; i++) {
                    printf("  ");
                }
            }
            printf("</%s>\n", curNode->name);
        } else if (curNode->type == XML_TEXT_NODE) {
            printf("%s\n", curNode->content);
        }
    }
}

xmlNodePtr find_tag(xmlNodePtr node, const QString &tag, const QString &tag_class = "") {
    if (node == nullptr) {
        return nullptr;
    }

    xmlNodePtr tag_ptr = nullptr;

    if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*)(tag.toStdString().c_str())) == 0) {
        if (tag_class == "") {
            return tag_ptr;
        } else {
            xmlChar* class_attr = xmlGetProp(node, (const xmlChar*)"class");

            if (class_attr != nullptr) {
                const QString class_attr_str = xmlCharToQString(class_attr);
                if (class_attr_str == tag_class) {
                    return tag_ptr;
                }
            }

            xmlFree(class_attr);
        }
        
    }
 
    for (xmlNodePtr child = node->children; child != nullptr; child = child->next) {
        tag_ptr = find_tag(child, tag, tag_class);
        if (tag_ptr != nullptr) {
            break;
        }
    }

    return tag_ptr;
}

QString get_tag_contents(xmlNodePtr node) {
    QString contents;
    xmlChar* content = xmlNodeListGetString(node->doc, node->children, 5);
    contents =xmlCharToQString(content);
    xmlFree(content);
    return contents;
}

QStringList find_tags_contents(xmlNodePtr node, const QString &tag, const QString &tag_class = "") {
    QStringList contents;
    if (node == nullptr) {
        return {};
    }

    if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*)(tag.toStdString().c_str())) == 0) {
        if (tag_class == "") {
            xmlChar* content = xmlNodeListGetString(node->doc, node->children, 1);
            contents << xmlCharToQString(content);
            xmlFree(content);
        } else {
            xmlChar* class_attr = xmlGetProp(node, (const xmlChar*)"class");

            if (class_attr != nullptr) {
                const QString class_attr_str = xmlCharToQString(class_attr);
                if (class_attr_str == tag_class) {
                    xmlChar* content = xmlNodeListGetString(node->doc, node->children, 5);
                    contents << xmlCharToQString(content);
                    xmlFree(content);
                }
            }

            xmlFree(class_attr);
        }
        
    }
 
    for (xmlNodePtr child = node->children; child != nullptr; child = child->next) {
        contents.append(find_tags_contents(child, tag, tag_class));
    }

    return contents;
}

CodeforcesWrapper::CodeforcesWrapper() {
    network_access_manager = new QNetworkAccessManager();

    QObject::connect(network_access_manager, &QNetworkAccessManager::finished,
         [=](QNetworkReply *reply) {
            if (reply->error()) {
                qDebug() << reply->errorString();
                return;
            }

            QString answer = reply->readAll();

            htmlDocPtr doc = htmlReadDoc((const xmlChar*)(answer.toStdString().c_str()), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

            if (doc == NULL) {
                qDebug() << "Failed to parse HTML";
                return;
            }

            const QString title = find_tags_contents(xmlDocGetRootElement(doc), "div", "time-limit")[0];
            const QString time_limit = find_tags_contents(xmlDocGetRootElement(doc), "div", "time-limit")[0];
            const QString memory_limit = find_tags_contents(xmlDocGetRootElement(doc), "div", "memory-limit")[0];

            emit problem_parsed({
                title, time_limit, memory_limit
            });

            xmlFreeDoc(doc);

        }
    );


    QUrl url("https://codeforces.com/problemset/problem/158/A");

    QNetworkRequest request(url);

    network_access_manager->get(request); 

   

}

CodeforcesWrapper::~CodeforcesWrapper() {

    delete network_access_manager;
}