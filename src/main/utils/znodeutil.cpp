//
// Created by Lukas Valine on 4/14/22.
//


#include "utils/znodeutil.h"

ZNodeView *ZNodeUtil::nodesFromJson(json j, ZNodeView *parent) {
    if (parent == nullptr) {
        auto node = newNode(ZNodeDefStore::get().getJsonNodeType("object"));
        node->setText("root");
        auto objectNodes = nodesFromJson(j, node);
        return node;
    }
    for (auto& el : j.items()) {
        if (el.value().is_object()) {
            auto node = newNode(ZNodeDefStore::get().getJsonNodeType("object"));
            connectNodes(0, 0, node, parent);
            node->setText(el.key());
            auto objectNodes = nodesFromJson(el.value(), node);
        } else if (el.value().is_string()) {
            auto node = newNode(ZNodeDefStore::get().getJsonNodeType("string"));
            string value = el.value();
            node->setText(el.key() + "\n" + value);
            connectNodes(0, 0, node, parent);
        } else if (el.value().is_number_integer()) {
            auto node = newNode(ZNodeDefStore::get().getJsonNodeType("integer"));
            connectNodes(0, 0, node, parent);
            int value = el.value();
            node->setText(el.key() + "\n" + to_string(value));
        } else if (el.value().is_number_float()) {
            auto node = newNode(ZNodeDefStore::get().getJsonNodeType("float"));
            connectNodes(0, 0, node, parent);
            float value = el.value();
            node->setText(el.key() + "\n" + to_string(value));
        } else if (el.value().is_boolean()) {
            auto node = newNode(ZNodeDefStore::get().getJsonNodeType("boolean"));
            connectNodes(0, 0, node, parent);
            bool value = el.value();
            node->setText(el.key() + "\n" + to_string(value));
        } else if (el.value().is_array()) {
            auto node = newNode(ZNodeDefStore::get().getJsonNodeType("array"));
            connectNodes(0, 0, node, parent);
            node->setText(el.key() + "\n" + "size: " +  to_string(el.value().size()));
            float sizef = el.value().size();
            node->setBackgroundColor(vec4(tanh(sizef / 5.0f) + 0.1,0.1,0.1,1));
            auto objectNodes = nodesFromJson(el.value()[0], node);
        } else {
            auto node = newNode(ZNodeDefStore::get().getJsonNodeType("boolean"));
            connectNodes(0, 0, node, parent);parent = node;
            bool value = el.value();
            node->setBackgroundColor(vec4(0,1,0.1,1.0));
            node->setText(el.key() + "\n" + to_string(value));
        }
        //std::cout << "key: " << el.key() << ", value:" << el.value() << '\n';
    }

    return parent;
}

ZNodeView *ZNodeUtil::newNode(string typeString) {
    auto type = ZNodeDefStore::get().getMathNodeType(typeString);
    ZNodeView* node = nullptr;
    if (!mDeleteNodes.empty()) {
        while (node == nullptr && !mDeleteNodes.empty()) {
            node = mDeleteNodes.front();
            mDeleteNodes.pop();
            mDeleteNodesSet.erase(node);
        }

        if (node == nullptr) {
            node = new ZNodeView(type);
        } else {
            node->setType(type);

        }
    } else {
        node = new ZNodeView(type);
    }
    return node;
}

ZNodeUtil::ZNodeUtil() {
    vector<NodeType*> types = ZNodeDefStore::get().getAllNodeTypes();
    for (NodeType* type : types) {
        mTypes.insert({type->mName, type});
        mFunctions.insert(type->mName);
    }
    if (mTypes.count("pow") > 0) {
        mTypes.insert({"^", mTypes.at("pow")});
    }
}

