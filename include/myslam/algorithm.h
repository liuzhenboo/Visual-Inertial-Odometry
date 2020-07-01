#ifndef MYSLAM_ALGORITHM_H
#define MYSLAM_ALGORITHM_H

// algorithms used in myslam
#include "myslam/common_include.h"

namespace myslam {

/**
 * linear triangulation with SVD
 * @param poses     poses,
 * @param points    points in normalized plane
 * @param pt_world  triangulated point in the world
 * @return true if success
 */
inline bool triangulation1(const std::vector<SE3> &poses,
                   const std::vector<Vec3> points, Vec3 &pt_world) {
    MatXX A(2 * poses.size(), 4);
    VecX b(2 * poses.size());
    b.setZero();
    for (size_t i = 0; i < poses.size(); ++i) {
        Mat34 m = poses[i].matrix3x4();
        A.block<1, 4>(2 * i, 0) = points[i][0] * m.row(2) - m.row(0);
        A.block<1, 4>(2 * i + 1, 0) = points[i][1] * m.row(2) - m.row(1);
    }
    auto svd = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
    pt_world = (svd.matrixV().col(3) / svd.matrixV()(3, 3)).head<3>();
    //std::cout << svd.singularValues()[3] / svd.singularValues()[2] << std::endl;
    if (svd.singularValues()[3] / svd.singularValues()[2] < 1e-2) {
        // 解质量不好，放弃
        return true;
    }
    return false;
}

/**
 * 平行双目三角化
 * @param poses     poses,
 * @param points    points in normalized plane
 * @param pt_world  triangulated point in the world
 * @return true if success
 */
// s*x_r^*x_l = -x_r^*t
inline bool triangulation2(const std::vector<SE3> &poses,
                   const std::vector<Vec3> points, Vec3 &pt_world){

    //求深度s
    Vec3 x_l = points[0];
    Vec3 x_r = points[1];
    Vec3 t = poses[1].translation();
    Vec3 s;
    s[0] = -(-x_r[2]*t[1] + x_r[1]*t[2])/(-x_r[2]*x_l[1] + x_r[1]*x_l[2]); 
    s[1] = -(x_r[2]*t[0] - x_r[0]*t[2])/(x_r[2]*x_l[0] - x_r[0]*x_l[2]);
    s[2] = -(-x_r[1]*t[0] + x_r[0]*t[1])/(-x_r[1]*x_l[0] + x_r[0]*x_l[1]);
    std::cout << "s:" << s[0] << ", " << s[1] << ", " << s[2] << std::endl;
    if(s[2] > 0)
    {
        pt_world = s;
        return true;
    }
    else
    {
        return false;
    }
}
inline bool triangulation(const std::vector<SE3> &poses,
                   const std::vector<Vec3> points, Vec3 &pt_world){

    //求深度s
    Vec3 x_l = points[0];
    Vec3 x_r = points[1];
    Vec3 t = poses[1].translation();
    double s;
    s = t[1]/(x_r[1] - x_l[1]);
    if(s > 0)
    {
        pt_world = s*x_l;
        std::cout << "s:" << s << std::endl;
        return true;
    }
    else
    {
        return false;
    }
    
    
    
}
// converters
inline Vec2 toVec2(const cv::Point2f p) { return Vec2(p.x, p.y); }

}  // namespace myslam

#endif  // MYSLAM_ALGORITHM_H
