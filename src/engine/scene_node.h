#ifndef SRC_SCENE_NODE_H_
#define SRC_SCENE_NODE_H_

#include <glm/glm.hpp>
#include <vector>

using std::vector;

// Drawable interface.
class Drawable {
  public:
    virtual void draw() = 0;
};

// The SceneNode class provides some very minimal scene graph functionality.
// All nodes that are decendents of Renderer::rootNode() are drawn.
// Parent transform are multiplied in to children but the other attrs are independent of parents.
// Not memory managed. All SceneNodes must be created and destroyed by client.
class SceneNode : public Drawable {
  public:
    SceneNode();
    ~SceneNode();
    // Drawables should redefine these.
    virtual void draw() {}
    virtual void drawOccluder() {}
    virtual void extent(glm::vec2 *min, glm::vec2 *max) { *min = glm::vec2(0.0f); *max = glm::vec2(0.0f); }
    // Checks if shape extent is onscreen.
    bool onScreen();
    // Sets the drawable parent. Setting parent to NULL removes this drawable and all children from the scene graph.
    SceneNode *parent() { return parent_; }
    void setParent(SceneNode *parent);
    // Locks children to have all the same visibility, occlusion and priority of this node.
    void lockChildren();
    bool isLocking() { return is_locking_; }
    bool isLocked() { return is_locked_; }
    // Get all visible descendents sorted by priority for rendering.
    void getSortedVisibleDescendants(vector<SceneNode *> *drawables);
    // Get the full transform of drawable element.
    glm::mat3 fullTransform();
    // Get the transform relative to the parent drawable
    glm::mat3 relativeTransform() { return relative_transform_; }
    void setRelativeTransform(const glm::mat3 &transform) { relative_transform_ = transform; }
    // We care about order cause we render in flatland.
    float displayPriority() const { return priority_; }
    void setDisplayPriority(float priority) { priority_ = priority; }
    // Controls visibility.
    bool isVisible();
    void setIsVisible(bool visible) { is_visible_ = visible; }
    // Controls whether or not to consider this shape an occluder while shading.
    bool isOccluder();
    void setIsOccluder(bool occluder) { is_occluder_ = occluder; }
    float occluderColor() { return occluder_color_; }
    void setOccluderColor(float color) { occluder_color_ = color; }
    // Controls whether or not to draw this shape to the stencil buffer for the stencil test before the 3D is drawn.
    bool is3DStencil();
    void setIs3DStencil(bool stencil) { is_3D_stencil_ = stencil; }
  private:
    // This would either make our links madness or we would need to mem manage the scene graph.
    // So no copy!
    SceneNode(const SceneNode &other);
    SceneNode& operator=(SceneNode other);
    // Helpers
    void addChild(SceneNode *child);
    void removeChild(SceneNode *child);
    void lock(SceneNode *locking_ancestor);
    void getDescendants(vector<SceneNode *> *drawables);
    void getNonLockedDescendants(vector<SceneNode *> *drawables);
    // Member data.
    SceneNode *parent_;
    vector<SceneNode *> children_;
    glm::mat3 relative_transform_;
    float priority_;
    SceneNode *locking_ancestor_;
    bool is_occluder_, is_3D_stencil_, is_visible_, is_locking_, is_locked_;
    float occluder_color_;
};

#endif  // SRC_SCENE_NODE_H_
