#ifndef SRC_ENTITY_H_
#define SRC_ENTITY_H_

#include <glm/glm.hpp>
#include <vector>

using std::vector;

// Drawable interface.
class Drawable {
  public:
    virtual void draw() = 0;
};

// The Entity class provides some very minimal scene graph functionality.
// All nodes that are decendents of Renderer::rootEntity() are drawn.
// Parent transform are multiplied in to children but the other attrs are independent of parents.
// Not memory managed. All Entitys must be created and destroyed by client.
class Entity : public Drawable {
  public:
    Entity();
    ~Entity();
    // =====Virtuals=====
    virtual void update(float delta_time) {}
    virtual void draw() {}
    virtual void drawOccluder() {}
    virtual void extent(glm::vec2 *min, glm::vec2 *max) { *min = glm::vec2(0.0f); *max = glm::vec2(0.0f); }

    // Sets the drawable parent. Setting parent to NULL removes this drawable and all children from the scene graph.
    Entity *parent() { return parent_; }
    void setParent(Entity *parent);

    // Get the full transform of drawable element.
    glm::mat3 fullTransform();
    // Get the transform relative to the parent drawable
    glm::mat3 relativeTransform() { return relative_transform_; }
    void setRelativeTransform(const glm::mat3 &transform) { relative_transform_ = transform; }    
    // We care about order cause we render in flatland.
    float displayPriority() const { return priority_; }
    void setDisplayPriority(float priority) { priority_ = priority; }
    // Checks if shape extent is onscreen.
    bool onScreen();
    
    // =====Toggles=====
    // Controls visibility.
    bool isVisible() const { return is_visible_; }
    void setIsVisible(bool visible) { is_visible_ = visible; }
    // Controls whether or not to consider this shape an occluder while shading.
    bool isOccluder() const { return is_occluder_; }
    void setIsOccluder(bool occluder) { is_occluder_ = occluder; }
    float occluderColor() { return occluder_color_; }
    void setOccluderColor(float color) { occluder_color_ = color; }
    // Control whether or not to call the update function on this entity.
    bool doUpdate() const { return do_update_; }
    void setDoUpdate(bool update) { do_update_ = update; }

    // =====For engine use=====
    void drawAll();
    void drawAllOccluders();
    void updateAll(float delta_time);
  private:
    // This would either make our links madness or we would need to mem manage the scene graph.
    // So no copy!
    Entity(const Entity &other);
    Entity& operator=(Entity other);
    // Helpers
    void addChild(Entity *child);
    void removeChild(Entity *child);
    void lock(Entity *locking_ancestor);
    // Member data.
    Entity *parent_;
    vector<Entity *> children_;
    glm::mat3 relative_transform_;
    float priority_;
    Entity *locking_ancestor_;
    bool is_occluder_, is_visible_, do_update_;
    float occluder_color_;
};

#endif  // SRC_ENTITY_H_
