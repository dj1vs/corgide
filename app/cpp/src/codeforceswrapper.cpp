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
    QStringList contents;
    if (node == nullptr) {
        return {};
    }

    if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*)(tag.toStdString().c_str())) == 0) {
        if (tag_class == "") {
            return node;
        } else {
            xmlChar* class_attr = xmlGetProp(node, (const xmlChar*)"class");

            if (class_attr != nullptr) {
                const QString class_attr_str = xmlCharToQString(class_attr);
                if (class_attr_str == tag_class) {
                    return node;
                }
            }

            xmlFree(class_attr);
        }
        
    }
 
    for (xmlNodePtr child = node->children; child != nullptr; child = child->next) {
        auto tag_ptr = find_tag(child, tag, tag_class);
        if (tag_ptr != nullptr) {
            return tag_ptr;
        }
    }

    return nullptr;
}

QString get_tag_contents(xmlNodePtr node, bool filter = true) {
    if (node == nullptr) {
        return "";
    }
    QString contents;

    xmlChar* content;
    if (filter) {
        content = xmlNodeListGetString(node->doc, node->children, 0);
    } else {
        content = xmlNodeGetContent(node);
    }
    contents =xmlCharToQString(content);
    xmlFree(content);
    return contents;
}

QString get_tag_contents_r(xmlNodePtr node, bool filter = true) {
    QString contents;
    if (filter) {
        contents = get_tag_contents(node);

        for (xmlNodePtr child = node->children; child != nullptr; child = child->next) {
            contents += get_tag_contents_r(child);
        }
    } else {
        contents = get_tag_contents(node, filter);
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

            const QString title = get_tag_contents(find_tag(xmlDocGetRootElement(doc), "div", "title"));
            const QString time_limit = get_tag_contents(find_tag(xmlDocGetRootElement(doc), "div", "time-limit"));
            const QString memory_limit = get_tag_contents(find_tag(xmlDocGetRootElement(doc), "div", "memory-limit"));
            const QString statement = get_tag_contents_r(find_tag(xmlDocGetRootElement(doc), "div", "header")->next, false);

            emit problem_parsed({
                title, time_limit, memory_limit
            });

            qDebug() << title << time_limit << memory_limit << statement;

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