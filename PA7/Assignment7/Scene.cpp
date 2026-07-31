//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    Intersection inter = intersect(ray);
    if (!inter.happened) {
        // Only primary rays see the background/sky color.
        // Secondary rays that escape the box contribute zero radiance,
        // since the only light source in the Cornell box is the ceiling light.
        if (depth == 0)
            return this->backgroundColor;
        else
            return Vector3f(0.0f);
    }
    Vector3f p = inter.coords;
    Vector3f N = inter.normal;
    Material *mat = inter.m;
    Vector3f wo = normalize(-ray.direction);

    if (inter.m->hasEmission()) {
        return inter.m->getEmission();
    }
    Vector3f L_dir = 0.0f;
    float pdf_light;
    Intersection lightInter;
    sampleLight(lightInter, pdf_light);
    Vector3f lightDir = lightInter.coords - p;
    float lightDist2 = dotProduct(lightDir, lightDir);
    Vector3f wi = normalize(lightDir);

    Vector3f shadowOrigin = dotProduct(wi, N) > 0 ? p + N * EPSILON
                                                  : p - N * EPSILON;
    Ray shadowRay(shadowOrigin, wi);
    Intersection shadowInter = intersect(shadowRay);

    bool inShadow = shadowInter.happened &&
         shadowInter.distance * shadowInter.distance < lightDist2 - 0.001f;


    if (!inShadow) {
        Vector3f fr = mat->eval(wi, wo, N);
        float cosTheta = std::max(0.0f, dotProduct(wi, N));
        float cosThetaP = std::max(0.0f, dotProduct(-wi, lightInter.normal));
        L_dir = lightInter.emit * fr * cosTheta * cosThetaP
                / lightDist2 / pdf_light;
    }
    
    //indirect illumination
    if (get_random_float() > RussianRoulette) {
        return L_dir;
    }
    Vector3f L_indir = 0.0f;
    Vector3f i_wi = mat->sample(wo, N);
    Vector3f nextOrig = dotProduct(i_wi, N) > 0 ? p + N * EPSILON : p - N * EPSILON;
    Ray nextRay(nextOrig, i_wi);
    float pdf_brdf = mat->pdf(i_wi, wo, N);
    Vector3f fr_i = mat->eval(i_wi, wo, N);
    float cosTheta_i = std::max(0.0f, dotProduct(i_wi, N));

    L_indir = castRay(nextRay, depth+1) * cosTheta_i * fr_i
    / pdf_brdf / RussianRoulette;

    return L_dir + L_indir;
}