#include "core/component/node.h"


void Node2D::translate(float dx, float dy) {
    _transform.position += glm::vec2(dx, dy);
}

void Node2D::rotate(float degrees) {
    _transform.rotation = glm::radians(degrees);
}

void Node2D::scale(float sx, float sy) {
    _transform.scale *= glm::vec2(sx, sy);
}

void Node2D::set_z_index(int index) {
    if (_z_index == index) return; // No change needed

    _z_index = index;

    // If this node has a parent, re-sort its position in the parent's draw list
    if (_parent) {
        auto& parent_draw_list = _parent->_draw_list;

        // Remove this node from current position
        auto it = std::find(parent_draw_list.begin(), parent_draw_list.end(), this);
        if (it != parent_draw_list.end()) {
            parent_draw_list.erase(it);
        }

        // Insert at correct position based on new z-index
        auto insert_pos = std::lower_bound(parent_draw_list.begin(), parent_draw_list.end(), this,
            [](const Node2D* a, const Node2D* b) { return a->_z_index < b->_z_index; });

        parent_draw_list.insert(insert_pos, this);
    }
}


int Node2D::get_z_index() const {
    return _z_index;
}

int Node2D::get_effective_z_index() const {
    if (_parent) {
        return _parent->get_effective_z_index() + _z_index;
    }
    return _z_index;
}

void Node2D::print_tree(const int indent) const {
    for (int i = 0; i < indent; ++i) {
        printf(" ");
    }

    printf("%s (zIndex: %d, Position: %.2f, %.2f)\n", _name.c_str(), _z_index, _transform.position.x, _transform.position.y);

    for (const auto& [name, node] : _nodes) {
        node->print_tree(indent + 2);
    }
}

Transform Node2D::get_global_transform() const {
    if (_parent) {
        auto [position, scale, rotation] = _parent->get_global_transform();

        Transform global;
        global.position = position + _transform.position;
        global.scale    = scale * _transform.scale;
        global.rotation = rotation + _transform.rotation;

        return global;
    }

    return _transform;
}

Transform Node2D::get_transform() const {
    return _transform;
}

void Node2D::set_transform(const Transform& transform) {
    _transform = transform;
}


Node2D::~Node2D() {
    for (auto it = _nodes.begin(); it != _nodes.end(); ++it) {
        delete it->second;
    }
}

void Node2D::add_child(const std::string& name, Node2D* node) {
    node->_parent = this;
    node->_name   = name;

    _nodes.emplace(name, node);

    auto insert_pos = std::ranges::lower_bound(_draw_list, node,
        [](const Node2D* a, const Node2D* b) { return a->_z_index < b->_z_index; });

    _draw_list.insert(insert_pos, node);
}


Node2D* Node2D::get_node(const std::string& path) {
    const size_t slash = path.find('/');


    const std::string head = (slash == std::string::npos) ? path : path.substr(0, slash);
    const std::string tail = (slash == std::string::npos) ? "" : path.substr(slash + 1);

    const auto it = _nodes.find(head);
    if (it == _nodes.end()) {
        LOG_ERROR("Node with name %s wasn't found", head.c_str());
        return nullptr;
    }

    if (tail.empty()) {
        return it->second;
    }

    return it->second->get_node(tail);
}

void Node2D::change_visibility(bool visible) {
    _is_visible = visible;
}

void Node2D::ready() {
}

void Node2D::process(double delta_time) {
    for (auto* child : _draw_list) {
        child->process(delta_time);
    }
}


void Node2D::draw(Renderer* renderer) {
    for (auto* child : _draw_list) {
        if (!child->_is_visible) {
            continue;
        }

        child->draw(renderer);
    }
}


void Node2D::input(const InputManager* input) {
    for (auto& [name, child] : _nodes) {
        child->input(input);
    }
}
