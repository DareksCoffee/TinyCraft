#include <graphics/frustum.h>
#include <math.h>

void frustum_from_matrices(Frustum* frustum, mat4 view, mat4 projection)
{
    mat4 vp;
    glm_mat4_mul(projection, view, vp);

    frustum->planes[0][0] = vp[0][3] + vp[0][0];
    frustum->planes[0][1] = vp[1][3] + vp[1][0];
    frustum->planes[0][2] = vp[2][3] + vp[2][0];
    frustum->planes[0][3] = vp[3][3] + vp[3][0];

    frustum->planes[1][0] = vp[0][3] - vp[0][0];
    frustum->planes[1][1] = vp[1][3] - vp[1][0];
    frustum->planes[1][2] = vp[2][3] - vp[2][0];
    frustum->planes[1][3] = vp[3][3] - vp[3][0];

    frustum->planes[2][0] = vp[0][3] + vp[0][1];
    frustum->planes[2][1] = vp[1][3] + vp[1][1];
    frustum->planes[2][2] = vp[2][3] + vp[2][1];
    frustum->planes[2][3] = vp[3][3] + vp[3][1];

    frustum->planes[3][0] = vp[0][3] - vp[0][1];
    frustum->planes[3][1] = vp[1][3] - vp[1][1];
    frustum->planes[3][2] = vp[2][3] - vp[2][1];
    frustum->planes[3][3] = vp[3][3] - vp[3][1];

    frustum->planes[4][0] = vp[0][3] + vp[0][2];
    frustum->planes[4][1] = vp[1][3] + vp[1][2];
    frustum->planes[4][2] = vp[2][3] + vp[2][2];
    frustum->planes[4][3] = vp[3][3] + vp[3][2];

    frustum->planes[5][0] = vp[0][3] - vp[0][2];
    frustum->planes[5][1] = vp[1][3] - vp[1][2];
    frustum->planes[5][2] = vp[2][3] - vp[2][2];
    frustum->planes[5][3] = vp[3][3] - vp[3][2];

    for(int i = 0; i < 6; i++) {
        float length = sqrtf(frustum->planes[i][0] * frustum->planes[i][0] +
                           frustum->planes[i][1] * frustum->planes[i][1] +
                           frustum->planes[i][2] * frustum->planes[i][2]);
        frustum->planes[i][0] /= length;
        frustum->planes[i][1] /= length;
        frustum->planes[i][2] /= length;
        frustum->planes[i][3] /= length;
    }
}

int frustum_point_inside(Frustum* frustum, vec3 point)
{
    for(int i = 0; i < 6; i++) {
        float distance = frustum->planes[i][0] * point[0] +
                        frustum->planes[i][1] * point[1] +
                        frustum->planes[i][2] * point[2] +
                        frustum->planes[i][3];
        if(distance < 0.0f)
            return 0;
    }
    return 1;
}

int frustum_sphere_inside(Frustum* frustum, vec3 center, float radius)
{
    for(int i = 0; i < 6; i++) {
        float distance = frustum->planes[i][0] * center[0] +
                        frustum->planes[i][1] * center[1] +
                        frustum->planes[i][2] * center[2] +
                        frustum->planes[i][3];
        if(distance < -radius)
            return 0;
    }
    return 1;
}

int frustum_aabb_inside(Frustum* frustum, vec3 min, vec3 max)
{
    for(int i = 0; i < 6; i++) {
        vec3 p_vertex, n_vertex;
        
        p_vertex[0] = (frustum->planes[i][0] > 0.0f) ? max[0] : min[0];
        p_vertex[1] = (frustum->planes[i][1] > 0.0f) ? max[1] : min[1];
        p_vertex[2] = (frustum->planes[i][2] > 0.0f) ? max[2] : min[2];
        
        n_vertex[0] = (frustum->planes[i][0] > 0.0f) ? min[0] : max[0];
        n_vertex[1] = (frustum->planes[i][1] > 0.0f) ? min[1] : max[1];
        n_vertex[2] = (frustum->planes[i][2] > 0.0f) ? min[2] : max[2];
        
        float p_dist = frustum->planes[i][0] * p_vertex[0] +
                      frustum->planes[i][1] * p_vertex[1] +
                      frustum->planes[i][2] * p_vertex[2] +
                      frustum->planes[i][3];
        
        if(p_dist < 0.0f)
            return 0;
    }
    return 1;
}
