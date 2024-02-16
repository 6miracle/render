#ifndef __RENDER_BVH_HPP__
#define __RENDER_BVH_HPP__

#include "Engine/Model/model.h"
#include "pch.h"
#include "tools/AABB.hpp"
#include "util.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace render {
// 两种设计方式
// 设置一个node类和一个BVH加速结构类
// 或者只设置一个单独的BVH类
class BVH: public Model {
public:
    BVH() = default;
    BVH(const std::vector<std::shared_ptr<Model>>& objects): BVH(objects, 0, objects.size()) { }
    BVH(Objects& obj): BVH(obj.objects(), 0, obj.size()) { }
    BVH(const std::vector<std::shared_ptr<Model>>& src_objects, size_t start, size_t end) { 
        size_ = end - start ;
        auto objects = src_objects;
        int axis = random_int(0, 2);
        // std::cout << start << " " << end << '\n';
        auto compare = (axis == 0) ? xCompare
                    : (axis == 1) ? yCompare
                    : zCompare;
        size_t object_span = end - start;
        if(object_span == 1) {
            left_ = right_ = objects[start];
        } else if(object_span == 2) {
            if(compare(objects[start], objects[start + 1])) {
                left_ = objects[start];
                right_ = objects[start + 1];
            }else {
                left_ = objects[start + 1];
                right_ = objects[start];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, compare);
            auto mid = start + object_span / 2;
            left_ = std::make_shared<BVH>(objects, start, mid);
            right_ = std::make_shared<BVH>(objects, mid, end);
        //    std::cout << "total " << size_  << " l=" << std::dynamic_pointer_cast<BVH>(left_)->size() 
        //     << " r="<<std::dynamic_pointer_cast<BVH>(right_)->size() <<'\n';
        }
        bbox_ = Bounds(left_->bbBox(), right_->bbBox());
        // std::cout << "total " << size_  << '\n';
    }
    bool hit(const Ray& r, double ray_tmin, double ray_tmax, Intersection& rec) const override { 
        if(!bbox_.hit(r, ray_tmin, ray_tmax)) {
            return false;
        }
        bool hit_left = left_->hit(r, ray_tmin, ray_tmax, rec);
        bool hit_right = right_->hit(r, ray_tmin, hit_left ? rec.t : ray_tmax, rec);
        return hit_left || hit_right;
    }
    Type type() const override { return Model::Type::BVH; }  

    static bool xCompare(const std::shared_ptr<Model>& m1, const std::shared_ptr<Model>& m2) {
        return m1->bbBox().Min().x() < m2->bbBox().Min().x();
    }
    static bool yCompare(const std::shared_ptr<Model>& m1, const std::shared_ptr<Model>& m2) {
        return m1->bbBox().Min().y() < m2->bbBox().Min().y();
    }
    static bool zCompare(const std::shared_ptr<Model>& m1, const std::shared_ptr<Model>& m2) {
        return m1->bbBox().Min().z() < m2->bbBox().Min().z();
    }
    Bounds bbBox() const override { return bbox_; }

    //  int size() const { return size_; }

private:
    Bounds bbox_;
    std::shared_ptr<Model> left_;
    std::shared_ptr<Model> right_;
    int size_;
};

}
#endif